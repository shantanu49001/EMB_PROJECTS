/***************************************************************************//**
* \file UART_drv.c
* \version 3.20
*
* \brief
*  This file contains the Endpoint 0 Driver for the USBFS Component.  
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART_pvt.h"
#include "cyapicallbacks.h"


/***************************************
* Global data allocation
***************************************/

volatile T_UART_EP_CTL_BLOCK UART_EP[UART_MAX_EP];

/** Contains the current configuration number, which is set by the host using a 
 * SET_CONFIGURATION request. This variable is initialized to zero in 
 * USBFS_InitComponent() API and can be read by the USBFS_GetConfiguration() 
 * API.*/
volatile uint8 UART_configuration;

/** Contains the current interface number.*/
volatile uint8 UART_interfaceNumber;

/** This variable is set to one after SET_CONFIGURATION and SET_INTERFACE 
 *requests. It can be read by the USBFS_IsConfigurationChanged() API */
volatile uint8 UART_configurationChanged;

/** Contains the current device address.*/
volatile uint8 UART_deviceAddress;

/** This is a two-bit variable that contains power status in the bit 0 
 * (DEVICE_STATUS_BUS_POWERED or DEVICE_STATUS_SELF_POWERED) and remote wakeup 
 * status (DEVICE_STATUS_REMOTE_WAKEUP) in the bit 1. This variable is 
 * initialized to zero in USBFS_InitComponent() API, configured by the 
 * USBFS_SetPowerStatus() API. The remote wakeup status cannot be set using the 
 * API SetPowerStatus(). */
volatile uint8 UART_deviceStatus;

volatile uint8 UART_interfaceSetting[UART_MAX_INTERFACES_NUMBER];
volatile uint8 UART_interfaceSetting_last[UART_MAX_INTERFACES_NUMBER];
volatile uint8 UART_interfaceStatus[UART_MAX_INTERFACES_NUMBER];

/** Contains the started device number. This variable is set by the 
 * USBFS_Start() or USBFS_InitComponent() APIs.*/
volatile uint8 UART_device;

/** Initialized class array for each interface. It is used for handling Class 
 * specific requests depend on interface class. Different classes in multiple 
 * alternate settings are not supported.*/
const uint8 CYCODE *UART_interfaceClass;


/***************************************
* Local data allocation
***************************************/

volatile uint8  UART_ep0Toggle;
volatile uint8  UART_lastPacketSize;

/** This variable is used by the communication functions to handle the current 
* transfer state.
* Initialized to TRANS_STATE_IDLE in the USBFS_InitComponent() API and after a 
* complete transfer in the status stage.
* Changed to the TRANS_STATE_CONTROL_READ or TRANS_STATE_CONTROL_WRITE in setup 
* transaction depending on the request type.
*/
volatile uint8  UART_transferState;
volatile T_UART_TD UART_currentTD;
volatile uint8  UART_ep0Mode;
volatile uint8  UART_ep0Count;
volatile uint16 UART_transferByteCount;


