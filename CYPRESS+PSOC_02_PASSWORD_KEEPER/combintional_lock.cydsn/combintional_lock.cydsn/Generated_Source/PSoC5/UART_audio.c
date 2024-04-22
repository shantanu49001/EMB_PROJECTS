/***************************************************************************//**
* \file UART_audio.c
* \version 3.20
*
* \brief
*  This file contains the USB AUDIO Class request handler.
*
* Related Document:
*  Universal Serial Bus Device Class Definition for Audio Devices Release 1.0
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART_audio.h"
#include "UART_pvt.h"
#include "cyapicallbacks.h"

#if defined(UART_ENABLE_AUDIO_CLASS)

/***************************************
* Custom Declarations
***************************************/

/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */


#if !defined(USER_SUPPLIED_AUDIO_HANDLER)

/***************************************
*    AUDIO Variables
***************************************/

#if defined(UART_ENABLE_AUDIO_STREAMING)
    /** Contains the current audio sample frequency. It is set by the host using a SET_CUR request to the endpoint.*/
    volatile uint8 UART_currentSampleFrequency[UART_MAX_EP][UART_SAMPLE_FREQ_LEN];
    /** Used as a flag for the user code, to inform it that the host has been sent a request 
     * to change the sample frequency. The sample frequency will be sent on the next OUT transaction. 
     * It contains the endpoint address when set. The following code is recommended for 
     * detecting new sample frequency in main code:
     * \snippet  /USBFS_sut_02.cydsn/main.c Detecting new Sample Frequency
     *     
     * The USBFS_transferState variable is checked to make sure that the transfer completes. */
    volatile uint8 UART_frequencyChanged;
    /** Contains the mute configuration set by the host.*/
    volatile uint8 UART_currentMute;
    /** Contains the volume level set by the host.*/
    volatile uint8 UART_currentVolume[UART_VOLUME_LEN];
    volatile uint8 UART_minimumVolume[UART_VOLUME_LEN] = {UART_VOL_MIN_LSB,
                                                                                  UART_VOL_MIN_MSB};
    volatile uint8 UART_maximumVolume[UART_VOLUME_LEN] = {UART_VOL_MAX_LSB,
                                                                                  UART_VOL_MAX_MSB};
    volatile uint8 UART_resolutionVolume[UART_VOLUME_LEN] = {UART_VOL_RES_LSB,
                                                                                     UART_VOL_RES_MSB};
#endif /*  UART_ENABLE_AUDIO_STREAMING */


