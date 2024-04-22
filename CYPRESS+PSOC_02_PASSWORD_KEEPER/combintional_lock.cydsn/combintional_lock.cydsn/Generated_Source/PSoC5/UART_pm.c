/***************************************************************************//**
* \file UART_pm.c
* \version 3.20
*
* \brief
*  This file provides Suspend/Resume APIs implementation.
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART_pvt.h"
#include "UART_Dp.h"
#include "cyapicallbacks.h"

/***************************************
* Custom Declarations
***************************************/
/* `#START PM_CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */


/***************************************
* Local data allocation
***************************************/

static UART_BACKUP_STRUCT  UART_backup;

#if (UART_DP_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: UART_DP_ISR
    ****************************************************************************//**
    *
    *  This Interrupt Service Routine handles DP pin changes for wake-up from
    *  the sleep mode.
    *
    *******************************************************************************/
    CY_ISR(UART_DP_ISR)
    {
    #ifdef UART_DP_ISR_ENTRY_CALLBACK
        UART_DP_ISR_EntryCallback();
    #endif /* (UART_DP_ISR_ENTRY_CALLBACK) */

        /* `#START DP_USER_CODE` Place your code here */

        /* `#END` */

        (void) UART_Dp_ClearInterrupt();
		
	#ifdef UART_DP_ISR_EXIT_CALLBACK
        UART_DP_ISR_ExitCallback();
    #endif /* (UART_DP_ISR_EXIT_CALLBACK) */
    }
#endif /* (UART_DP_ISR_ACTIVE) */


/*******************************************************************************
* Function Name: UART_SaveConfig
****************************************************************************//**
*
*  Saves the current user configuration.
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_SaveConfig(void) 
{
    /* Empty function added for the compatibility purpose. */
}


/*******************************************************************************
* Function Name: UART_RestoreConfig
****************************************************************************//**
*
*  Restores the current user configuration.
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_RestoreConfig(void) 
{
    if (UART_configuration != 0u)
    {
        UART_ConfigReg();
        UART_EpStateInit();
    }
}


/*******************************************************************************
* Function Name: UART_Suspend
****************************************************************************//**
*
*  This function prepares the USBFS component to enter low power mode. The 
*  interrupt on falling edge on Dp pin is configured to wakeup device when the 
*  host drives resume condition. The pull-up is enabled on the Dp line while 
*  device is in low power mode. The supported low power modes are Deep Sleep 
*  (PSoC 4200L) and Sleep (PSoC 3/ PSoC 5LP).
*  
*  *Note* For PSoC 4200L devices, this function should not be called before 
*  entering Sleep.
*  
*  *Note* After enter low power mode, the data which is left in the IN or OUT 
*  endpoint buffers is not restored after wakeup and lost. Therefore it should 
*  be stored in the SRAM for OUT endpoint or read by the host for IN endpoint 
*  before enter low power mode.
*
* \globalvars
*  UART_backup.enable:  modified.
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_Suspend(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();

    if (0u != (UART_CR0_REG & UART_CR0_ENABLE))
    {
        /* USB block is enabled. */
        UART_backup.enableState = 1u;

    #if (UART_EP_MANAGEMENT_DMA)
        UART_Stop_DMA(UART_MAX_EP);
    #endif /* (UART_EP_MANAGEMENT_DMA) */

    #if (CY_PSOC4)
        /* Suspend enter sequence. */
        UART_POWER_CTRL_REG |= (UART_POWER_CTRL_SUSPEND |
                                            UART_POWER_CTRL_SUSPEND_DEL);

        /* Store state of USB regulator and disable it. */
        UART_backup.mode = (uint8)  (UART_CR1_REG & UART_CR1_REG_ENABLE);
        UART_CR1_REG    &= (uint32) ~UART_CR1_REG_ENABLE;
        
        /* Store SIE interrupt sources. Valid bits are 0 - 4. */
        UART_backup.intrSeiMask = (uint8) UART_INTR_SIE_MASK_REG;

    #else
        /* Ensure USB transmit enable is low (USB_USBIO_CR0.ten). - Manual Transmission - Disabled. */
        UART_USBIO_CR0_REG &= (uint8) ~UART_USBIO_CR0_TEN;
        CyDelayUs(UART_WAIT_REG_STABILITY_50NS);  /*~50ns delay. */

        /* Disable the USBIO by asserting PM.USB_CR0.fsusbio_pd_n(Inverted) and pd_pullup_hv(Inverted) high. */
        UART_PM_USB_CR0_REG &= (uint8) ~(UART_PM_USB_CR0_PD_N |
                                                     UART_PM_USB_CR0_PD_PULLUP_N);

        /* Disable the SIE. */
        UART_CR0_REG &= (uint8) ~UART_CR0_ENABLE;

        CyDelayUs(UART_WAIT_REG_STABILITY_50NS);  /* ~50ns delay. */
        /* Store mode and disable VRegulator. */
        UART_backup.mode = (uint8) (UART_CR1_REG & UART_CR1_REG_ENABLE);
        UART_CR1_REG    &= (uint8) ~UART_CR1_REG_ENABLE;

        CyDelayUs(UART_WAIT_REG_STABILITY_1US); /* min 0.5us delay required. */

        /* Disable the USBIO reference by setting PM.USB_CR0.fsusbio_ref_en.*/
        UART_PM_USB_CR0_REG &= (uint8) ~UART_PM_USB_CR0_REF_EN;

        /* Switch DP and DM terminals to GPIO mode and disconnect 1.5k pull-up. */
        UART_USBIO_CR1_REG |= UART_USBIO_CR1_IOMODE;

        /* Disable USBFS block. */
        /* Clear power active and standby mode templates: disable USB block. */
        UART_PM_ACT_CFG_REG  &= (uint8) ~UART_PM_ACT_EN_FSUSB;
        UART_PM_STBY_CFG_REG &= (uint8) ~UART_PM_STBY_EN_FSUSB;

        CyDelayUs(UART_WAIT_REG_STABILITY_1US); /* min 0.5us delay required. */
    #endif /* (CY_PSOC4) */
    }
    else
    {
        UART_backup.enableState = 0u;
    }

    CyExitCriticalSection(enableInterrupts);