/*******************************************************************************
* Function Name: UART_ep_0_Interrupt
****************************************************************************//**
*
*  This Interrupt Service Routine handles Endpoint 0 (Control Pipe) traffic.
*  It dispatches setup requests and handles the data and status stages.
*
*
*******************************************************************************/
CY_ISR(UART_EP_0_ISR)
{
    uint8 tempReg;
    uint8 modifyReg;

#ifdef UART_EP_0_ISR_ENTRY_CALLBACK
    UART_EP_0_ISR_EntryCallback();
#endif /* (UART_EP_0_ISR_ENTRY_CALLBACK) */
    
    tempReg = UART_EP0_CR_REG;
    if ((tempReg & UART_MODE_ACKD) != 0u)
    {
        modifyReg = 1u;
        if ((tempReg & UART_MODE_SETUP_RCVD) != 0u)
        {
            if ((tempReg & UART_MODE_MASK) != UART_MODE_NAK_IN_OUT)
            {
                /* Mode not equal to NAK_IN_OUT: invalid setup */
                modifyReg = 0u;
            }
            else
            {
                UART_HandleSetup();
                
                if ((UART_ep0Mode & UART_MODE_SETUP_RCVD) != 0u)
                {
                    /* SETUP bit set: exit without mode modificaiton */
                    modifyReg = 0u;
                }
            }
        }
        else if ((tempReg & UART_MODE_IN_RCVD) != 0u)
        {
            UART_HandleIN();
        }
        else if ((tempReg & UART_MODE_OUT_RCVD) != 0u)
        {
            UART_HandleOUT();
        }
        else
        {
            modifyReg = 0u;
        }
        
        /* Modify the EP0_CR register */
        if (modifyReg != 0u)
        {
            
            tempReg = UART_EP0_CR_REG;
            
            /* Make sure that SETUP bit is cleared before modification */
            if ((tempReg & UART_MODE_SETUP_RCVD) == 0u)
            {
                /* Update count register */
                tempReg = (uint8) UART_ep0Toggle | UART_ep0Count;
                UART_EP0_CNT_REG = tempReg;
               
                /* Make sure that previous write operaiton was successful */
                if (tempReg == UART_EP0_CNT_REG)
                {
                    /* Repeat until next successful write operation */
                    do
                    {
                        /* Init temporary variable */
                        modifyReg = UART_ep0Mode;
                        
                        /* Unlock register */
                        tempReg = (uint8) (UART_EP0_CR_REG & UART_MODE_SETUP_RCVD);
                        
                        /* Check if SETUP bit is not set */
                        if (0u == tempReg)
                        {
                            /* Set the Mode Register  */
                            UART_EP0_CR_REG = UART_ep0Mode;
                            
                            /* Writing check */
                            modifyReg = UART_EP0_CR_REG & UART_MODE_MASK;
                        }
                    }
                    while (modifyReg != UART_ep0Mode);
                }
            }
        }
    }

    UART_ClearSieInterruptSource(UART_INTR_SIE_EP0_INTR);
	
#ifdef UART_EP_0_ISR_EXIT_CALLBACK
    UART_EP_0_ISR_ExitCallback();
#endif /* (UART_EP_0_ISR_EXIT_CALLBACK) */
}


/*******************************************************************************
* Function Name: UART_HandleSetup
****************************************************************************//**
*
*  This Routine dispatches requests for the four USB request types
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_HandleSetup(void) 
{
    uint8 requestHandled;
    
    /* Clear register lock by SIE (read register) and clear setup bit 
    * (write any value in register).
    */
    requestHandled = (uint8) UART_EP0_CR_REG;
    UART_EP0_CR_REG = (uint8) requestHandled;
    requestHandled = (uint8) UART_EP0_CR_REG;

    if ((requestHandled & UART_MODE_SETUP_RCVD) != 0u)
    {
        /* SETUP bit is set: exit without mode modification. */
        UART_ep0Mode = requestHandled;
    }
    else
    {
        /* In case the previous transfer did not complete, close it out */
        UART_UpdateStatusBlock(UART_XFER_PREMATURE);

        /* Check request type. */
        switch (UART_bmRequestTypeReg & UART_RQST_TYPE_MASK)
        {
            case UART_RQST_TYPE_STD:
                requestHandled = UART_HandleStandardRqst();
                break;
                
            case UART_RQST_TYPE_CLS:
                requestHandled = UART_DispatchClassRqst();
                break;
                
            case UART_RQST_TYPE_VND:
                requestHandled = UART_HandleVendorRqst();
                break;
                
            default:
                requestHandled = UART_FALSE;
                break;
        }
        
        /* If request is not recognized. Stall endpoint 0 IN and OUT. */
        if (requestHandled == UART_FALSE)
        {
            UART_ep0Mode = UART_MODE_STALL_IN_OUT;
        }
    }
}


/*******************************************************************************
* Function Name: UART_HandleIN
****************************************************************************//**
*
*  This routine handles EP0 IN transfers.
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_HandleIN(void) 
{
    switch (UART_transferState)
    {
        case UART_TRANS_STATE_IDLE:
            break;
        
        case UART_TRANS_STATE_CONTROL_READ:
            UART_ControlReadDataStage();
            break;
            
        case UART_TRANS_STATE_CONTROL_WRITE:
            UART_ControlWriteStatusStage();
            break;
            
        case UART_TRANS_STATE_NO_DATA_CONTROL:
            UART_NoDataControlStatusStage();
            break;
            
        default:    /* there are no more states */
            break;
    }
}


/*******************************************************************************
* Function Name: UART_HandleOUT
****************************************************************************//**
*
*  This routine handles EP0 OUT transfers.
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_HandleOUT(void) 
{
    switch (UART_transferState)
    {
        case UART_TRANS_STATE_IDLE:
            break;
        
        case UART_TRANS_STATE_CONTROL_READ:
            UART_ControlReadStatusStage();
            break;
            
        case UART_TRANS_STATE_CONTROL_WRITE:
            UART_ControlWriteDataStage();
            break;
            
        case UART_TRANS_STATE_NO_DATA_CONTROL:
            /* Update the completion block */
            UART_UpdateStatusBlock(UART_XFER_ERROR);
            
            /* We expect no more data, so stall INs and OUTs */
            UART_ep0Mode = UART_MODE_STALL_IN_OUT;
            break;
            
        default:    
            /* There are no more states */
            break;
    }
}


