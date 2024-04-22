/***************************************************************************//**
* \file UART_cls.c
* \version 3.20
*
* \brief
*  This file contains the USB Class request handler.
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

#if(UART_EXTERN_CLS == UART_FALSE)

/***************************************
* User Implemented Class Driver Declarations.
***************************************/
/* `#START USER_DEFINED_CLASS_DECLARATIONS` Place your declaration here */

/* `#END` */


/*******************************************************************************
* Function Name: UART_DispatchClassRqst
****************************************************************************//**
*  This routine dispatches class specific requests depend on interface class.
*
* \return
*  requestHandled.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 UART_DispatchClassRqst(void) 
{
    uint8 interfaceNumber;
    uint8 requestHandled = UART_FALSE;

    /* Get interface to which request is intended. */
    switch (UART_bmRequestTypeReg & UART_RQST_RCPT_MASK)
    {
        case UART_RQST_RCPT_IFC:
            /* Class-specific request directed to interface: wIndexLoReg 
            * contains interface number.
            */
            interfaceNumber = (uint8) UART_wIndexLoReg;
            break;
        
        case UART_RQST_RCPT_EP:
            /* Class-specific request directed to endpoint: wIndexLoReg contains 
            * endpoint number. Find interface related to endpoint. 
            */
            interfaceNumber = UART_EP[UART_wIndexLoReg & UART_DIR_UNUSED].interface;
            break;
            
        default:
            /* Default interface is zero. */
            interfaceNumber = 0u;
            break;
    }
    
    /* Check that interface is within acceptable range */
    if (interfaceNumber <= UART_MAX_INTERFACES_NUMBER)
    {
    #if (defined(UART_ENABLE_HID_CLASS)   || \
         defined(UART_ENABLE_AUDIO_CLASS) || \
         defined(UART_ENABLE_CDC_CLASS)   || \
         UART_ENABLE_MSC_CLASS)

        /* Handle class request depends on interface type. */
        switch (UART_interfaceClass[interfaceNumber])
        {
        #if defined(UART_ENABLE_HID_CLASS)
            case UART_CLASS_HID:
                requestHandled = UART_DispatchHIDClassRqst();
                break;
        #endif /* (UART_ENABLE_HID_CLASS) */
                
        #if defined(UART_ENABLE_AUDIO_CLASS)
            case UART_CLASS_AUDIO:
                requestHandled = UART_DispatchAUDIOClassRqst();
                break;
        #endif /* (UART_CLASS_AUDIO) */
                
        #if defined(UART_ENABLE_CDC_CLASS)
            case UART_CLASS_CDC:
                requestHandled = UART_DispatchCDCClassRqst();
                break;
        #endif /* (UART_ENABLE_CDC_CLASS) */
            
        #if (UART_ENABLE_MSC_CLASS)
            case UART_CLASS_MSD:
            #if (UART_HANDLE_MSC_REQUESTS)
                /* MSC requests are handled by the component. */
                requestHandled = UART_DispatchMSCClassRqst();
            #elif defined(UART_DISPATCH_MSC_CLASS_RQST_CALLBACK)
                /* MSC requests are handled by user defined callbcak. */
                requestHandled = UART_DispatchMSCClassRqst_Callback();
            #else
                /* MSC requests are not handled. */
                requestHandled = UART_FALSE;
            #endif /* (UART_HANDLE_MSC_REQUESTS) */
                break;
        #endif /* (UART_ENABLE_MSC_CLASS) */
            
            default:
                /* Request is not handled: unknown class request type. */
                requestHandled = UART_FALSE;
                break;
        }
    #endif /* Class support is enabled */
    }
    
    /* `#START USER_DEFINED_CLASS_CODE` Place your Class request here */

    /* `#END` */

#ifdef UART_DISPATCH_CLASS_RQST_CALLBACK
    if (UART_FALSE == requestHandled)
    {
        requestHandled = UART_DispatchClassRqst_Callback(interfaceNumber);
    }
#endif /* (UART_DISPATCH_CLASS_RQST_CALLBACK) */

    return (requestHandled);
}


/*******************************************************************************
* Additional user functions supporting Class Specific Requests
********************************************************************************/

/* `#START CLASS_SPECIFIC_FUNCTIONS` Place any additional functions here */

/* `#END` */

#endif /* UART_EXTERN_CLS */


/* [] END OF FILE */
