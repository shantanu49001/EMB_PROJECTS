/***************************************************************************//**
* \file UART_midi.c
* \version 3.20
*
* \brief
*  MIDI Streaming request handler.
*  This file contains routines for sending and receiving MIDI
*  messages, and handles running status in both directions.
*
* Related Document:
*  Universal Serial Bus Device Class Definition for MIDI Devices Release 1.0
*  MIDI 1.0 Detailed Specification Document Version 4.2
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART_midi.h"
#include "UART_pvt.h"
#include "cyapicallbacks.h"

#if defined(UART_ENABLE_MIDI_STREAMING)

/***************************************
*    MIDI Constants
***************************************/

#if (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF)
    /* The Size of the MIDI messages (MIDI Table 4-1) */
    static const uint8 CYCODE UART_MIDI_SIZE[] = {
    /*  Miscellaneous function codes(Reserved)  */ 0x03u,
    /*  Cable events (Reserved)                 */ 0x03u,
    /*  Two-byte System Common messages         */ 0x02u,
    /*  Three-byte System Common messages       */ 0x03u,
    /*  SysEx starts or continues               */ 0x03u,
    /*  Single-byte System Common Message or
        SysEx ends with following single byte   */ 0x01u,
    /*  SysEx ends with following two bytes     */ 0x02u,
    /*  SysEx ends with following three bytes   */ 0x03u,
    /*  Note-off                                */ 0x03u,
    /*  Note-on                                 */ 0x03u,
    /*  Poly-KeyPress                           */ 0x03u,
    /*  Control Change                          */ 0x03u,
    /*  Program Change                          */ 0x02u,
    /*  Channel Pressure                        */ 0x02u,
    /*  PitchBend Change                        */ 0x03u,
    /*  Single Byte                             */ 0x01u
    };
#endif  /* UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF */



/***************************************
*  Global variables
***************************************/


#if (UART_MIDI_IN_BUFF_SIZE > 0)
    #if (UART_MIDI_IN_BUFF_SIZE >= 256)
        /** Input endpoint buffer pointer. This pointer is used as an index for the
        * USBMIDI_midiInBuffer to write data. It is cleared to zero by the
        * USBMIDI_MIDI_EP_Init() function.*/
        volatile uint16 UART_midiInPointer;                            /* Input endpoint buffer pointer */
    #else
        volatile uint8 UART_midiInPointer;                             /* Input endpoint buffer pointer */
    #endif /* (UART_MIDI_IN_BUFF_SIZE >= 256) */
    /** Contains the midi IN endpoint number, It is initialized after a
     * SET_CONFIGURATION request based on a user descriptor. It is used in MIDI
     * APIs to send data to the host.*/
    volatile uint8 UART_midi_in_ep;
    /** Input endpoint buffer with a length equal to MIDI IN EP Max Packet Size.
     * This buffer is used to save and combine the data received from the UARTs,
     * generated internally by USBMIDI_PutUsbMidiIn() function messages, or both.
     * The USBMIDI_MIDI_IN_Service() function transfers the data from this buffer to the host.*/
    uint8 UART_midiInBuffer[UART_MIDI_IN_BUFF_SIZE];       /* Input endpoint buffer */
#endif /* (UART_MIDI_IN_BUFF_SIZE > 0) */

#if (UART_MIDI_OUT_BUFF_SIZE > 0)
    /** Contains the midi OUT endpoint number. It is initialized after a
     * SET_CONFIGURATION request based on a user descriptor. It is used in
     * MIDI APIs to receive data from the host.*/
    volatile uint8 UART_midi_out_ep;                                   /* Output endpoint number */
    /** Output endpoint buffer with a length equal to MIDI OUT EP Max Packet Size.
     * This buffer is used by the USBMIDI_MIDI_OUT_EP_Service() function to save
     * the data received from the host. The received data is then parsed. The
     * parsed data is transferred to the UARTs buffer and also used for internal
     * processing by the USBMIDI_callbackLocalMidiEvent() function.*/
    uint8 UART_midiOutBuffer[UART_MIDI_OUT_BUFF_SIZE];     /* Output endpoint buffer */
#endif /* (UART_MIDI_OUT_BUFF_SIZE > 0) */

#if (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF)

    static UART_MIDI_RX_STATUS UART_MIDI1_Event;            /* MIDI RX status structure */
    static volatile uint8 UART_MIDI1_TxRunStat;                         /* MIDI Output running status */
    /** The USBFS supports a maximum of two external Jacks. The two flag variables
     * are used to represent the status of two external Jacks. These optional variables
     * are allocated when External Mode is enabled. The following flags help to
     * detect and generate responses for SysEx messages. The USBMIDI_MIDI2_InqFlags
     * is optional and is not available when only one external Jack is configured.
     * Flag                          | Description
     * ------------------------------|---------------------------------------
     * USBMIDI_INQ_SYSEX_FLAG        | Non-real-time SysEx message received.
     * USBMIDI_INQ_IDENTITY_REQ_FLAG | Identity Request received. You should clear this bit when an Identity Reply message is generated.
     * SysEX messages are intended for local device and shouldn't go out on the
     * external MIDI jack, this flag indicates when a MIDI SysEx OUT message is
     * in progress for the application */
    volatile uint8 UART_MIDI1_InqFlags;                                 /* Device inquiry flag */

    #if (UART_MIDI_EXT_MODE >= UART_TWO_EXT_INTRF)
        static UART_MIDI_RX_STATUS UART_MIDI2_Event;        /* MIDI RX status structure */
        static volatile uint8 UART_MIDI2_TxRunStat;                     /* MIDI Output running status */
        /** See description of \ref UART_MIDI1_InqFlags*/
        volatile uint8 UART_MIDI2_InqFlags;                             /* Device inquiry flag */
    #endif /* (UART_MIDI_EXT_MODE >= UART_TWO_EXT_INTRF) */
#endif /* (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF) */


/***************************************
* Custom Declarations
***************************************/

/* `#START MIDI_CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */


#if (UART_ENABLE_MIDI_API != 0u)
/*******************************************************************************
* Function Name: UART_MIDI_Init
****************************************************************************//**
*
*  This function initializes the MIDI interface and UART(s) to be ready to
*   receive data from the PC and MIDI ports.
*
* \globalvars
*
*  \ref UART_midiInBuffer: This buffer is used for saving and combining
*    the received data from UART(s) and(or) generated internally by
*    PutUsbMidiIn() function messages. UART_MIDI_IN_EP_Service()
*    function transfers the data from this buffer to the PC.
*
*  \ref UART_midiOutBuffer: This buffer is used by the
*    UART_MIDI_OUT_Service() function for saving the received
*    from the PC data, then the data are parsed and transferred to UART(s)
*    buffer and to the internal processing by the
*
*  \ref UART_callbackLocalMidiEvent function.
*
*  \ref UART_midi_out_ep: Used as an OUT endpoint number.
*
*  \ref UART_midi_in_ep: Used as an IN endpoint number.
*
*   \ref UART_midiInPointer: Initialized to zero.
*
* \sideeffect
*   The priority of the UART RX ISR should be higher than UART TX ISR. To do
*   that this function changes the priority of the UARTs TX and RX interrupts.
*
* \reentrant
*  No
*
*******************************************************************************/
void UART_MIDI_Init(void) 
{
#if (UART_MIDI_IN_BUFF_SIZE > 0)
   UART_midiInPointer = 0u;
#endif /* (UART_MIDI_IN_BUFF_SIZE > 0) */

#if (UART_EP_MANAGEMENT_DMA_AUTO)
    #if (UART_MIDI_IN_BUFF_SIZE > 0)
        /* Provide buffer for IN endpoint. */
        UART_LoadInEP(UART_midi_in_ep, UART_midiInBuffer,
                                                               UART_MIDI_IN_BUFF_SIZE);
    #endif  /* (UART_MIDI_IN_BUFF_SIZE > 0) */

    #if (UART_MIDI_OUT_BUFF_SIZE > 0)
        /* Provide buffer for OUT endpoint. */
        (void)UART_ReadOutEP(UART_midi_out_ep, UART_midiOutBuffer,
                                                                       UART_MIDI_OUT_BUFF_SIZE);
    #endif /* (UART_MIDI_OUT_BUFF_SIZE > 0) */
#endif /* (UART_EP_MANAGEMENT_DMA_AUTO) */

#if (UART_MIDI_OUT_BUFF_SIZE > 0)
    UART_EnableOutEP(UART_midi_out_ep);
#endif /* (UART_MIDI_OUT_BUFF_SIZE > 0) */

    /* Initialize the MIDI port(s) */
#if (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF)
    UART_MIDI_InitInterface();
#endif /* (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF) */
}


#if (UART_MIDI_OUT_BUFF_SIZE > 0)
    /*******************************************************************************
    * Function Name: UART_MIDI_OUT_Service
    ****************************************************************************//**
    *
    *  This function services the traffic from the USBMIDI OUT endpoint and
    *  sends the data to the MIDI output ports (TX UARTs). It is blocked by the
    *  UART when not enough space is available in the UART TX buffer.
    *  This function is automatically called from OUT EP ISR in DMA with
    *  Automatic Memory Management mode. In Manual and DMA with Manual EP
    *  Management modes you must call it from the main foreground task.
    *
    * \globalvars
    *
    *  \ref UART_midiOutBuffer: Used as temporary buffer between USB
    *       internal memory and UART TX buffer.
    *
    *  \ref UART_midi_out_ep: Used as an OUT endpoint number.
    *
    * \reentrant
    *  No
    *
    *******************************************************************************/
    void UART_MIDI_OUT_Service(void) 
    {
    #if (UART_MIDI_OUT_BUFF_SIZE >= 256)
        uint16 outLength;
        uint16 outPointer;
    #else
        uint8 outLength;
        uint8 outPointer;
    #endif /* (UART_MIDI_OUT_BUFF_SIZE >= 256) */

        /* Service the USB MIDI output endpoint. */
        if (UART_OUT_BUFFER_FULL == UART_GetEPState(UART_midi_out_ep))
        {
        #if (UART_MIDI_OUT_BUFF_SIZE >= 256)
            outLength = UART_GetEPCount(UART_midi_out_ep);
        #else
            outLength = (uint8)UART_GetEPCount(UART_midi_out_ep);
        #endif /* (UART_MIDI_OUT_BUFF_SIZE >= 256) */

        #if (!UART_EP_MANAGEMENT_DMA_AUTO)
            #if (UART_MIDI_OUT_BUFF_SIZE >= 256)
                outLength = UART_ReadOutEP(UART_midi_out_ep,
                                                       UART_midiOutBuffer, outLength);
            #else
                outLength = (uint8)UART_ReadOutEP(UART_midi_out_ep,
                                                              UART_midiOutBuffer, (uint16) outLength);
            #endif /* (UART_MIDI_OUT_BUFF_SIZE >= 256) */

            #if (UART_EP_MANAGEMENT_DMA_MANUAL)
                /* Wait until DMA complete transferring data from OUT endpoint buffer. */
                while (UART_OUT_BUFFER_FULL == UART_GetEPState(UART_midi_out_ep))
                {
                }

                /* Enable OUT endpoint for communication with host. */
                UART_EnableOutEP(UART_midi_out_ep);
            #endif /* (UART_EP_MANAGEMENT_DMA_MANUAL) */
        #endif /* (!UART_EP_MANAGEMENT_DMA_AUTO) */

            if (outLength >= UART_EVENT_LENGTH)
            {
                outPointer = 0u;
                while (outPointer < outLength)
                {
                    /* In some OS OUT packet could be appended by nulls which could be skipped. */
                    if (UART_midiOutBuffer[outPointer] == 0u)
                    {
                        break;
                    }

                /* Route USB MIDI to the External connection */
                #if (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF)
                    if ((UART_midiOutBuffer[outPointer] & UART_CABLE_MASK) ==
                         UART_MIDI_CABLE_00)
                    {
                        UART_MIDI1_ProcessUsbOut(&UART_midiOutBuffer[outPointer]);
                    }
                    else if ((UART_midiOutBuffer[outPointer] & UART_CABLE_MASK) ==
                             UART_MIDI_CABLE_01)
                    {
                    #if (UART_MIDI_EXT_MODE >= UART_TWO_EXT_INTRF)
                         UART_MIDI2_ProcessUsbOut(&UART_midiOutBuffer[outPointer]);
                    #endif /*  UART_MIDI_EXT_MODE >= UART_TWO_EXT_INTRF */
                    }
                    else
                    {
                        /* `#START CUSTOM_MIDI_OUT_EP_SERV` Place your code here */

                        /* `#END` */

                        #ifdef UART_MIDI_OUT_EP_SERVICE_CALLBACK
                            UART_MIDI_OUT_EP_Service_Callback();
                        #endif /* UART_MIDI_OUT_EP_SERVICE_CALLBACK */
                    }
                #endif /* (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF) */

                    /* Process any local MIDI output functions */
                    UART_callbackLocalMidiEvent(UART_midiOutBuffer[outPointer] & UART_CABLE_MASK,
                                                            &UART_midiOutBuffer[outPointer + UART_EVENT_BYTE1]);
                    outPointer += UART_EVENT_LENGTH;
                }
            }

        #if (UART_EP_MANAGEMENT_DMA_AUTO)
            /* Enable OUT endpoint for communication */
            UART_EnableOutEP(UART_midi_out_ep);
        #endif  /* (UART_EP_MANAGEMENT_DMA_AUTO) */
        }
    }
#endif /* (UART_MIDI_OUT_BUFF_SIZE > 0) */