/*******************************************************************************
* Function Name: UART_LoadEP0
****************************************************************************//**
*
*  This routine loads the EP0 data registers for OUT transfers. It uses the
*  currentTD (previously initialized by the _InitControlWrite function and
*  updated for each OUT transfer, and the bLastPacketSize) to determine how
*  many uint8s to transfer on the current OUT.
*
*  If the number of uint8s remaining is zero and the last transfer was full,
*  we need to send a zero length packet.  Otherwise we send the minimum
*  of the control endpoint size (8) or remaining number of uint8s for the
*  transaction.
*
*
* \globalvars
*  UART_transferByteCount - Update the transfer byte count from the
*     last transaction.
*  UART_ep0Count - counts the data loaded to the SIE memory in
*     current packet.
*  UART_lastPacketSize - remembers the USBFS_ep0Count value for the
*     next packet.
*  UART_transferByteCount - sum of the previous bytes transferred
*     on previous packets(sum of USBFS_lastPacketSize)
*  UART_ep0Toggle - inverted
*  UART_ep0Mode  - prepare for mode register content.
*  UART_transferState - set to TRANS_STATE_CONTROL_READ
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_LoadEP0(void) 
{
    uint8 ep0Count = 0u;

    /* Update the transfer byte count from the last transaction */
    UART_transferByteCount += UART_lastPacketSize;

    /* Now load the next transaction */
    while ((UART_currentTD.count > 0u) && (ep0Count < 8u))
    {
        UART_EP0_DR_BASE.epData[ep0Count] = (uint8) *UART_currentTD.pData;
        UART_currentTD.pData = &UART_currentTD.pData[1u];
        ep0Count++;
        UART_currentTD.count--;
    }

    /* Support zero-length packet */
    if ((UART_lastPacketSize == 8u) || (ep0Count > 0u))
    {
        /* Update the data toggle */
        UART_ep0Toggle ^= UART_EP0_CNT_DATA_TOGGLE;
        /* Set the Mode Register  */
        UART_ep0Mode = UART_MODE_ACK_IN_STATUS_OUT;
        /* Update the state (or stay the same) */
        UART_transferState = UART_TRANS_STATE_CONTROL_READ;
    }
    else
    {
        /* Expect Status Stage Out */
        UART_ep0Mode = UART_MODE_STATUS_OUT_ONLY;
        /* Update the state (or stay the same) */
        UART_transferState = UART_TRANS_STATE_CONTROL_READ;
    }

    /* Save the packet size for next time */
    UART_ep0Count =       (uint8) ep0Count;
    UART_lastPacketSize = (uint8) ep0Count;
}


/*******************************************************************************
* Function Name: UART_InitControlRead
****************************************************************************//**
*
*  Initialize a control read transaction. It is used to send data to the host.
*  The following global variables should be initialized before this function
*  called. To send zero length packet use InitZeroLengthControlTransfer
*  function.
*
*
* \return
*  requestHandled state.
*
* \globalvars
*  UART_currentTD.count - counts of data to be sent.
*  UART_currentTD.pData - data pointer.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 UART_InitControlRead(void) 
{
    uint16 xferCount;

    if (UART_currentTD.count == 0u)
    {
        (void) UART_InitZeroLengthControlTransfer();
    }
    else
    {
        /* Set up the state machine */
        UART_transferState = UART_TRANS_STATE_CONTROL_READ;
        
        /* Set the toggle, it gets updated in LoadEP */
        UART_ep0Toggle = 0u;
        
        /* Initialize the Status Block */
        UART_InitializeStatusBlock();
        
        xferCount = ((uint16)((uint16) UART_lengthHiReg << 8u) | ((uint16) UART_lengthLoReg));

        if (UART_currentTD.count > xferCount)
        {
            UART_currentTD.count = xferCount;
        }
        
        UART_LoadEP0();
    }

    return (UART_TRUE);
}