/*******************************************************************************
* Function Name: UART_DispatchAUDIOClassRqst
****************************************************************************//**
*
*  This routine dispatches class requests
*
* \return
*  Results of Audio Class request handling: 
*  - UART_TRUE  - request was handled without errors
*  - UART_FALSE - error occurs during handling of request     
*
* \globalvars
*   UART_currentSampleFrequency: Contains the current audio Sample
*       Frequency. It is set by the Host using SET_CUR request to the endpoint.
*   UART_frequencyChanged: This variable is used as a flag for the
*       user code, to be aware that Host has been sent request for changing
*       Sample Frequency. Sample frequency will be sent on the next OUT
*       transaction. It is contains endpoint address when set. The following
*       code is recommended for detecting new Sample Frequency in main code:
*       
*  \snippet  /USBFS_sut_02.cydsn/main.c Detecting new Sample Frequency
*      
*   UART_transferState variable is checked to be sure that transfer
*              completes.
*   UART_currentMute: Contains mute configuration set by Host.
*   UART_currentVolume: Contains volume level set by Host.
*
* \reentrant
*  No.
*
*******************************************************************************/
uint8 UART_DispatchAUDIOClassRqst(void) 
{
    uint8 requestHandled = UART_FALSE;
    
    uint8 RqstRcpt = (uint8)(UART_bmRequestTypeReg & UART_RQST_RCPT_MASK);
#if defined(UART_ENABLE_AUDIO_STREAMING)
    uint8 wValueHi = (uint8) UART_wValueHiReg;
    uint8 epNumber = (uint8) UART_wIndexLoReg & UART_DIR_UNUSED;
#endif /* (UART_ENABLE_AUDIO_STREAMING) */
    
    /* Check request direction: D2H or H2D. */
    if (0u != (UART_bmRequestTypeReg & UART_RQST_DIR_D2H))
    {
        /* Handle direction from device to host. */
        
        if (UART_RQST_RCPT_EP == RqstRcpt)
        {
            /* Request recipient is to endpoint. */
            switch (UART_bRequestReg)
            {
                case UART_GET_CUR:
                #if defined(UART_ENABLE_AUDIO_STREAMING)
                    if (wValueHi == UART_SAMPLING_FREQ_CONTROL)
                    {
                         /* point Control Selector is Sampling Frequency */
                        UART_currentTD.wCount = UART_SAMPLE_FREQ_LEN;
                        UART_currentTD.pData  = UART_currentSampleFrequency[epNumber];
                        
                        requestHandled   = UART_InitControlRead();
                    }
                #endif /* (UART_ENABLE_AUDIO_STREAMING) */
                
                    /* `#START AUDIO_READ_REQUESTS` Place other request handler here */

                    /* `#END` */
                
                #ifdef UART_DISPATCH_AUDIO_CLASS_AUDIO_READ_REQUESTS_CALLBACK    
                    UART_DispatchAUDIOClass_AUDIO_READ_REQUESTS_Callback();
                #endif /* (UART_DISPATCH_AUDIO_CLASS_AUDIO_READ_REQUESTS_CALLBACK) */                   
                break;
                
                default:
                    /* Do not handle this request unless callback is defined. */
                    break;
            }
        
        }
        else if (UART_RQST_RCPT_IFC == RqstRcpt)
        {
            /* Request recipient is interface or entity ID. */
            switch (UART_bRequestReg)
            {
                case UART_GET_CUR:
                #if defined(UART_ENABLE_AUDIO_STREAMING)
                    if (wValueHi == UART_MUTE_CONTROL)
                    {
                        /* `#START MUTE_CONTROL_GET_REQUEST` Place multi-channel handler here */

                        /* `#END` */

                    #ifdef UART_DISPATCH_AUDIO_CLASS_MUTE_CONTROL_GET_REQUEST_CALLBACK
                        UART_DispatchAUDIOClass_MUTE_CONTROL_GET_REQUEST_Callback();
                    #endif /* (UART_DISPATCH_AUDIO_CLASS_MUTE_CONTROL_GET_REQUEST_CALLBACK) */

                        /* Entity ID Control Selector is MUTE */
                        UART_currentTD.wCount = 1u;
                        UART_currentTD.pData  = &UART_currentMute;
                        
                        requestHandled = UART_InitControlRead();
                    }
                    else if (wValueHi == UART_VOLUME_CONTROL)
                    {
                        /* `#START VOLUME_CONTROL_GET_REQUEST` Place multi-channel handler here */

                        /* `#END` */

                    #ifdef UART_DISPATCH_AUDIO_CLASS_VOLUME_CONTROL_GET_REQUEST_CALLBACK
                        UART_DispatchAUDIOClass_VOLUME_CONTROL_GET_REQUEST_Callback();
                    #endif /* (UART_DISPATCH_AUDIO_CLASS_VOLUME_CONTROL_GET_REQUEST_CALLBACK) */

                        /* Entity ID Control Selector is VOLUME, */
                        UART_currentTD.wCount = UART_VOLUME_LEN;
                        UART_currentTD.pData  = UART_currentVolume;
                        
                        requestHandled = UART_InitControlRead();
                    }
                    else
                    {
                        /* `#START OTHER_GET_CUR_REQUESTS` Place other request handler here */

                        /* `#END` */

                    #ifdef UART_DISPATCH_AUDIO_CLASS_OTHER_GET_CUR_REQUESTS_CALLBACK
                        UART_DispatchAUDIOClass_OTHER_GET_CUR_REQUESTS_Callback();
                    #endif /* (UART_DISPATCH_AUDIO_CLASS_OTHER_GET_CUR_REQUESTS_CALLBACK) */
                    }
                    break;
                    
                case UART_GET_MIN:
                    if (wValueHi == UART_VOLUME_CONTROL)
                    {
                        /* Entity ID Control Selector is VOLUME, */
                        UART_currentTD.wCount = UART_VOLUME_LEN;
                        UART_currentTD.pData  = &UART_minimumVolume[0];
                        
                        requestHandled = UART_InitControlRead();
                    }
                    break;
                    
                case UART_GET_MAX:
                    if (wValueHi == UART_VOLUME_CONTROL)
                    {
                        /* Entity ID Control Selector is VOLUME, */
                        UART_currentTD.wCount = UART_VOLUME_LEN;
                        UART_currentTD.pData  = &UART_maximumVolume[0];
                        
                        requestHandled = UART_InitControlRead();
                    }
                    break;
                    
                case UART_GET_RES:
                    if (wValueHi == UART_VOLUME_CONTROL)
                    {
                         /* Entity ID Control Selector is VOLUME, */
                        UART_currentTD.wCount = UART_VOLUME_LEN;
                        UART_currentTD.pData  = &UART_resolutionVolume[0];
                        
                        requestHandled   = UART_InitControlRead();
                    }
                    break;
                    
                /* The contents of the status message is reserved for future use.
                * For the time being, a null packet should be returned in the data stage of the
                * control transfer, and the received null packet should be ACKed.
                */
                case UART_GET_STAT:
                    UART_currentTD.wCount = 0u;    
                    
                    requestHandled = UART_InitControlWrite();

                #endif /* (UART_ENABLE_AUDIO_STREAMING) */
                
                    /* `#START AUDIO_WRITE_REQUESTS` Place other request handler here */

                    /* `#END` */
                
                #ifdef UART_DISPATCH_AUDIO_CLASS_AUDIO_WRITE_REQUESTS_CALLBACK
                    UART_DispatchAUDIOClass_AUDIO_WRITE_REQUESTS_Callback();
                #endif /* (UART_DISPATCH_AUDIO_CLASS_AUDIO_WRITE_REQUESTS_CALLBACK) */
                    break;
                
                default:
                    /* Do not handle this request. */
                    break;
            }
        }
        else
        {   
            /* Do not handle other requests recipients. */
        }
    }
    else
    {
        /* Handle direction from host to device. */
        
        if (UART_RQST_RCPT_EP == RqstRcpt)
        {
            /* Request recipient is endpoint. */
            switch (UART_bRequestReg)
            {
                case UART_SET_CUR:
                #if defined(UART_ENABLE_AUDIO_STREAMING)
                    if (wValueHi == UART_SAMPLING_FREQ_CONTROL)
                    {
                         /* point Control Selector is Sampling Frequency */
                        UART_currentTD.wCount = UART_SAMPLE_FREQ_LEN;
                        UART_currentTD.pData  = UART_currentSampleFrequency[epNumber];
                        UART_frequencyChanged = (uint8) epNumber;
                        
                        requestHandled   = UART_InitControlWrite();
                    }
                #endif /* (UART_ENABLE_AUDIO_STREAMING) */

                    /* `#START AUDIO_SAMPLING_FREQ_REQUESTS` Place other request handler here */

                    /* `#END` */

                #ifdef UART_DISPATCH_AUDIO_CLASS_AUDIO_SAMPLING_FREQ_REQUESTS_CALLBACK
                    UART_DispatchAUDIOClass_AUDIO_SAMPLING_FREQ_REQUESTS_Callback();
                #endif /* (UART_DISPATCH_AUDIO_CLASS_AUDIO_SAMPLING_FREQ_REQUESTS_CALLBACK) */
                    break;
                
                default:
                    /* Do not handle this request. */
                    break;
            }
        }
        else if(UART_RQST_RCPT_IFC == RqstRcpt)
        {
            /* Request recipient is interface or entity ID. */
            switch (UART_bRequestReg)
            {
                case UART_SET_CUR:
                #if defined(UART_ENABLE_AUDIO_STREAMING)
                    if (wValueHi == UART_MUTE_CONTROL)
                    {
                        /* `#START MUTE_SET_REQUEST` Place multi-channel handler here */

                        /* `#END` */

                    #ifdef UART_DISPATCH_AUDIO_CLASS_MUTE_SET_REQUEST_CALLBACK
                        UART_DispatchAUDIOClass_MUTE_SET_REQUEST_Callback();
                    #endif /* (UART_DISPATCH_AUDIO_CLASS_MUTE_SET_REQUEST_CALLBACK) */

                        /* Entity ID Control Selector is MUTE */
                        UART_currentTD.wCount = 1u;
                        UART_currentTD.pData  = &UART_currentMute;
                        
                        requestHandled = UART_InitControlWrite();
                    }
                    else if (wValueHi == UART_VOLUME_CONTROL)
                    {
                        /* `#START VOLUME_CONTROL_SET_REQUEST` Place multi-channel handler here */

                        /* `#END` */

                    #ifdef UART_DISPATCH_AUDIO_CLASS_VOLUME_CONTROL_SET_REQUEST_CALLBACK
                        UART_DispatchAUDIOClass_VOLUME_CONTROL_SET_REQUEST_Callback();
                    #endif /* (UART_DISPATCH_AUDIO_CLASS_VOLUME_CONTROL_SET_REQUEST_CALLBACK) */

                        /* Entity ID Control Selector is VOLUME */
                        UART_currentTD.wCount = UART_VOLUME_LEN;
                        UART_currentTD.pData  = UART_currentVolume;
                        
                        requestHandled = UART_InitControlWrite();
                    }
                    else
                    {
                        /* `#START OTHER_SET_CUR_REQUESTS` Place other request handler here */

                        /* `#END` */

                    #ifdef UART_DISPATCH_AUDIO_CLASS_OTHER_SET_CUR_REQUESTS_CALLBACK
                        UART_DispatchAUDIOClass_OTHER_SET_CUR_REQUESTS_Callback();
                    #endif /* (UART_DISPATCH_AUDIO_CLASS_OTHER_SET_CUR_REQUESTS_CALLBACK) */
                    }
                #endif /*  UART_ENABLE_AUDIO_STREAMING */
                
                
                    /* `#START AUDIO_CONTROL_SEL_REQUESTS` Place other request handler here */

                    /* `#END` */
                    
                #ifdef UART_DISPATCH_AUDIO_CLASS_AUDIO_CONTROL_SEL_REQUESTS_CALLBACK
                    UART_DispatchAUDIOClass_AUDIO_CONTROL_SEL_REQUESTS_Callback();
                #endif /* (UART_DISPATCH_AUDIO_CLASS_AUDIO_CONTROL_SEL_REQUESTS_CALLBACK) */
                break;

                default:
                    /* Do not handle this request. */
                break;
            }
        }
        else
        {
            /* Do not handle other requests recipients. */
        }
    }

    return (requestHandled);
}
#endif /* (USER_SUPPLIED_AUDIO_HANDLER) */


/*******************************************************************************
* Additional user functions supporting AUDIO Requests
********************************************************************************/

/* `#START AUDIO_FUNCTIONS` Place any additional functions here */

/* `#END` */

#endif  /* (UART_ENABLE_AUDIO_CLASS) */


/* [] END OF FILE */