#if (UART_MIDI_IN_BUFF_SIZE > 0)
    /*******************************************************************************
    * Function Name: UART_MIDI_IN_EP_Service
    ****************************************************************************//**
    *
    *  Services the USB MIDI IN endpoint. Non-blocking.
    *  Checks that previous packet was processed by HOST, otherwise service the
    *  input endpoint on the subsequent call. It is called from the
    *  UART_MIDI_IN_Service() and from the
    *  UART_PutUsbMidiIn() function.
    *
    * \globalvars
    *  UART_midi_in_ep: Used as an IN endpoint number.
    *  UART_midiInBuffer: Function loads the data from this buffer to
    *    the USB IN endpoint.
    *   UART_midiInPointer: Cleared to zero when data are sent.
    *
    * \reentrant
    *  No
    *
    *******************************************************************************/
    void UART_MIDI_IN_EP_Service(void) 
    {
        /* Service the USB MIDI input endpoint */
        /* Check that previous packet was processed by HOST, otherwise service the USB later */
        if (UART_midiInPointer != 0u)
        {
            if(UART_GetEPState(UART_midi_in_ep) == UART_EVENT_PENDING)
            {
            #if (UART_EP_MANAGEMENT_DMA_AUTO)
                UART_LoadInEP(UART_midi_in_ep, NULL, (uint16)UART_midiInPointer);
            #else
                UART_LoadInEP(UART_midi_in_ep, UART_midiInBuffer,
                                                              (uint16) UART_midiInPointer);
            #endif /* (UART_EP_MANAGEMENT_DMA_AUTO) */

            /* Clear the midiInPointer. For DMA mode, clear this pointer in the ARB ISR when data are moved by DMA */
            #if (UART_EP_MANAGEMENT_MANUAL)
                UART_midiInPointer = 0u;
            #endif /* (UART_EP_MANAGEMENT_MANUAL) */
            }
        }
    }


    /*******************************************************************************
    * Function Name: UART_MIDI_IN_Service
    ****************************************************************************//**
    *
    *  This function services the traffic from the MIDI input ports (RX UART)
    *  and prepare data in USB MIDI IN endpoint buffer.
    *  Calls the UART_MIDI_IN_EP_Service() function to sent the
    *  data from buffer to PC. Non-blocking. Should be called from main foreground
    *  task.
    *  This function is not protected from the reentrant calls. When it is required
    *  to use this function in UART RX ISR to guaranty low latency, care should be
    *  taken to protect from reentrant calls.
    *  In PSoC 3, if this function is called from an ISR, you must declare this
    *  function as re-entrant so that different variable storage space is
    *  created by the compiler. This is automatically taken care for PSoC 4 and
    *  PSoC 5LP devices by the compiler.
    *
    * \globalvars
    *
    *   UART_midiInPointer: Cleared to zero when data are sent.
    *
    * \reentrant
    *  No
    *
    *******************************************************************************/
    void UART_MIDI_IN_Service(void) 
    {
        /* Service the MIDI UART inputs until either both receivers have no more
        *  events or until the input endpoint buffer fills up.
        */
    #if (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF)
            uint8 m1 = 0u;
            uint8 m2 = 0u;

        if (0u == UART_midiInPointer)
        {
            do
            {
                if (UART_midiInPointer <= (UART_MIDI_IN_BUFF_SIZE - UART_EVENT_LENGTH))
                {
                    /* Check MIDI1 input port for a complete event */
                    m1 = UART_MIDI1_GetEvent();
                    if (m1 != 0u)
                    {
                        UART_PrepareInBuffer(m1, (uint8 *)&UART_MIDI1_Event.msgBuff[0],
                                                                       UART_MIDI1_Event.size, UART_MIDI_CABLE_00);
                    }
                }

            #if (UART_MIDI_EXT_MODE >= UART_TWO_EXT_INTRF)
                if (UART_midiInPointer <= (UART_MIDI_IN_BUFF_SIZE - UART_EVENT_LENGTH))
                {
                    /* Check MIDI2 input port for a complete event */
                    m2 = UART_MIDI2_GetEvent();
                    if (m2 != 0u)
                    {
                        UART_PrepareInBuffer(m2, (uint8 *)&UART_MIDI2_Event.msgBuff[0],
                                                                       UART_MIDI2_Event.size, UART_MIDI_CABLE_01);
                    }
                }
            #endif /*  UART_MIDI_EXT_MODE >= UART_TWO_EXT_INTRF */
            }
            while((UART_midiInPointer <= (UART_MIDI_IN_BUFF_SIZE - UART_EVENT_LENGTH)) &&
                  ((m1 != 0u) || (m2 != 0u)));
        }
    #endif /* (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF) */

        /* Service the USB MIDI input endpoint */
        UART_MIDI_IN_EP_Service();
    }


    /***************************************************************************
    * Function Name: UART_PutUsbMidiIn
    ************************************************************************//**
    *
    *  This function puts one MIDI message into the USB MIDI In endpoint buffer.
    *  This is a MIDI input message to the host. This function is used only if
    *  the device has internal MIDI input functionality.
    *  The UART_MIDI_IN_Service() function should also be called to
    *  send the message from local buffer to the IN endpoint.
    *
    *  \param ic: The length of the MIDI message or command is described on the
    *  following table.
    *  Value          | Description
    *  ---------------|---------------------------------------------------------
    *  0              | No message (should never happen)
    *  1 - 3          | Complete MIDI message in midiMsg
    *  3 IN EP LENGTH | Complete SySEx message(without EOSEX byte) in midiMsg. The length is limited by the max BULK EP size(64)
    *  MIDI_SYSEX     | Start or continuation of SysEx message (put event bytes in midiMsg buffer)
    *  MIDI_EOSEX     | End of SysEx message (put event bytes in midiMsg buffer)
    *  MIDI_TUNEREQ   | Tune Request message (single byte system common message)
    *  0xF8 - 0xFF    | Single byte real-time message
    *
    *  \param midiMsg: pointer to MIDI message.
    *  \param cable:   cable number.
    *
    * \return
    *   Return Value          | Description
    *   ----------------------|-----------------------------------------
    *  UART_TRUE  | Host is not ready to receive this message
    *  UART_FALSE | Success transfer
    *
    * \globalvars
    *
    *  \ref UART_midi_in_ep: MIDI IN endpoint number used for
    *        sending data.
    *
    *  \ref UART_midiInPointer: Checked this variable to see if
    *        there is enough free space in the IN endpoint buffer. If buffer is
    *        full, initiate sending to PC.
    *
    * \reentrant
    *  No
    *
    ***************************************************************************/
    uint8 UART_PutUsbMidiIn(uint8 ic, const uint8 midiMsg[], uint8 cable)
                                                                
    {
        uint8 retError = UART_FALSE;

        /* Protect PrepareInBuffer() function from concurrent calls */
    #if (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF)
        MIDI1_UART_DisableRxInt();
        #if (UART_MIDI_EXT_MODE >= UART_TWO_EXT_INTRF)
            MIDI2_UART_DisableRxInt();
        #endif /* (UART_MIDI_EXT_MODE >= UART_TWO_EXT_INTRF) */
    #endif /* (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF) */

        if (UART_midiInPointer >
                    (UART_EP[UART_midi_in_ep].bufferSize - UART_EVENT_LENGTH))
        {
            UART_MIDI_IN_EP_Service();
        }

        if (UART_midiInPointer <=
                    (UART_EP[UART_midi_in_ep].bufferSize - UART_EVENT_LENGTH))
        {
            if((ic < UART_EVENT_LENGTH) || (ic >= UART_MIDI_STATUS_MASK))
            {
                UART_PrepareInBuffer(ic, midiMsg, ic, cable);
            }
            else /* Only SysEx message is greater than 4 bytes */
            {
                /* Convert SysEx message into midi message format */
                uint8 idx = 0u;
                do
                {
                    /* Process 3 bytes of message until 0-2 bytes are left. These bytes are handled by MIDI_EOSEX. */
                    UART_PrepareInBuffer(UART_MIDI_SYSEX, &midiMsg[idx],
                                                     UART_EVENT_BYTE3, cable);

                    /* Move to next 3 bytes of message */
                    ic  -= UART_EVENT_BYTE3;
                    idx += UART_EVENT_BYTE3;

                    if (UART_midiInPointer >
                        (UART_EP[UART_midi_in_ep].bufferSize - UART_EVENT_LENGTH))
                    {
                        /* Load message into endpoint */
                        UART_MIDI_IN_EP_Service();

                        if (UART_midiInPointer >
                           (UART_EP[UART_midi_in_ep].bufferSize - UART_EVENT_LENGTH))
                        {
                            /* Error condition. Host is not ready to receive this packet. */
                            retError = UART_TRUE;
                            break;
                        }
                    }
                }
                while (ic >= UART_EVENT_BYTE3);

                if (retError == UART_FALSE)
                {
                    /* Handle end of message: valid size of messages is 0, 1 and 2 */
                    UART_PrepareInBuffer(UART_MIDI_EOSEX, &midiMsg[idx], ic, cable);
                }
            }
        }
        else
        {
            /* Error condition. Host is not ready to receive this packet. */
            retError = UART_TRUE;
        }

    #if (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF)
        MIDI1_UART_EnableRxInt();
        #if (UART_MIDI_EXT_MODE >= UART_TWO_EXT_INTRF)
            MIDI2_UART_EnableRxInt();
        #endif /* (UART_MIDI_EXT_MODE >= UART_TWO_EXT_INTRF) */
    #endif /* (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF) */

        return (retError);
    }


    /*******************************************************************************
    * Function Name: UART_PrepareInBuffer
    ****************************************************************************//**
    *
    *  Builds a USB MIDI event in the input endpoint buffer at the current pointer.
    *  Puts one MIDI message into the USB MIDI In endpoint buffer.
    *
    *  \param ic:   0 = No message (should never happen)
    *        1 - 3 = Complete MIDI message at pMdat[0]
    *        MIDI_SYSEX = Start or continuation of SysEx message
    *                     (put eventLen bytes in buffer)
    *        MIDI_EOSEX = End of SysEx message
    *                     (put eventLen bytes in buffer,
    *                      and append MIDI_EOSEX)
    *        MIDI_TUNEREQ = Tune Request message (single byte system common msg)
    *        0xf8 - 0xff = Single byte real-time message
    *
    *  \param srcBuff: pointer to MIDI data
    *  \param eventLen: number of bytes in MIDI event
    *  \param cable: MIDI source port number
    *
    * \globalvars
    *  UART_midiInBuffer: This buffer is used for saving and combine the
    *    received from UART(s) and(or) generated internally by
    *    UART_PutUsbMidiIn() function messages.
    *  UART_midiInPointer: Used as an index for midiInBuffer to
    *     write data.
    *
    * \reentrant
    *  No
    *
    *******************************************************************************/
    void UART_PrepareInBuffer(uint8 ic, const uint8 srcBuff[], uint8 eventLen, uint8 cable)
                                                                 
    {
        uint8 srcBuffZero;
        uint8 srcBuffOne;

        srcBuffZero = srcBuff[0u];
        srcBuffOne  = srcBuff[1u];

        if (ic >= (UART_MIDI_STATUS_MASK | UART_MIDI_SINGLE_BYTE_MASK))
        {
            UART_midiInBuffer[UART_midiInPointer] = UART_SINGLE_BYTE | cable;
            UART_midiInPointer++;
            UART_midiInBuffer[UART_midiInPointer] = ic;
            UART_midiInPointer++;
            UART_midiInBuffer[UART_midiInPointer] = 0u;
            UART_midiInPointer++;
            UART_midiInBuffer[UART_midiInPointer] = 0u;
            UART_midiInPointer++;
        }
        else if((ic < UART_EVENT_LENGTH) || (ic == UART_MIDI_SYSEX))
        {
            if(ic == UART_MIDI_SYSEX)
            {
                UART_midiInBuffer[UART_midiInPointer] = UART_SYSEX | cable;
                UART_midiInPointer++;
            }
            else if (srcBuffZero < UART_MIDI_SYSEX)
            {
                UART_midiInBuffer[UART_midiInPointer] = (srcBuffZero >> 4u) | cable;
                UART_midiInPointer++;
            }
            else if (srcBuffZero == UART_MIDI_TUNEREQ)
            {
                UART_midiInBuffer[UART_midiInPointer] = UART_1BYTE_COMMON | cable;
                UART_midiInPointer++;
            }
            else if ((srcBuffZero == UART_MIDI_QFM) || (srcBuffZero == UART_MIDI_SONGSEL))
            {
                UART_midiInBuffer[UART_midiInPointer] = UART_2BYTE_COMMON | cable;
                UART_midiInPointer++;
            }
            else if (srcBuffZero == UART_MIDI_SPP)
            {
                UART_midiInBuffer[UART_midiInPointer] = UART_3BYTE_COMMON | cable;
                UART_midiInPointer++;
            }
            else
            {
            }

            UART_midiInBuffer[UART_midiInPointer] = srcBuffZero;
            UART_midiInPointer++;
            UART_midiInBuffer[UART_midiInPointer] = srcBuffOne;
            UART_midiInPointer++;
            UART_midiInBuffer[UART_midiInPointer] = srcBuff[2u];
            UART_midiInPointer++;
        }
        else if (ic == UART_MIDI_EOSEX)
        {
            switch (eventLen)
            {
                case 0u:
                    UART_midiInBuffer[UART_midiInPointer] = UART_SYSEX_ENDS_WITH1 | cable;
                    UART_midiInPointer++;
                    UART_midiInBuffer[UART_midiInPointer] = UART_MIDI_EOSEX;
                    UART_midiInPointer++;
                    UART_midiInBuffer[UART_midiInPointer] = 0u;
                    UART_midiInPointer++;
                    UART_midiInBuffer[UART_midiInPointer] = 0u;
                    UART_midiInPointer++;
                    break;
                case 1u:
                    UART_midiInBuffer[UART_midiInPointer] = UART_SYSEX_ENDS_WITH2 | cable;
                    UART_midiInPointer++;
                    UART_midiInBuffer[UART_midiInPointer] = srcBuffZero;
                    UART_midiInPointer++;
                    UART_midiInBuffer[UART_midiInPointer] = UART_MIDI_EOSEX;
                    UART_midiInPointer++;
                    UART_midiInBuffer[UART_midiInPointer] = 0u;
                    UART_midiInPointer++;
                    break;
                case 2u:
                    UART_midiInBuffer[UART_midiInPointer] = UART_SYSEX_ENDS_WITH3 | cable;
                    UART_midiInPointer++;
                    UART_midiInBuffer[UART_midiInPointer] = srcBuffZero;
                    UART_midiInPointer++;
                    UART_midiInBuffer[UART_midiInPointer] = srcBuffOne;
                    UART_midiInPointer++;
                    UART_midiInBuffer[UART_midiInPointer] = UART_MIDI_EOSEX;
                    UART_midiInPointer++;
                    break;
                default:
                    break;
            }
        }
        else
        {
        }
    }