#if (UART_DP_ISR_ACTIVE)
    /* Clear active mode Dp interrupt source history. */
    (void) UART_Dp_ClearInterrupt();
    CyIntClearPending(UART_DP_INTC_VECT_NUM);

    CyIntEnable      (UART_DP_INTC_VECT_NUM);
#endif /* (UART_DP_ISR_ACTIVE). */
}


/*******************************************************************************
* Function Name: UART_Resume
****************************************************************************//**
*
*  This function prepares the USBFS component for active mode operation after 
*  exit low power mode. It restores the component active mode configuration such 
*  as device address assigned previously by the host, endpoints buffer and disables 
*  interrupt on Dp pin.
*  The supported low power modes are Deep Sleep (PSoC 4200L) and Sleep 
*  (PSoC 3/ PSoC 5LP).
*  
*  *Note* For PSoC 4200L devices, this function should not be called after 
*  exiting Sleep.
*  
*  *Note* To resume communication with the host, the data endpoints must be 
*  managed: the OUT endpoints must be enabled and IN endpoints must be loaded 
*  with data. For DMA with Automatic Buffer Management, all endpoints buffers 
*  must be initialized again before making them available to the host.
*
*
* \globalvars
*  UART_backup - checked.
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_Resume(void) 
{
    uint8 enableInterrupts;

    enableInterrupts = CyEnterCriticalSection();

    if (0u != UART_backup.enableState)
    {
    #if (UART_DP_ISR_ACTIVE)
        CyIntDisable(UART_DP_INTC_VECT_NUM);
    #endif /* (UART_DP_ISR_ACTIVE) */

    #if (CY_PSOC4)
        /* Enable clock to USB IP. */
        UART_USB_CLK_EN_REG |= UART_USB_CLK_CSR_CLK_EN;

        /* Restore arbiter configuration for DMA transfers. */
        #if (UART_EP_MANAGEMENT_DMA)
            #if (UART_ARB_ISR_ACTIVE)
                /* Enable ARB EP interrupt sources. */
                UART_ARB_INT_EN_REG = UART_DEFAULT_ARB_INT_EN;
            #endif /* (UART_EP_MANAGEMENT_DMA) */

            /* Configure arbiter for Manual or Auto DMA operation and clear 
            * configuration completion. 
            */
            UART_ARB_CFG_REG = UART_DEFAULT_ARB_CFG;
        #endif /* (UART_EP_MANAGEMENT_DMA) */

        /* Restore level (hi, lo, med) for each interrupt source. */
        UART_INTR_LVL_SEL_REG = UART_DEFAULT_INTR_LVL_SEL;
        
        /* Store SIE interrupt sources. */
        UART_INTR_SIE_MASK_REG = (uint32) UART_backup.intrSeiMask;

        /* Set EP0.CR: ACK Setup, NAK IN/OUT. */
        UART_EP0_CR_REG = UART_MODE_NAK_IN_OUT;

        /* Restore data EP1-8 configuration. */
        UART_RestoreConfig();

        /* Restore state of USB regulator and wait until it supples stable power. */
        UART_CR1_REG |= UART_backup.mode;
        CyDelayUs(UART_WAIT_VREF_STABILITY);

        /* Suspend exit sequence. */
        UART_POWER_CTRL_REG &= (uint32) ~UART_POWER_CTRL_SUSPEND;
        CyDelayUs(UART_WAIT_SUSPEND_DEL_DISABLE);
        UART_POWER_CTRL_REG &= (uint32) ~UART_POWER_CTRL_SUSPEND_DEL;

    #else
        /* Set power active and standby mode templates: enable USB block. */
        UART_PM_ACT_CFG_REG  |= UART_PM_ACT_EN_FSUSB;
        UART_PM_STBY_CFG_REG |= UART_PM_STBY_EN_FSUSB;

        /* Enable core clock. */
        UART_USB_CLK_EN_REG |= UART_USB_CLK_ENABLE;

        /* Enable the USBIO reference by setting PM.USB_CR0.fsusbio_ref_en.*/
        UART_PM_USB_CR0_REG |= UART_PM_USB_CR0_REF_EN;

        /* The reference is available ~40us after power restored. */
        CyDelayUs(UART_WAIT_VREF_RESTORE);
        /* Restore state of USB regulator and wait until it supples stable power. */
        UART_CR1_REG |= UART_backup.mode;
        CyDelayUs(UART_WAIT_VREF_STABILITY);   /*~50ns delay. */

        /* Enable USBIO. */
        UART_PM_USB_CR0_REG |= UART_PM_USB_CR0_PD_N;
        CyDelayUs(UART_WAIT_PD_PULLUP_N_ENABLE);
        /* Set the USBIO pull-up enable. */
        UART_PM_USB_CR0_REG |= UART_PM_USB_CR0_PD_PULLUP_N;

        /* Restore arbiter configuration for DMA transfers. */
        #if (UART_EP_MANAGEMENT_DMA)
            #if (UART_ARB_ISR_ACTIVE)
                /* Enable ARB EP interrupt sources. */
                UART_ARB_INT_EN_REG = UART_DEFAULT_ARB_INT_EN;
            #endif /* (UART_EP_MANAGEMENT_DMA) */

            /* Configure arbiter for Manual or Auto DMA operation and clear 
            * configuration completion. 
            */
            UART_ARB_CFG_REG = UART_DEFAULT_ARB_CFG;
        #endif /* (UART_EP_MANAGEMENT_DMA) */

        /* Set EP0.CR: ACK Setup, STALL IN/OUT. */
        UART_EP0_CR_REG = UART_MODE_STALL_IN_OUT;

        /* Enable the USB IP to respond to USB traffic with the last address. */
        UART_CR0_REG |= UART_CR0_ENABLE;
        CyDelayCycles(UART_WAIT_CR0_REG_STABILITY);

        /* Enable D+ pull-up and keep USB control on IO. */
        UART_USBIO_CR1_REG = UART_USBIO_CR1_USBPUEN;

        /* Restore data EP1-8 configuration. */
        UART_RestoreConfig();
    #endif /* (CY_PSOC4) */
    }

    CyExitCriticalSection(enableInterrupts);
}


/* [] END OF FILE */