/*******************************************************************************
* Function Name: UART_InitZeroLengthControlTransfer
****************************************************************************//**
*
*  Initialize a zero length data IN transfer.
*
* \return
*  requestHandled state.
*
* \globalvars
*  UART_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*  UART_ep0Mode  - prepare for mode register content.
*  UART_transferState - set to TRANS_STATE_CONTROL_READ
*  UART_ep0Count - cleared, means the zero-length packet.
*  UART_lastPacketSize - cleared.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 UART_InitZeroLengthControlTransfer(void)
                                                
{
    /* Update the state */
    UART_transferState = UART_TRANS_STATE_CONTROL_READ;
    
    /* Set the data toggle */
    UART_ep0Toggle = UART_EP0_CNT_DATA_TOGGLE;
    
    /* Set the Mode Register  */
    UART_ep0Mode = UART_MODE_ACK_IN_STATUS_OUT;
    
    /* Save the packet size for next time */
    UART_lastPacketSize = 0u;
    
    UART_ep0Count = 0u;

    return (UART_TRUE);
}


/*******************************************************************************
* Function Name: UART_ControlReadDataStage
****************************************************************************//**
*
*  Handle the Data Stage of a control read transfer.
*
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_ControlReadDataStage(void) 

{
    UART_LoadEP0();
}


/*******************************************************************************
* Function Name: UART_ControlReadStatusStage
****************************************************************************//**
*
*  Handle the Status Stage of a control read transfer.
*
*
* \globalvars
*  UART_USBFS_transferByteCount - updated with last packet size.
*  UART_transferState - set to TRANS_STATE_IDLE.
*  UART_ep0Mode  - set to MODE_STALL_IN_OUT.
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_ControlReadStatusStage(void) 
{
    /* Update the transfer byte count */
    UART_transferByteCount += UART_lastPacketSize;
    
    /* Go Idle */
    UART_transferState = UART_TRANS_STATE_IDLE;
    
    /* Update the completion block */
    UART_UpdateStatusBlock(UART_XFER_STATUS_ACK);
    
    /* We expect no more data, so stall INs and OUTs */
    UART_ep0Mode = UART_MODE_STALL_IN_OUT;
}


/*******************************************************************************
* Function Name: UART_InitControlWrite
****************************************************************************//**
*
*  Initialize a control write transaction
*
* \return
*  requestHandled state.
*
* \globalvars
*  UART_USBFS_transferState - set to TRANS_STATE_CONTROL_WRITE
*  UART_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*  UART_ep0Mode  - set to MODE_ACK_OUT_STATUS_IN
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 UART_InitControlWrite(void) 
{
    uint16 xferCount;

    /* Set up the state machine */
    UART_transferState = UART_TRANS_STATE_CONTROL_WRITE;
    
    /* This might not be necessary */
    UART_ep0Toggle = UART_EP0_CNT_DATA_TOGGLE;
    
    /* Initialize the Status Block */
    UART_InitializeStatusBlock();

    xferCount = ((uint16)((uint16) UART_lengthHiReg << 8u) | ((uint16) UART_lengthLoReg));

    if (UART_currentTD.count > xferCount)
    {
        UART_currentTD.count = xferCount;
    }

    /* Expect Data or Status Stage */
    UART_ep0Mode = UART_MODE_ACK_OUT_STATUS_IN;

    return(UART_TRUE);
}


/*******************************************************************************
* Function Name: UART_ControlWriteDataStage
****************************************************************************//**
*
*  Handle the Data Stage of a control write transfer
*       1. Get the data (We assume the destination was validated previously)
*       2. Update the count and data toggle
*       3. Update the mode register for the next transaction
*
*
* \globalvars
*  UART_transferByteCount - Update the transfer byte count from the
*    last transaction.
*  UART_ep0Count - counts the data loaded from the SIE memory
*    in current packet.
*  UART_transferByteCount - sum of the previous bytes transferred
*    on previous packets(sum of USBFS_lastPacketSize)
*  UART_ep0Toggle - inverted
*  UART_ep0Mode  - set to MODE_ACK_OUT_STATUS_IN.
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_ControlWriteDataStage(void) 
{
    uint8 ep0Count;
    uint8 regIndex = 0u;

    ep0Count = (UART_EP0_CNT_REG & UART_EPX_CNT0_MASK) - UART_EPX_CNTX_CRC_COUNT;

    UART_transferByteCount += (uint8)ep0Count;

    while ((UART_currentTD.count > 0u) && (ep0Count > 0u))
    {
        *UART_currentTD.pData = (uint8) UART_EP0_DR_BASE.epData[regIndex];
        UART_currentTD.pData = &UART_currentTD.pData[1u];
        regIndex++;
        ep0Count--;
        UART_currentTD.count--;
    }
    
    UART_ep0Count = (uint8)ep0Count;
    
    /* Update the data toggle */
    UART_ep0Toggle ^= UART_EP0_CNT_DATA_TOGGLE;
    
    /* Expect Data or Status Stage */
    UART_ep0Mode = UART_MODE_ACK_OUT_STATUS_IN;
}