#endif /* (UART_MIDI_IN_BUFF_SIZE > 0) */


/* The implementation for external serial input and output connections
*  to route USB MIDI data to and from those connections.
*/
#if (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF)
    /*******************************************************************************
    * Function Name: UART_MIDI_InitInterface
    ****************************************************************************//**
    *
    *  Initializes MIDI variables and starts the UART(s) hardware block(s).
    *
    * \sideeffect
    *  Change the priority of the UART(s) TX interrupts to be higher than the
    *  default EP ISR priority.
    *
    * \globalvars
    *   UART_MIDI_Event: initialized to zero.
    *   UART_MIDI_TxRunStat: initialized to zero.
    *
    *******************************************************************************/
    void UART_MIDI_InitInterface(void) 
    {
        UART_MIDI1_Event.length  = 0u;
        UART_MIDI1_Event.count   = 0u;
        UART_MIDI1_Event.size    = 0u;
        UART_MIDI1_Event.runstat = 0u;
        UART_MIDI1_TxRunStat     = 0u;
        UART_MIDI1_InqFlags      = 0u;

        /* Start UART block */
        MIDI1_UART_Start();

        /* Change the priority of the UART TX and RX interrupt */
        CyIntSetPriority(MIDI1_UART_TX_VECT_NUM, UART_CUSTOM_UART_TX_PRIOR_NUM);
        CyIntSetPriority(MIDI1_UART_RX_VECT_NUM, UART_CUSTOM_UART_RX_PRIOR_NUM);

    #if (UART_MIDI_EXT_MODE >= UART_TWO_EXT_INTRF)
        UART_MIDI2_Event.length  = 0u;
        UART_MIDI2_Event.count   = 0u;
        UART_MIDI2_Event.size    = 0u;
        UART_MIDI2_Event.runstat = 0u;
        UART_MIDI2_TxRunStat     = 0u;
        UART_MIDI2_InqFlags      = 0u;

        /* Start second UART block */
        MIDI2_UART_Start();

        /* Change the priority of the UART TX interrupt */
        CyIntSetPriority(MIDI2_UART_TX_VECT_NUM, UART_CUSTOM_UART_TX_PRIOR_NUM);
        CyIntSetPriority(MIDI2_UART_RX_VECT_NUM, UART_CUSTOM_UART_RX_PRIOR_NUM);
    #endif /* (UART_MIDI_EXT_MODE >= UART_TWO_EXT_INTRF) */

        /* `#START MIDI_INIT_CUSTOM` Init other extended UARTs here */

        /* `#END` */

    #ifdef UART_MIDI_INIT_CALLBACK
        UART_MIDI_Init_Callback();
    #endif /* (UART_MIDI_INIT_CALLBACK) */
    }


    /*******************************************************************************
    * Function Name: UART_ProcessMidiIn
    ****************************************************************************//**
    *
    *  Processes one byte of incoming MIDI data.
    *
    *   mData = current MIDI input data byte
    *   *rxStat = pointer to a MIDI_RX_STATUS structure
    *
    * \return
    *   0, if no complete message
    *   1 - 4, if message complete
    *   MIDI_SYSEX, if start or continuation of system exclusive
    *   MIDI_EOSEX, if end of system exclusive
    *   0xf8 - 0xff, if single byte real time message
    *
    *******************************************************************************/
    uint8 UART_ProcessMidiIn(uint8 mData, UART_MIDI_RX_STATUS *rxStat)
                                                                
    {
        uint8 midiReturn = 0u;

        /* Check for a MIDI status byte.  All status bytes, except real time messages,
        *  which are a single byte, force the start of a new buffer cycle.
        */
        if ((mData & UART_MIDI_STATUS_BYTE_MASK) != 0u)
        {
            if ((mData & UART_MIDI_STATUS_MASK) == UART_MIDI_STATUS_MASK)
            {
                if ((mData & UART_MIDI_SINGLE_BYTE_MASK) != 0u) /* System Real-Time Messages(single byte) */
                {
                    midiReturn = mData;
                }
                else                              /* System Common Messages */
                {
                    switch (mData)
                    {
                        case UART_MIDI_SYSEX:
                            rxStat->msgBuff[0u] = UART_MIDI_SYSEX;
                            rxStat->runstat = UART_MIDI_SYSEX;
                            rxStat->count = 1u;
                            rxStat->length = 3u;
                            break;
                        case UART_MIDI_EOSEX:
                            rxStat->runstat = 0u;
                            rxStat->size = rxStat->count;
                            rxStat->count = 0u;
                            midiReturn = UART_MIDI_EOSEX;
                            break;
                        case UART_MIDI_SPP:
                            rxStat->msgBuff[0u] = UART_MIDI_SPP;
                            rxStat->runstat = 0u;
                            rxStat->count = 1u;
                            rxStat->length = 3u;
                            break;
                        case UART_MIDI_SONGSEL:
                            rxStat->msgBuff[0u] = UART_MIDI_SONGSEL;
                            rxStat->runstat = 0u;
                            rxStat->count = 1u;
                            rxStat->length = 2u;
                            break;
                        case UART_MIDI_QFM:
                            rxStat->msgBuff[0u] = UART_MIDI_QFM;
                            rxStat->runstat = 0u;
                            rxStat->count = 1u;
                            rxStat->length = 2u;
                            break;
                        case UART_MIDI_TUNEREQ:
                            rxStat->msgBuff[0u] = UART_MIDI_TUNEREQ;
                            rxStat->runstat = 0u;
                            rxStat->size = 1u;
                            rxStat->count = 0u;
                            midiReturn = rxStat->size;
                            break;
                        default:
                            break;
                    }
                }
            }
            else /* Channel Messages */
            {
                rxStat->msgBuff[0u] = mData;
                rxStat->runstat = mData;
                rxStat->count = 1u;
                switch (mData & UART_MIDI_STATUS_MASK)
                {
                    case UART_MIDI_NOTE_OFF:
                    case UART_MIDI_NOTE_ON:
                    case UART_MIDI_POLY_KEY_PRESSURE:
                    case UART_MIDI_CONTROL_CHANGE:
                    case UART_MIDI_PITCH_BEND_CHANGE:
                        rxStat->length = 3u;
                        break;
                    case UART_MIDI_PROGRAM_CHANGE:
                    case UART_MIDI_CHANNEL_PRESSURE:
                        rxStat->length = 2u;
                        break;
                    default:
                        rxStat->runstat = 0u;
                        rxStat->count = 0u;
                        break;
                }
            }
        }

        /* Otherwise, it's a data byte */
        else
        {
            if (rxStat->runstat == UART_MIDI_SYSEX)
            {
                rxStat->msgBuff[rxStat->count] = mData;
                rxStat->count++;
                if (rxStat->count >= rxStat->length)
                {
                    rxStat->size = rxStat->count;
                    rxStat->count = 0u;
                    midiReturn = UART_MIDI_SYSEX;
                }
            }
            else if (rxStat->count > 0u)
            {
                rxStat->msgBuff[rxStat->count] = mData;
                rxStat->count++;
                if (rxStat->count >= rxStat->length)
                {
                    rxStat->size = rxStat->count;
                    rxStat->count = 0u;
                    midiReturn = rxStat->size;
                }
            }
            else if (rxStat->runstat != 0u)
            {
                rxStat->msgBuff[0u] = rxStat->runstat;
                rxStat->msgBuff[1u] = mData;
                rxStat->count = 2u;
                switch (rxStat->runstat & UART_MIDI_STATUS_MASK)
                {
                    case UART_MIDI_NOTE_OFF:
                    case UART_MIDI_NOTE_ON:
                    case UART_MIDI_POLY_KEY_PRESSURE:
                    case UART_MIDI_CONTROL_CHANGE:
                    case UART_MIDI_PITCH_BEND_CHANGE:
                        rxStat->length = 3u;
                        break;
                    case UART_MIDI_PROGRAM_CHANGE:
                    case UART_MIDI_CHANNEL_PRESSURE:
                        rxStat->size = rxStat->count;
                        rxStat->count = 0u;
                        midiReturn = rxStat->size;
                        break;
                    default:
                        rxStat->count = 0u;
                    break;
                }
            }
            else
            {
            }
        }
        return (midiReturn);
    }


    /*******************************************************************************
    * Function Name: UART_MIDI1_GetEvent
    ****************************************************************************//**
    *
    *  Checks for incoming MIDI data, calls the MIDI event builder if so.
    *  Returns either empty or with a complete event.
    *
    * \return
    *   0, if no complete message
    *   1 - 4, if message complete
    *   MIDI_SYSEX, if start or continuation of system exclusive
    *   MIDI_EOSEX, if end of system exclusive
    *   0xf8 - 0xff, if single byte real time message
    *
    * \globalvars
    *  UART_MIDI1_Event: RX status structure used to parse received
    *    data.
    *
    *******************************************************************************/
    uint8 UART_MIDI1_GetEvent(void) 
    {
        uint8 msgRtn = 0u;
        uint8 rxData;
        #if (MIDI1_UART_RXBUFFERSIZE >= 256u)
            uint16 rxBufferRead;
            #if (CY_PSOC3) /* This local variable is required only for PSOC3 and large buffer */
                uint16 rxBufferWrite;
            #endif /* (CY_PSOC3) */
        #else
            uint8 rxBufferRead;
        #endif /* (MIDI1_UART_RXBUFFERSIZE >= 256u) */

        uint8 rxBufferLoopDetect;
        /* Read buffer loop condition to the local variable */
        rxBufferLoopDetect = MIDI1_UART_rxBufferLoopDetect;

        if ((MIDI1_UART_rxBufferRead != MIDI1_UART_rxBufferWrite) || (rxBufferLoopDetect != 0u))
        {
            /* Protect variables that could change on interrupt by disabling Rx interrupt.*/
            #if ((MIDI1_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3))
                CyIntDisable(MIDI1_UART_RX_VECT_NUM);
            #endif /* ((MIDI1_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3)) */

            rxBufferRead = MIDI1_UART_rxBufferRead;
            #if ((MIDI1_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3))
                rxBufferWrite = MIDI1_UART_rxBufferWrite;
                CyIntEnable(MIDI1_UART_RX_VECT_NUM);
            #endif /* ((MIDI1_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3)) */

            /* Stay here until either the buffer is empty or we have a complete message
            *  in the message buffer. Note that we must use a temporary buffer pointer
            *  since it takes two instructions to increment with a wrap, and we can't
            *  risk doing that with the real pointer and getting an interrupt in between
            *  instructions.
            */

            #if ((MIDI1_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3))
                while ( ((rxBufferRead != rxBufferWrite) || (rxBufferLoopDetect != 0u)) && (msgRtn == 0u) )
            #else
                while ( ((rxBufferRead != MIDI1_UART_rxBufferWrite) || (rxBufferLoopDetect != 0u)) && (msgRtn == 0u) )
            #endif /*  ((MIDI1_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3)) */
                {
                    rxData = MIDI1_UART_rxBuffer[rxBufferRead];
                    /* Increment pointer with a wrap */
                    rxBufferRead++;
                    if (rxBufferRead >= MIDI1_UART_RXBUFFERSIZE)
                    {
                        rxBufferRead = 0u;
                    }

                    /* If loop condition was set - update real read buffer pointer
                    *  to avoid overflow status
                    */
                    if (rxBufferLoopDetect != 0u )
                    {
                        MIDI1_UART_rxBufferLoopDetect = 0u;
                    #if ((MIDI1_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3))
                        CyIntDisable(MIDI1_UART_RX_VECT_NUM);
                    #endif /*  MIDI1_UART_RXBUFFERSIZE >= 256 */

                        MIDI1_UART_rxBufferRead = rxBufferRead;
                    #if ((MIDI1_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3))
                        CyIntEnable(MIDI1_UART_RX_VECT_NUM);
                    #endif /*  MIDI1_UART_RXBUFFERSIZE >= 256 */
                    }

                    msgRtn = UART_ProcessMidiIn(rxData,
                                                    (UART_MIDI_RX_STATUS *)&UART_MIDI1_Event);

                    /* Read buffer loop condition to the local variable */
                    rxBufferLoopDetect = MIDI1_UART_rxBufferLoopDetect;
                }

            /* Finally, update the real output pointer, then return with
            *  an indication as to whether there's a complete message in the buffer.
            */
        #if ((MIDI1_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3))
            CyIntDisable(MIDI1_UART_RX_VECT_NUM);
        #endif /* ((MIDI1_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3)) */

        MIDI1_UART_rxBufferRead = rxBufferRead;
        #if ((MIDI1_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3))
            CyIntEnable(MIDI1_UART_RX_VECT_NUM);
        #endif /* ((MIDI1_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3)) */
        }

        return (msgRtn);
    }


    /*******************************************************************************
    * Function Name: UART_MIDI1_ProcessUsbOut
    ****************************************************************************//**
    *
    *  Process a USB MIDI output event.
    *  Puts data into the MIDI TX output buffer.
    *
    *  \param *epBuf: pointer on MIDI event.
    *
    * \globalvars
    *  UART_MIDI1_TxRunStat: This variable used to save the MIDI
    *    status byte and skip to send the repeated status byte in subsequent event.
    *  UART_MIDI1_InqFlags: The following flags are set when SysEx
    *    message comes.
    *    UART_INQ_SYSEX_FLAG: Non-Real Time SySEx message received.
    *    UART_INQ_IDENTITY_REQ_FLAG: Identity Request received.
    *      This bit should be cleared by user when Identity Reply message generated.
    *
    *******************************************************************************/
    void UART_MIDI1_ProcessUsbOut(const uint8 epBuf[])
                                                            
    {
        uint8 cmd;
        uint8 len;
        uint8 i;

        /* User code is required at the beginning of the procedure */
        /* `#START MIDI1_PROCESS_OUT_BEGIN` */

        /* `#END` */

    #ifdef UART_MIDI1_PROCESS_USB_OUT_ENTRY_CALLBACK
        UART_MIDI1_ProcessUsbOut_EntryCallback();
    #endif /* (UART_MIDI1_PROCESS_USB_OUT_ENTRY_CALLBACK) */

        cmd = epBuf[UART_EVENT_BYTE0] & UART_CIN_MASK;

        if ((cmd != UART_RESERVED0) && (cmd != UART_RESERVED1))
        {
            len = UART_MIDI_SIZE[cmd];
            i = UART_EVENT_BYTE1;
            /* Universal System Exclusive message parsing */
            if (cmd == UART_SYSEX)
            {
                if ((epBuf[UART_EVENT_BYTE1] == UART_MIDI_SYSEX) &&
                    (epBuf[UART_EVENT_BYTE2] == UART_MIDI_SYSEX_NON_REAL_TIME))
                {
                    /* Non-Real Time SySEx starts */
                    UART_MIDI1_InqFlags |= UART_INQ_SYSEX_FLAG;
                }
                else
                {
                    UART_MIDI1_InqFlags &= (uint8)~UART_INQ_SYSEX_FLAG;
                }
            }
            else if (cmd == UART_SYSEX_ENDS_WITH1)
            {
                UART_MIDI1_InqFlags &= (uint8)~UART_INQ_SYSEX_FLAG;
            }
            else if (cmd == UART_SYSEX_ENDS_WITH2)
            {
                UART_MIDI1_InqFlags &= (uint8)~UART_INQ_SYSEX_FLAG;
            }
            else if (cmd == UART_SYSEX_ENDS_WITH3)
            {
                /* Identify Request support */
                if ((UART_MIDI1_InqFlags & UART_INQ_SYSEX_FLAG) != 0u)
                {
                    UART_MIDI1_InqFlags &= (uint8)~UART_INQ_SYSEX_FLAG;
                    if ((epBuf[UART_EVENT_BYTE1] == UART_MIDI_SYSEX_GEN_INFORMATION) &&
                        (epBuf[UART_EVENT_BYTE2] == UART_MIDI_SYSEX_IDENTITY_REQ))
                    {
                        /* Set the flag about received the Identity Request.
                        *  The Identity Reply message may be send by user code.
                        */
                        UART_MIDI1_InqFlags |= UART_INQ_IDENTITY_REQ_FLAG;
                    }
                }
            }
            else /* Do nothing for other command */
            {
            }

            /* Running Status for Voice and Mode messages only. */
            if ((cmd >= UART_NOTE_OFF) && (cmd <= UART_PITCH_BEND_CHANGE))
            {
                if (UART_MIDI1_TxRunStat == epBuf[UART_EVENT_BYTE1])
                {
                    /* Skip the repeated Status byte */
                    i++;
                }
                else
                {
                    /* Save Status byte for next event */
                    UART_MIDI1_TxRunStat = epBuf[UART_EVENT_BYTE1];
                }
            }
            else
            {
                /* Clear Running Status */
                UART_MIDI1_TxRunStat = 0u;
            }

            /* Puts data into the MIDI TX output buffer.*/
            do
            {
                MIDI1_UART_PutChar(epBuf[i]);
                i++;
            }
            while (i <= len);
        }

        /* User code is required at the end of the procedure */
        /* `#START MIDI1_PROCESS_OUT_END` */

        /* `#END` */

    #ifdef UART_MIDI1_PROCESS_USB_OUT_EXIT_CALLBACK
        UART_MIDI1_ProcessUsbOut_ExitCallback();
    #endif /* (UART_MIDI1_PROCESS_USB_OUT_EXIT_CALLBACK) */
    }


