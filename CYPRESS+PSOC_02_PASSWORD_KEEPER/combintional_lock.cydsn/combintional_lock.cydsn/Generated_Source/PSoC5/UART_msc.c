/***************************************************************************//**
* \file UART_cdc.c
* \version 3.20
*
* \brief
*  This file contains the USB MSC Class request handler and global API for MSC 
*  class.
*
* Related Document:
*  Universal Serial Bus Class Definitions for Communication Devices Version 1.1
*
********************************************************************************
* \copyright
* Copyright 2012-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART_msc.h"
#include "UART_pvt.h"
#include "cyapicallbacks.h"

#if (UART_HANDLE_MSC_REQUESTS)

/***************************************
*          Internal variables
***************************************/

static uint8 UART_lunCount = UART_MSC_LUN_NUMBER;


/*******************************************************************************
* Function Name: UART_DispatchMSCClassRqst
****************************************************************************//**
*   
*  \internal 
*  This routine dispatches MSC class requests.
*
* \return
*  Status of request processing: handled or not handled.
*
* \globalvars
*  UART_lunCount - stores number of LUN (logical units).
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 UART_DispatchMSCClassRqst(void) 
{
    uint8 requestHandled = UART_FALSE;
    
    /* Get request data. */
    uint16 value  = UART_GET_UINT16(UART_wValueHiReg,  UART_wValueLoReg);
    uint16 dataLength = UART_GET_UINT16(UART_wLengthHiReg, UART_wLengthLoReg);
       
    /* Check request direction: D2H or H2D. */
    if (0u != (UART_bmRequestTypeReg & UART_RQST_DIR_D2H))
    {
        /* Handle direction from device to host. */
        
        if (UART_MSC_GET_MAX_LUN == UART_bRequestReg)
        {
            /* Check request fields. */
            if ((value  == UART_MSC_GET_MAX_LUN_WVALUE) &&
                (dataLength == UART_MSC_GET_MAX_LUN_WLENGTH))
            {
                /* Reply to Get Max LUN request: setup control read. */
                UART_currentTD.pData = &UART_lunCount;
                UART_currentTD.count =  UART_MSC_GET_MAX_LUN_WLENGTH;
                
                requestHandled  = UART_InitControlRead();
            }
        }
    }
    else
    {
        /* Handle direction from host to device. */
        
        if (UART_MSC_RESET == UART_bRequestReg)
        {
            /* Check request fields. */
            if ((value  == UART_MSC_RESET_WVALUE) &&
                (dataLength == UART_MSC_RESET_WLENGTH))
            {
                /* Handle to Bulk-Only Reset request: no data control transfer. */
                UART_currentTD.count = UART_MSC_RESET_WLENGTH;
                
            #ifdef UART_DISPATCH_MSC_CLASS_MSC_RESET_RQST_CALLBACK
                UART_DispatchMSCClass_MSC_RESET_RQST_Callback();
            #endif /* (UART_DISPATCH_MSC_CLASS_MSC_RESET_RQST_CALLBACK) */
                
                requestHandled = UART_InitNoDataControlTransfer();
            }
        }
    }
    
    return (requestHandled);
}


/*******************************************************************************
* Function Name: UART_MSC_SetLunCount
****************************************************************************//**
*
*  This function sets the number of logical units supported in the application. 
*  The default number of logical units is set in the component customizer.
*
*  \param lunCount: Count of the logical units. Valid range is between 1 and 16.
*
*
* \globalvars
*  UART_lunCount - stores number of LUN (logical units).
*
* \reentrant
*  No.
*
*******************************************************************************/
void UART_MSC_SetLunCount(uint8 lunCount) 
{
    UART_lunCount = (lunCount - 1u);
}


/*******************************************************************************
* Function Name: UART_MSC_GetLunCount
****************************************************************************//**
*
*  This function returns the number of logical units.
*
* \return
*   Number of the logical units.
*
* \globalvars
*  UART_lunCount - stores number of LUN (logical units).
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 UART_MSC_GetLunCount(void) 
{
    return (UART_lunCount + 1u);
}   

#endif /* (UART_HANDLE_MSC_REQUESTS) */


/* [] END OF FILE */