/*******************************************************************************
* Function Name: UART_ControlWriteStatusStage
****************************************************************************//**
*
*  Handle the Status Stage of a control write transfer
*
* \globalvars
*  UART_transferState - set to TRANS_STATE_IDLE.
*  UART_USBFS_ep0Mode  - set to MODE_STALL_IN_OUT.
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_ControlWriteStatusStage(void) 
{
    /* Go Idle */
    UART_transferState = UART_TRANS_STATE_IDLE;
    
    /* Update the completion block */    
    UART_UpdateStatusBlock(UART_XFER_STATUS_ACK);
    
    /* We expect no more data, so stall INs and OUTs */
    UART_ep0Mode = UART_MODE_STALL_IN_OUT;
}


/*******************************************************************************
* Function Name: UART_InitNoDataControlTransfer
****************************************************************************//**
*
*  Initialize a no data control transfer
*
* \return
*  requestHandled state.
*
* \globalvars
*  UART_transferState - set to TRANS_STATE_NO_DATA_CONTROL.
*  UART_ep0Mode  - set to MODE_STATUS_IN_ONLY.
*  UART_ep0Count - cleared.
*  UART_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 UART_InitNoDataControlTransfer(void) 
{
    UART_transferState = UART_TRANS_STATE_NO_DATA_CONTROL;
    UART_ep0Mode       = UART_MODE_STATUS_IN_ONLY;
    UART_ep0Toggle     = UART_EP0_CNT_DATA_TOGGLE;
    UART_ep0Count      = 0u;

    return (UART_TRUE);
}


/*******************************************************************************
* Function Name: UART_NoDataControlStatusStage
****************************************************************************//**
*  Handle the Status Stage of a no data control transfer.
*
*  SET_ADDRESS is special, since we need to receive the status stage with
*  the old address.
*
* \globalvars
*  UART_transferState - set to TRANS_STATE_IDLE.
*  UART_ep0Mode  - set to MODE_STALL_IN_OUT.
*  UART_ep0Toggle - set to EP0_CNT_DATA_TOGGLE
*  UART_deviceAddress - used to set new address and cleared
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_NoDataControlStatusStage(void) 
{
    if (0u != UART_deviceAddress)
    {
        /* Update device address if we got new address. */
        UART_CR0_REG = (uint8) UART_deviceAddress | UART_CR0_ENABLE;
        UART_deviceAddress = 0u;
    }

    UART_transferState = UART_TRANS_STATE_IDLE;
    
    /* Update the completion block. */
    UART_UpdateStatusBlock(UART_XFER_STATUS_ACK);
    
    /* Stall IN and OUT, no more data is expected. */
    UART_ep0Mode = UART_MODE_STALL_IN_OUT;
}


/*******************************************************************************
* Function Name: UART_UpdateStatusBlock
****************************************************************************//**
*
*  Update the Completion Status Block for a Request.  The block is updated
*  with the completion code the UART_transferByteCount.  The
*  StatusBlock Pointer is set to NULL.
*
*  completionCode - status.
*
*
* \globalvars
*  UART_currentTD.pStatusBlock->status - updated by the
*    completionCode parameter.
*  UART_currentTD.pStatusBlock->length - updated.
*  UART_currentTD.pStatusBlock - cleared.
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_UpdateStatusBlock(uint8 completionCode) 
{
    if (UART_currentTD.pStatusBlock != NULL)
    {
        UART_currentTD.pStatusBlock->status = completionCode;
        UART_currentTD.pStatusBlock->length = UART_transferByteCount;
        UART_currentTD.pStatusBlock = NULL;
    }
}


/*******************************************************************************
* Function Name: UART_InitializeStatusBlock
****************************************************************************//**
*
*  Initialize the Completion Status Block for a Request.  The completion
*  code is set to USB_XFER_IDLE.
*
*  Also, initializes UART_transferByteCount.  Save some space,
*  this is the only consumer.
*
* \globalvars
*  UART_currentTD.pStatusBlock->status - set to XFER_IDLE.
*  UART_currentTD.pStatusBlock->length - cleared.
*  UART_transferByteCount - cleared.
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_InitializeStatusBlock(void) 
{
    UART_transferByteCount = 0u;
    
    if (UART_currentTD.pStatusBlock != NULL)
    {
        UART_currentTD.pStatusBlock->status = UART_XFER_IDLE;
        UART_currentTD.pStatusBlock->length = 0u;
    }
}


/* [] END OF FILE */