#if (UART_MIDI_EXT_MODE >= UART_TWO_EXT_INTRF)
    /*******************************************************************************
    * Function Name: UART_MIDI2_GetEvent
    ****************************************************************************//**
    *
    *  Checks for incoming MIDI data, calls the MIDI event builder if so.
    *  Returns either empty or with a complete event.
    *
    * \return
    *   0, if no complete message
    *   1 - 4, if message complete
    *   MIDI_SYSEX, if start or continuation of system exclusive
    *   MIDI_EOSEX, if end of system exclusive
    *   0xf8 - 0xff, if single byte real time message
    *
    * \globalvars
    *  UART_MIDI2_Event: RX status structure used to parse received
    *    data.
    *
    *******************************************************************************/
    uint8 UART_MIDI2_GetEvent(void) 
    {
        uint8 msgRtn = 0u;
        uint8 rxData;

        #if (MIDI2_UART_RXBUFFERSIZE >= 256u)
            uint16 rxBufferRead;
            #if (CY_PSOC3) /* This local variable required only for PSOC3 and large buffer */
                uint16 rxBufferWrite;
            #endif /* (CY_PSOC3) */
        #else
            uint8 rxBufferRead;
        #endif /* (MIDI2_UART_RXBUFFERSIZE >= 256) */

        uint8 rxBufferLoopDetect;
        /* Read buffer loop condition to the local variable */
        rxBufferLoopDetect = MIDI2_UART_rxBufferLoopDetect;

        if ( (MIDI2_UART_rxBufferRead != MIDI2_UART_rxBufferWrite) || (rxBufferLoopDetect != 0u) )
        {
            /* Protect variables that could change on interrupt by disabling Rx interrupt.*/
            #if ((MIDI2_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3))
                CyIntDisable(MIDI2_UART_RX_VECT_NUM);
            #endif /* ((MIDI2_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3)) */
            rxBufferRead = MIDI2_UART_rxBufferRead;
            #if ((MIDI2_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3))
                rxBufferWrite = MIDI2_UART_rxBufferWrite;
                CyIntEnable(MIDI2_UART_RX_VECT_NUM);
            #endif /* ((MIDI2_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3)) */

            /* Stay here until either the buffer is empty or we have a complete message
            *  in the message buffer. Note that we must use a temporary output pointer to
            *  since it takes two instructions to increment with a wrap, and we can't
            *  risk doing that with the real pointer and getting an interrupt in between
            *  instructions.
            */

            #if ((MIDI2_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3))
                while ( ((rxBufferRead != rxBufferWrite) || (rxBufferLoopDetect != 0u)) && (msgRtn == 0u) )
            #else
                while ( ((rxBufferRead != MIDI2_UART_rxBufferWrite) || (rxBufferLoopDetect != 0u)) && (msgRtn == 0u) )
            #endif /* ((MIDI2_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3)) */
                {
                    rxData = MIDI2_UART_rxBuffer[rxBufferRead];
                    rxBufferRead++;
                    if(rxBufferRead >= MIDI2_UART_RXBUFFERSIZE)
                    {
                        rxBufferRead = 0u;
                    }

                    /* If loop condition was set - update real read buffer pointer
                    *  to avoid overflow status
                    */
                    if (rxBufferLoopDetect != 0u)
                    {
                        MIDI2_UART_rxBufferLoopDetect = 0u;
                    #if ((MIDI2_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3))
                        CyIntDisable(MIDI2_UART_RX_VECT_NUM);
                    #endif /* ((MIDI2_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3)) */

                        MIDI2_UART_rxBufferRead = rxBufferRead;
                    #if ((MIDI2_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3))
                        CyIntEnable(MIDI2_UART_RX_VECT_NUM);
                    #endif /* ((MIDI2_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3)) */
                    }

                    msgRtn = UART_ProcessMidiIn(rxData,
                                                    (UART_MIDI_RX_STATUS *)&UART_MIDI2_Event);

                    /* Read buffer loop condition to the local variable */
                    rxBufferLoopDetect = MIDI2_UART_rxBufferLoopDetect;
                }

            /* Finally, update the real output pointer, then return with
            *  an indication as to whether there's a complete message in the buffer.
            */
        #if ((MIDI2_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3))
            CyIntDisable(MIDI2_UART_RX_VECT_NUM);
        #endif /* ((MIDI2_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3)) */

            MIDI2_UART_rxBufferRead = rxBufferRead;
        #if ((MIDI2_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3))
            CyIntEnable(MIDI2_UART_RX_VECT_NUM);
        #endif /* ((MIDI2_UART_RXBUFFERSIZE >= 256u) && (CY_PSOC3)) */
        }

        return (msgRtn);
    }


    /*******************************************************************************
    * Function Name: UART_MIDI2_ProcessUsbOut
    ****************************************************************************//**
    *
    *  Process a USB MIDI output event.
    *  Puts data into the MIDI TX output buffer.
    *
    *  \param *epBuf: pointer on MIDI event.
    *
    * \globalvars
    *  UART_MIDI2_TxRunStat: This variable used to save the MIDI
    *    status byte and skip to send the repeated status byte in subsequent event.
    *  UART_MIDI2_InqFlags: The following flags are set when SysEx
    *    message comes.
    *  UART_INQ_SYSEX_FLAG: Non-Real Time SySEx message received.
    *  UART_INQ_IDENTITY_REQ_FLAG: Identity Request received.
    *    This bit should be cleared by user when Identity Reply message generated.
    *
    *******************************************************************************/
    void UART_MIDI2_ProcessUsbOut(const uint8 epBuf[])
                                                            
    {
        uint8 cmd;
        uint8 len;
        uint8 i;

        /* User code is required at the beginning of the procedure */
        /* `#START MIDI2_PROCESS_OUT_START` */

        /* `#END` */

    #ifdef UART_MIDI2_PROCESS_USB_OUT_ENTRY_CALLBACK
        UART_MIDI2_ProcessUsbOut_EntryCallback();
    #endif /* (UART_MIDI2_PROCESS_USB_OUT_ENTRY_CALLBACK) */

        cmd = epBuf[UART_EVENT_BYTE0] & UART_CIN_MASK;

        if ((cmd != UART_RESERVED0) && (cmd != UART_RESERVED1))
        {
            len = UART_MIDI_SIZE[cmd];
            i = UART_EVENT_BYTE1;

            /* Universal System Exclusive message parsing */
            if(cmd == UART_SYSEX)
            {
                if((epBuf[UART_EVENT_BYTE1] == UART_MIDI_SYSEX) &&
                   (epBuf[UART_EVENT_BYTE2] == UART_MIDI_SYSEX_NON_REAL_TIME))
                {
                    /* SySEx starts */
                    UART_MIDI2_InqFlags |= UART_INQ_SYSEX_FLAG;
                }
                else
                {
                    UART_MIDI2_InqFlags &= (uint8)~UART_INQ_SYSEX_FLAG;
                }
            }
            else if(cmd == UART_SYSEX_ENDS_WITH1)
            {
                UART_MIDI2_InqFlags &= (uint8)~UART_INQ_SYSEX_FLAG;
            }
            else if(cmd == UART_SYSEX_ENDS_WITH2)
            {
                UART_MIDI2_InqFlags &= (uint8)~UART_INQ_SYSEX_FLAG;
            }
            else if(cmd == UART_SYSEX_ENDS_WITH3)
            {
                /* Identify Request support */
                if ((UART_MIDI2_InqFlags & UART_INQ_SYSEX_FLAG) != 0u)
                {
                    UART_MIDI2_InqFlags &= (uint8)~UART_INQ_SYSEX_FLAG;

                    if((epBuf[UART_EVENT_BYTE1] == UART_MIDI_SYSEX_GEN_INFORMATION) &&
                       (epBuf[UART_EVENT_BYTE2] == UART_MIDI_SYSEX_IDENTITY_REQ))
                    {   /* Set the flag about received the Identity Request.
                        *  The Identity Reply message may be send by user code.
                        */
                        UART_MIDI2_InqFlags |= UART_INQ_IDENTITY_REQ_FLAG;
                    }
                }
            }
            else /* Do nothing for other command */
            {
            }

            /* Running Status for Voice and Mode messages only. */
            if ((cmd >= UART_NOTE_OFF) && ( cmd <= UART_PITCH_BEND_CHANGE))
            {
                if (UART_MIDI2_TxRunStat == epBuf[UART_EVENT_BYTE1])
                {   /* Skip the repeated Status byte */
                    i++;
                }
                else
                {   /* Save Status byte for next event */
                    UART_MIDI2_TxRunStat = epBuf[UART_EVENT_BYTE1];
                }
            }
            else
            {   /* Clear Running Status */
                UART_MIDI2_TxRunStat = 0u;
            }

            /* Puts data into the MIDI TX output buffer.*/
            do
            {
                MIDI2_UART_PutChar(epBuf[i]);
                i++;
            }
            while (i <= len);
        }

        /* User code is required at the end of the procedure */
        /* `#START MIDI2_PROCESS_OUT_END` */

        /* `#END` */

    #ifdef UART_MIDI2_PROCESS_USB_OUT_EXIT_CALLBACK
        UART_MIDI2_ProcessUsbOut_ExitCallback();
    #endif /* (UART_MIDI2_PROCESS_USB_OUT_EXIT_CALLBACK) */
    }
#endif /* (UART_MIDI_EXT_MODE >= UART_TWO_EXT_INTRF) */
#endif /* (UART_MIDI_EXT_MODE >= UART_ONE_EXT_INTRF) */

#endif  /*  (UART_ENABLE_MIDI_API != 0u) */


/* `#START MIDI_FUNCTIONS` Place any additional functions here */

/* `#END` */

#endif  /* defined(UART_ENABLE_MIDI_STREAMING) */


/* [] END OF FILE */
