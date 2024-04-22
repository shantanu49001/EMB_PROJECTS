/***************************************************************************//**
* \file UART_episr.c
* \version 3.20
*
* \brief
*  This file contains the Data endpoint Interrupt Service Routines.
*
********************************************************************************
* \copyright
* Copyright 2008-2016, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "UART_pvt.h"
#include "UART_cydmac.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */


#if (UART_EP1_ISR_ACTIVE)
    /******************************************************************************
    * Function Name: UART_EP_1_ISR
    ***************************************************************************//**
    *
    *  Endpoint 1 Interrupt Service Routine
    *
    ******************************************************************************/
    CY_ISR(UART_EP_1_ISR)
    {

    #ifdef UART_EP_1_ISR_ENTRY_CALLBACK
        UART_EP_1_ISR_EntryCallback();
    #endif /* (UART_EP_1_ISR_ENTRY_CALLBACK) */

        /* `#START EP1_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */
    
        UART_ClearSieEpInterruptSource(UART_SIE_INT_EP1_INTR);
            
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to be read from endpoint buffer. 
        */
    #if (CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (UART_EP[UART_EP1].addr & UART_DIR_IN))
    #endif /* (CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO) */
        {
            /* Read CR0 register to clear SIE lock. */
            (void) UART_SIE_EP_BASE.sieEp[UART_EP1].epCr0;
            
            /* Toggle all endpoint types except ISOC. */
            if (UART_GET_EP_TYPE(UART_EP1) != UART_EP_TYPE_ISOC)
            {
                UART_EP[UART_EP1].epToggle ^= UART_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            UART_EP[UART_EP1].apiEpState = UART_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(UART_ENABLE_MIDI_STREAMING) && \
            !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
            if (UART_midi_out_ep == UART_EP1)
            {
                UART_MIDI_OUT_Service();
            }
        #endif /* (UART_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)) */
    
        /* `#START EP1_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef UART_EP_1_ISR_EXIT_CALLBACK
        UART_EP_1_ISR_ExitCallback();
    #endif /* (UART_EP_1_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */
    }

#endif /* (UART_EP1_ISR_ACTIVE) */


#if (UART_EP2_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: UART_EP_2_ISR
    ****************************************************************************//**
    *
    *  Endpoint 2 Interrupt Service Routine.
    *
    *******************************************************************************/
    CY_ISR(UART_EP_2_ISR)
    {
    #ifdef UART_EP_2_ISR_ENTRY_CALLBACK
        UART_EP_2_ISR_EntryCallback();
    #endif /* (UART_EP_2_ISR_ENTRY_CALLBACK) */

        /* `#START EP2_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */

        UART_ClearSieEpInterruptSource(UART_SIE_INT_EP2_INTR);

        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to be read from endpoint buffer. 
        */
    #if (CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (UART_EP[UART_EP2].addr & UART_DIR_IN))
    #endif /* (CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO) */
        {            
            /* Read CR0 register to clear SIE lock. */
            (void) UART_SIE_EP_BASE.sieEp[UART_EP2].epCr0;
            
            /* Toggle all endpoint types except ISOC. */
            if (UART_GET_EP_TYPE(UART_EP2) != UART_EP_TYPE_ISOC)
            {
                UART_EP[UART_EP2].epToggle ^= UART_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            UART_EP[UART_EP2].apiEpState = UART_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(UART_ENABLE_MIDI_STREAMING) && \
            !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
            if (UART_midi_out_ep == UART_EP2)
            {
                UART_MIDI_OUT_Service();
            }
        #endif /* (UART_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)) */        
    
        /* `#START EP2_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef UART_EP_2_ISR_EXIT_CALLBACK
        UART_EP_2_ISR_ExitCallback();
    #endif /* (UART_EP_2_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (UART_EP2_ISR_ACTIVE) */


#if (UART_EP3_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: UART_EP_3_ISR
    ****************************************************************************//**
    *
    *  Endpoint 3 Interrupt Service Routine.
    *
    *******************************************************************************/
    CY_ISR(UART_EP_3_ISR)
    {
    #ifdef UART_EP_3_ISR_ENTRY_CALLBACK
        UART_EP_3_ISR_EntryCallback();
    #endif /* (UART_EP_3_ISR_ENTRY_CALLBACK) */

        /* `#START EP3_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */

        UART_ClearSieEpInterruptSource(UART_SIE_INT_EP3_INTR);    

        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to be read from endpoint buffer. 
        */
    #if (CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (UART_EP[UART_EP3].addr & UART_DIR_IN))
    #endif /* (UART_EP_MANAGEMENT_DMA_AUTO) */
        {            
            /* Read CR0 register to clear SIE lock. */
            (void) UART_SIE_EP_BASE.sieEp[UART_EP3].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (UART_GET_EP_TYPE(UART_EP3) != UART_EP_TYPE_ISOC)
            {
                UART_EP[UART_EP3].epToggle ^= UART_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            UART_EP[UART_EP3].apiEpState = UART_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(UART_ENABLE_MIDI_STREAMING) && \
            !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
            if (UART_midi_out_ep == UART_EP3)
            {
                UART_MIDI_OUT_Service();
            }
        #endif /* (UART_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)) */        

        /* `#START EP3_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef UART_EP_3_ISR_EXIT_CALLBACK
        UART_EP_3_ISR_ExitCallback();
    #endif /* (UART_EP_3_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (UART_EP3_ISR_ACTIVE) */


#if (UART_EP4_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: UART_EP_4_ISR
    ****************************************************************************//**
    *
    *  Endpoint 4 Interrupt Service Routine.
    *
    *******************************************************************************/
    CY_ISR(UART_EP_4_ISR)
    {
    #ifdef UART_EP_4_ISR_ENTRY_CALLBACK
        UART_EP_4_ISR_EntryCallback();
    #endif /* (UART_EP_4_ISR_ENTRY_CALLBACK) */

        /* `#START EP4_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */

        UART_ClearSieEpInterruptSource(UART_SIE_INT_EP4_INTR);
        
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (UART_EP[UART_EP4].addr & UART_DIR_IN))
    #endif /* (UART_EP_MANAGEMENT_DMA_AUTO) */
        {
            /* Read CR0 register to clear SIE lock. */
            (void) UART_SIE_EP_BASE.sieEp[UART_EP4].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (UART_GET_EP_TYPE(UART_EP4) != UART_EP_TYPE_ISOC)
            {
                UART_EP[UART_EP4].epToggle ^= UART_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            UART_EP[UART_EP4].apiEpState = UART_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(UART_ENABLE_MIDI_STREAMING) && \
            !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
            if(UART_midi_out_ep == UART_EP4)
            {
                UART_MIDI_OUT_Service();
            }
        #endif /* (UART_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)) */        

        /* `#START EP4_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef UART_EP_4_ISR_EXIT_CALLBACK
        UART_EP_4_ISR_ExitCallback();
    #endif /* (UART_EP_4_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (UART_EP4_ISR_ACTIVE) */


#if (UART_EP5_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: UART_EP_5_ISR
    ****************************************************************************//**
    *
    *  Endpoint 5 Interrupt Service Routine
    *
    *
    *******************************************************************************/
    CY_ISR(UART_EP_5_ISR)
    {
    #ifdef UART_EP_5_ISR_ENTRY_CALLBACK
        UART_EP_5_ISR_EntryCallback();
    #endif /* (UART_EP_5_ISR_ENTRY_CALLBACK) */

        /* `#START EP5_USER_CODE` Place your code here */

        /* `#END` */

    #if (defined(UART_ENABLE_MIDI_STREAMING) && !defined(UART_MAIN_SERVICE_MIDI_OUT) && \
                 UART_ISR_SERVICE_MIDI_OUT && CY_PSOC3)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */

        UART_ClearSieEpInterruptSource(UART_SIE_INT_EP5_INTR);
    
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (UART_EP[UART_EP5].addr & UART_DIR_IN))
    #endif /* (UART_EP_MANAGEMENT_DMA_AUTO) */
        {            
            /* Read CR0 register to clear SIE lock. */
            (void) UART_SIE_EP_BASE.sieEp[UART_EP5].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (UART_GET_EP_TYPE(UART_EP5) != UART_EP_TYPE_ISOC)
            {
                UART_EP[UART_EP5].epToggle ^= UART_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            UART_EP[UART_EP5].apiEpState = UART_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO))        
        #if (defined(UART_ENABLE_MIDI_STREAMING) && \
            !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
            if (UART_midi_out_ep == UART_EP5)
            {
                UART_MIDI_OUT_Service();
            }
        #endif /* (UART_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)) */

        /* `#START EP5_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef UART_EP_5_ISR_EXIT_CALLBACK
        UART_EP_5_ISR_ExitCallback();
    #endif /* (UART_EP_5_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (UART_EP5_ISR_ACTIVE) */


#if (UART_EP6_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: UART_EP_6_ISR
    ****************************************************************************//**
    *
    *  Endpoint 6 Interrupt Service Routine.
    *
    *
    *******************************************************************************/
    CY_ISR(UART_EP_6_ISR)
    {
    #ifdef UART_EP_6_ISR_ENTRY_CALLBACK
        UART_EP_6_ISR_EntryCallback();
    #endif /* (UART_EP_6_ISR_ENTRY_CALLBACK) */

        /* `#START EP6_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */

        UART_ClearSieEpInterruptSource(UART_SIE_INT_EP6_INTR);
        
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (UART_EP[UART_EP6].addr & UART_DIR_IN))
    #endif /* (UART_EP_MANAGEMENT_DMA_AUTO) */
        {
            /* Read CR0 register to clear SIE lock. */
            (void) UART_SIE_EP_BASE.sieEp[UART_EP6].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (UART_GET_EP_TYPE(UART_EP6) != UART_EP_TYPE_ISOC)
            {
                UART_EP[UART_EP6].epToggle ^= UART_EPX_CNT_DATA_TOGGLE;
            }
            
            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            UART_EP[UART_EP6].apiEpState = UART_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(UART_ENABLE_MIDI_STREAMING) && \
            !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
            if (UART_midi_out_ep == UART_EP6)
            {
                UART_MIDI_OUT_Service();
            }
        #endif /* (UART_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)) */

        /* `#START EP6_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef UART_EP_6_ISR_EXIT_CALLBACK
        UART_EP_6_ISR_ExitCallback();
    #endif /* (UART_EP_6_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (UART_EP6_ISR_ACTIVE) */


#if (UART_EP7_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: UART_EP_7_ISR
    ****************************************************************************//**
    *
    *  Endpoint 7 Interrupt Service Routine.
    *
    *
    *******************************************************************************/
    CY_ISR(UART_EP_7_ISR)
    {
    #ifdef UART_EP_7_ISR_ENTRY_CALLBACK
        UART_EP_7_ISR_EntryCallback();
    #endif /* (UART_EP_7_ISR_ENTRY_CALLBACK) */

        /* `#START EP7_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */
    
        UART_ClearSieEpInterruptSource(UART_SIE_INT_EP7_INTR);
        
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (UART_EP[UART_EP7].addr & UART_DIR_IN))
    #endif /* (UART_EP_MANAGEMENT_DMA_AUTO) */
        {           
            /* Read CR0 register to clear SIE lock. */
            (void) UART_SIE_EP_BASE.sieEp[UART_EP7].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (UART_GET_EP_TYPE(UART_EP7) != UART_EP_TYPE_ISOC)
            {
                UART_EP[UART_EP7].epToggle ^= UART_EPX_CNT_DATA_TOGGLE;
            }
            
            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            UART_EP[UART_EP7].apiEpState = UART_EVENT_PENDING;
        }


    #if (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(UART_ENABLE_MIDI_STREAMING) && \
            !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
            if(UART_midi_out_ep == UART_EP7)
            {
                UART_MIDI_OUT_Service();
            }
        #endif /* (UART_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)) */

        /* `#START EP7_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef UART_EP_7_ISR_EXIT_CALLBACK
        UART_EP_7_ISR_ExitCallback();
    #endif /* (UART_EP_7_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (UART_EP7_ISR_ACTIVE) */


#if (UART_EP8_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: UART_EP_8_ISR
    ****************************************************************************//**
    *
    *  Endpoint 8 Interrupt Service Routine
    *
    *
    *******************************************************************************/
    CY_ISR(UART_EP_8_ISR)
    {
    #ifdef UART_EP_8_ISR_ENTRY_CALLBACK
        UART_EP_8_ISR_EntryCallback();
    #endif /* (UART_EP_8_ISR_ENTRY_CALLBACK) */

        /* `#START EP8_USER_CODE` Place your code here */

        /* `#END` */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        {
            uint8 intEn = EA;
            CyGlobalIntEnable;  /* Enable nested interrupts. */
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */

        UART_ClearSieEpInterruptSource(UART_SIE_INT_EP8_INTR);
        
        /* Notifies user that transfer IN or OUT transfer is completed.
        * IN endpoint: endpoint buffer can be reloaded, Host is read data.
        * OUT endpoint: data is ready to read from endpoint buffer. 
        */
    #if (CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)
        if (0u != (UART_EP[UART_EP8].addr & UART_DIR_IN))
    #endif /* (UART_EP_MANAGEMENT_DMA_AUTO) */
        {
            /* Read CR0 register to clear SIE lock. */
            (void) UART_SIE_EP_BASE.sieEp[UART_EP8].epCr0;

            /* Toggle all endpoint types except ISOC. */
            if (UART_GET_EP_TYPE(UART_EP8) != UART_EP_TYPE_ISOC)
            {
                UART_EP[UART_EP8].epToggle ^= UART_EPX_CNT_DATA_TOGGLE;
            }

            /* EP_MANAGEMENT_DMA_AUTO (Ticket ID# 214187): For OUT endpoint this event is used to notify
            * user that DMA has completed copying data from OUT endpoint which is not completely true.
            * Because last chunk of data is being copied.
            * For CY_PSOC 3/5LP: it is acceptable as DMA is really fast.
            * For CY_PSOC4: this event is set in Arbiter interrupt (source is DMA_TERMIN).
            */
            UART_EP[UART_EP8].apiEpState = UART_EVENT_PENDING;
        }

    #if (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO))
        #if (defined(UART_ENABLE_MIDI_STREAMING) && \
            !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
            if (UART_midi_out_ep == UART_EP8)
            {
                UART_MIDI_OUT_Service();
            }
        #endif /* (UART_ISR_SERVICE_MIDI_OUT) */
    #endif /* (!(CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)) */

        /* `#START EP8_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef UART_EP_8_ISR_EXIT_CALLBACK
        UART_EP_8_ISR_ExitCallback();
    #endif /* (UART_EP_8_ISR_EXIT_CALLBACK) */

    #if (CY_PSOC3 && defined(UART_ENABLE_MIDI_STREAMING) && \
        !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
        
            EA = intEn; /* Restore nested interrupt configuration. */
        }
    #endif /* (CY_PSOC3 && UART_ISR_SERVICE_MIDI_OUT) */
    }
#endif /* (UART_EP8_ISR_ACTIVE) */


#if (UART_SOF_ISR_ACTIVE)
    /*******************************************************************************
    * Function Name: UART_SOF_ISR
    ****************************************************************************//**
    *
    *  Start of Frame Interrupt Service Routine.
    *
    *
    *******************************************************************************/
    CY_ISR(UART_SOF_ISR)
    {
    #ifdef UART_SOF_ISR_ENTRY_CALLBACK
        UART_SOF_ISR_EntryCallback();
    #endif /* (UART_SOF_ISR_ENTRY_CALLBACK) */

        /* `#START SOF_USER_CODE` Place your code here */

        /* `#END` */

        UART_ClearSieInterruptSource(UART_INTR_SIE_SOF_INTR);

    #ifdef UART_SOF_ISR_EXIT_CALLBACK
        UART_SOF_ISR_ExitCallback();
    #endif /* (UART_SOF_ISR_EXIT_CALLBACK) */
    }
#endif /* (UART_SOF_ISR_ACTIVE) */


#if (UART_BUS_RESET_ISR_ACTIVE)
/*******************************************************************************
* Function Name: UART_BUS_RESET_ISR
****************************************************************************//**
*
*  USB Bus Reset Interrupt Service Routine.  Calls _Start with the same
*  parameters as the last USER call to _Start
*
*
*******************************************************************************/
CY_ISR(UART_BUS_RESET_ISR)
{
#ifdef UART_BUS_RESET_ISR_ENTRY_CALLBACK
    UART_BUS_RESET_ISR_EntryCallback();
#endif /* (UART_BUS_RESET_ISR_ENTRY_CALLBACK) */

    /* `#START BUS_RESET_USER_CODE` Place your code here */

    /* `#END` */

    UART_ClearSieInterruptSource(UART_INTR_SIE_BUS_RESET_INTR);

    UART_ReInitComponent();

#ifdef UART_BUS_RESET_ISR_EXIT_CALLBACK
    UART_BUS_RESET_ISR_ExitCallback();
#endif /* (UART_BUS_RESET_ISR_EXIT_CALLBACK) */
}
#endif /* (UART_BUS_RESET_ISR_ACTIVE) */


#if (UART_LPM_ACTIVE)
/***************************************************************************
* Function Name: UART_INTR_LPM_ISR
************************************************************************//**
*
*   Interrupt Service Routine for LPM of the interrupt sources.
*
*
***************************************************************************/
CY_ISR(UART_LPM_ISR)
{
#ifdef UART_LPM_ISR_ENTRY_CALLBACK
    UART_LPM_ISR_EntryCallback();
#endif /* (UART_LPM_ISR_ENTRY_CALLBACK) */

    /* `#START LPM_BEGIN_USER_CODE` Place your code here */

    /* `#END` */

    UART_ClearSieInterruptSource(UART_INTR_SIE_LPM_INTR);

    /* `#START LPM_END_USER_CODE` Place your code here */

    /* `#END` */

#ifdef UART_LPM_ISR_EXIT_CALLBACK
    UART_LPM_ISR_ExitCallback();
#endif /* (UART_LPM_ISR_EXIT_CALLBACK) */
}
#endif /* (UART_LPM_ACTIVE) */


#if (UART_EP_MANAGEMENT_DMA && UART_ARB_ISR_ACTIVE)
    /***************************************************************************
    * Function Name: UART_ARB_ISR
    ************************************************************************//**
    *
    *  Arbiter Interrupt Service Routine.
    *
    *
    ***************************************************************************/
    CY_ISR(UART_ARB_ISR)
    {
        uint8 arbIntrStatus;
        uint8 epStatus;
        uint8 ep = UART_EP1;

    #ifdef UART_ARB_ISR_ENTRY_CALLBACK
        UART_ARB_ISR_EntryCallback();
    #endif /* (UART_ARB_ISR_ENTRY_CALLBACK) */

        /* `#START ARB_BEGIN_USER_CODE` Place your code here */

        /* `#END` */

        /* Get pending ARB interrupt sources. */
        arbIntrStatus = UART_ARB_INT_SR_REG;

        while (0u != arbIntrStatus)
        {
            /* Check which EP is interrupt source. */
            if (0u != (arbIntrStatus & 0x01u))
            {
                /* Get endpoint enable interrupt sources. */
                epStatus = (UART_ARB_EP_BASE.arbEp[ep].epSr & UART_ARB_EP_BASE.arbEp[ep].epIntEn);

                /* Handle IN endpoint buffer full event: happens only once when endpoint buffer is loaded. */
                if (0u != (epStatus & UART_ARB_EPX_INT_IN_BUF_FULL))
                {
                    if (0u != (UART_EP[ep].addr & UART_DIR_IN))
                    {
                        /* Clear data ready status. */
                        UART_ARB_EP_BASE.arbEp[ep].epCfg &= (uint8) ~UART_ARB_EPX_CFG_IN_DATA_RDY;

                    #if (CY_PSOC3 || CY_PSOC5LP)
                        #if (UART_EP_MANAGEMENT_DMA_AUTO && (UART_EP_DMA_AUTO_OPT == 0u))
                            /* Set up common area DMA with rest of data. */
                            if(UART_inLength[ep] > UART_DMA_BYTES_PER_BURST)
                            {
                                UART_LoadNextInEP(ep, 0u);
                            }
                            else
                            {
                                UART_inBufFull[ep] = 1u;
                            }
                        #endif /* (UART_EP_MANAGEMENT_DMA_AUTO && (UART_EP_DMA_AUTO_OPT == 0u)) */
                    #endif /* (CY_PSOC3 || CY_PSOC5LP) */

                        /* Arm IN endpoint. */
                        UART_SIE_EP_BASE.sieEp[ep].epCr0 = UART_EP[ep].epMode;

                    #if (defined(UART_ENABLE_MIDI_STREAMING) && UART_ISR_SERVICE_MIDI_IN)
                        if (ep == UART_midi_in_ep)
                        {
                            /* Clear MIDI input pointer. */
                            UART_midiInPointer = 0u;
                        }
                    #endif /* (UART_ENABLE_MIDI_STREAMING) */
                    }
                }

            #if (UART_EP_MANAGEMENT_DMA_MANUAL)
                /* Handle DMA completion event for OUT endpoints. */
                if (0u != (epStatus & UART_ARB_EPX_SR_DMA_GNT))
                {
                    if (0u == (UART_EP[ep].addr & UART_DIR_IN))
                    {
                        /* Notify user that data has been copied from endpoint buffer. */
                        UART_EP[ep].apiEpState = UART_NO_EVENT_PENDING;

                        /* DMA done coping data: OUT endpoint has to be re-armed by user. */
                    }
                }
            #endif /* (UART_EP_MANAGEMENT_DMA_MANUAL) */

            #if (CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO)
                /* Handle DMA completion event for OUT endpoints. */
                if (0u != (epStatus & UART_ARB_EPX_INT_DMA_TERMIN))
                {
                    uint32 channelNum = UART_DmaChan[ep];

                    /* Restore burst counter for endpoint. */
                    UART_DmaEpBurstCnt[ep] = UART_DMA_GET_BURST_CNT(UART_DmaEpBurstCntBackup[ep]);

                    /* Disable DMA channel to restore descriptor configuration. The on-going transfer is aborted. */
                    UART_CyDmaChDisable(channelNum);

                    /* Generate DMA tr_out signal to notify USB IP that DMA is done. This signal is not generated
                    * when transfer was aborted (it occurs when host writes less bytes than buffer size).
                    */
                    UART_CyDmaTriggerOut(UART_DmaBurstEndOut[ep]);

                    /* Restore destination address for output endpoint. */
                    UART_CyDmaSetDstAddress(channelNum, UART_DMA_DESCR0, (void*) ((uint32) UART_DmaEpBufferAddrBackup[ep]));
                    UART_CyDmaSetDstAddress(channelNum, UART_DMA_DESCR1, (void*) ((uint32) UART_DmaEpBufferAddrBackup[ep] +
                                                                                                                   UART_DMA_BYTES_PER_BURST));

                    /* Restore number of data elements to transfer which was adjusted for last burst. */
                    if (0u != (UART_DmaEpLastBurstEl[ep] & UART_DMA_DESCR_REVERT))
                    {
                        UART_CyDmaSetNumDataElements(channelNum, (UART_DmaEpLastBurstEl[ep] >> UART_DMA_DESCR_SHIFT),
                                                                             UART_DMA_GET_MAX_ELEM_PER_BURST(UART_DmaEpLastBurstEl[ep]));
                    }

                    /* Validate descriptor 0 and 1 (also reset current state). Command to start with descriptor 0. */
                    UART_CyDmaValidateDescriptor(channelNum, UART_DMA_DESCR0);
                    if (UART_DmaEpBurstCntBackup[ep] > 1u)
                    {
                        UART_CyDmaValidateDescriptor(channelNum, UART_DMA_DESCR1);
                    }
                    UART_CyDmaSetDescriptor0Next(channelNum);

                    /* Enable DMA channel: configuration complete. */
                    UART_CyDmaChEnable(channelNum);
                    
                    
                    /* Read CR0 register to clear SIE lock. */
                    (void) UART_SIE_EP_BASE.sieEp[ep].epCr0;
                    
                    /* Toggle all endpoint types except ISOC. */
                    if (UART_GET_EP_TYPE(ep) != UART_EP_TYPE_ISOC)
                    {
                        UART_EP[ep].epToggle ^= UART_EPX_CNT_DATA_TOGGLE;
                    }
            
                    /* Notify user that data has been copied from endpoint buffer. */
                    UART_EP[ep].apiEpState = UART_EVENT_PENDING;
                    
                #if (defined(UART_ENABLE_MIDI_STREAMING) && \
                    !defined(UART_MAIN_SERVICE_MIDI_OUT) && UART_ISR_SERVICE_MIDI_OUT)
                    if (UART_midi_out_ep == ep)
                    {
                        UART_MIDI_OUT_Service();
                    }
                #endif /* (UART_ISR_SERVICE_MIDI_OUT) */
                }
            #endif /* (CY_PSOC4 && UART_EP_MANAGEMENT_DMA_AUTO) */


                /* `#START ARB_USER_CODE` Place your code here for handle Buffer Underflow/Overflow */

                /* `#END` */

            #ifdef UART_ARB_ISR_CALLBACK
                UART_ARB_ISR_Callback(ep, epStatus);
            #endif /* (UART_ARB_ISR_CALLBACK) */

                /* Clear serviced endpoint interrupt sources. */
                UART_ARB_EP_BASE.arbEp[ep].epSr = epStatus;
            }

            ++ep;
            arbIntrStatus >>= 1u;
        }

        /* `#START ARB_END_USER_CODE` Place your code here */

        /* `#END` */

    #ifdef UART_ARB_ISR_EXIT_CALLBACK
        UART_ARB_ISR_ExitCallback();
    #endif /* (UART_ARB_ISR_EXIT_CALLBACK) */
    }

#endif /*  (UART_ARB_ISR_ACTIVE && UART_EP_MANAGEMENT_DMA) */


#if (UART_EP_MANAGEMENT_DMA_AUTO)
#if (CY_PSOC4)

    /******************************************************************************
    * Function Name: UART_EPxDmaDone
    ***************************************************************************//**
    *
    * \internal
    *  Endpoint  DMA Done Interrupt Service Routine basic function .
    *  
    *  \param dmaCh
    *  number of DMA channel
    *  
    *  \param ep
    *  number of USB end point
    *  
    *  \param dmaDone
    *  transfer completion flag
    *  
    *  \return
    *   updated transfer completion flag
    *
    ******************************************************************************/
    CY_INLINE static void UART_EPxDmaDone(uint8 dmaCh, uint8 ep)
    {
        uint32 nextAddr;

        /* Manage data elements which remain to transfer. */
        if (0u != UART_DmaEpBurstCnt[ep])
        {
            if(UART_DmaEpBurstCnt[ep] <= 2u)
            {
                /* Adjust length of last burst. */
                UART_CyDmaSetNumDataElements(dmaCh,
                                                    ((uint32) UART_DmaEpLastBurstEl[ep] >> UART_DMA_DESCR_SHIFT),
                                                    ((uint32) UART_DmaEpLastBurstEl[ep] &  UART_DMA_BURST_BYTES_MASK));
            }
            

            /* Advance source for input endpoint or destination for output endpoint. */
            if (0u != (UART_EP[ep].addr & UART_DIR_IN))
            {
                /* Change source for descriptor 0. */
                nextAddr = (uint32) UART_CyDmaGetSrcAddress(dmaCh, UART_DMA_DESCR0);
                nextAddr += (2u * UART_DMA_BYTES_PER_BURST);
                UART_CyDmaSetSrcAddress(dmaCh, UART_DMA_DESCR0, (void *) nextAddr);

                /* Change source for descriptor 1. */
                nextAddr += UART_DMA_BYTES_PER_BURST;
                UART_CyDmaSetSrcAddress(dmaCh, UART_DMA_DESCR1, (void *) nextAddr);
            }
            else
            {
                /* Change destination for descriptor 0. */
                nextAddr  = (uint32) UART_CyDmaGetDstAddress(dmaCh, UART_DMA_DESCR0);
                nextAddr += (2u * UART_DMA_BYTES_PER_BURST);
                UART_CyDmaSetDstAddress(dmaCh, UART_DMA_DESCR0, (void *) nextAddr);

                /* Change destination for descriptor 1. */
                nextAddr += UART_DMA_BYTES_PER_BURST;
                UART_CyDmaSetDstAddress(dmaCh, UART_DMA_DESCR1, (void *) nextAddr);
            }

            /* Enable DMA to execute transfer as it was disabled because there were no valid descriptor. */
            UART_CyDmaValidateDescriptor(dmaCh, UART_DMA_DESCR0);
            
            --UART_DmaEpBurstCnt[ep];
            if (0u != UART_DmaEpBurstCnt[ep])
            {
                UART_CyDmaValidateDescriptor(dmaCh, UART_DMA_DESCR1);
                --UART_DmaEpBurstCnt[ep];
            }
            
            UART_CyDmaChEnable (dmaCh);
            UART_CyDmaTriggerIn(UART_DmaReqOut[ep]);
        }
        else
        {
            /* No data to transfer. False DMA trig. Ignore.  */
        }

    }

    #if (UART_DMA1_ACTIVE)
        /******************************************************************************
        * Function Name: UART_EP1_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 1 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void UART_EP1_DMA_DONE_ISR(void)
        {

            UART_EPxDmaDone((uint8)UART_EP1_DMA_CH,
                                                  UART_EP1);
                
        }
    #endif /* (UART_DMA1_ACTIVE) */


    #if (UART_DMA2_ACTIVE)
        /******************************************************************************
        * Function Name: UART_EP2_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 2 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void UART_EP2_DMA_DONE_ISR(void)
        {

            UART_EPxDmaDone((uint8)UART_EP2_DMA_CH,
                                                  UART_EP2);
        }
    #endif /* (UART_DMA2_ACTIVE) */


    #if (UART_DMA3_ACTIVE)
        /******************************************************************************
        * Function Name: UART_EP3_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 3 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void UART_EP3_DMA_DONE_ISR(void)
        {

            UART_EPxDmaDone((uint8)UART_EP3_DMA_CH,
                                                  UART_EP3);
        }
    #endif /* (UART_DMA3_ACTIVE) */


    #if (UART_DMA4_ACTIVE)
        /******************************************************************************
        * Function Name: UART_EP4_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 4 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void UART_EP4_DMA_DONE_ISR(void)
        {

            UART_EPxDmaDone((uint8)UART_EP4_DMA_CH,
                                                  UART_EP4);
        }
    #endif /* (UART_DMA4_ACTIVE) */


    #if (UART_DMA5_ACTIVE)
        /******************************************************************************
        * Function Name: UART_EP5_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 5 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void UART_EP5_DMA_DONE_ISR(void)
        {

            UART_EPxDmaDone((uint8)UART_EP5_DMA_CH,
                                                  UART_EP5);
        }
    #endif /* (UART_DMA5_ACTIVE) */


    #if (UART_DMA6_ACTIVE)
        /******************************************************************************
        * Function Name: UART_EP6_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 6 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void UART_EP6_DMA_DONE_ISR(void)
        {

            UART_EPxDmaDone((uint8)UART_EP6_DMA_CH,
                                                  UART_EP6);
        }
    #endif /* (UART_DMA6_ACTIVE) */


    #if (UART_DMA7_ACTIVE)
        /******************************************************************************
        * Function Name: UART_EP7_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 7 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void UART_EP7_DMA_DONE_ISR(void)
        {

            UART_EPxDmaDone((uint8)UART_EP7_DMA_CH,
                                                  UART_EP7);
        }
    #endif /* (UART_DMA7_ACTIVE) */


    #if (UART_DMA8_ACTIVE)
        /******************************************************************************
        * Function Name: UART_EP8_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  Endpoint 8 DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        void UART_EP8_DMA_DONE_ISR(void)
        {

            UART_EPxDmaDone((uint8)UART_EP8_DMA_CH,
                                                  UART_EP8);
        }
    #endif /* (UART_DMA8_ACTIVE) */


#else
    #if (UART_EP_DMA_AUTO_OPT == 0u)
        /******************************************************************************
        * Function Name: UART_EP_DMA_DONE_ISR
        ***************************************************************************//**
        *
        *  DMA Done Interrupt Service Routine.
        *
        *
        ******************************************************************************/
        CY_ISR(UART_EP_DMA_DONE_ISR)
        {
            uint8 int8Status;
            uint8 int17Status;
            uint8 ep_status;
            uint8 ep = UART_EP1;

        #ifdef UART_EP_DMA_DONE_ISR_ENTRY_CALLBACK
            UART_EP_DMA_DONE_ISR_EntryCallback();
        #endif /* (UART_EP_DMA_DONE_ISR_ENTRY_CALLBACK) */

            /* `#START EP_DMA_DONE_BEGIN_USER_CODE` Place your code here */

            /* `#END` */

            /* Read clear on read status register with EP source of interrupt. */
            int17Status = UART_EP17_DMA_Done_SR_Read() & UART_EP17_SR_MASK;
            int8Status  = UART_EP8_DMA_Done_SR_Read()  & UART_EP8_SR_MASK;

            while (int8Status != 0u)
            {
                while (int17Status != 0u)
                {
                    if ((int17Status & 1u) != 0u)  /* If EpX interrupt present. */
                    {
                        /* Read Endpoint Status Register. */
                        ep_status = UART_ARB_EP_BASE.arbEp[ep].epSr;

                        if ((0u == (ep_status & UART_ARB_EPX_SR_IN_BUF_FULL)) &&
                            (0u ==UART_inBufFull[ep]))
                        {
                            /* `#START EP_DMA_DONE_USER_CODE` Place your code here */

                            /* `#END` */

                        #ifdef UART_EP_DMA_DONE_ISR_CALLBACK
                            UART_EP_DMA_DONE_ISR_Callback();
                        #endif /* (UART_EP_DMA_DONE_ISR_CALLBACK) */

                            /* Transfer again 2 last bytes into pre-fetch endpoint area. */
                            UART_ARB_EP_BASE.arbEp[ep].rwWaMsb = 0u;
                            UART_ARB_EP_BASE.arbEp[ep].rwWa = (UART_DMA_BYTES_PER_BURST * ep) - UART_DMA_BYTES_REPEAT;
                            UART_LoadNextInEP(ep, 1u);

                            /* Set Data ready status to generate DMA request. */
                            UART_ARB_EP_BASE.arbEp[ep].epCfg |= UART_ARB_EPX_CFG_IN_DATA_RDY;
                        }
                    }

                    ep++;
                    int17Status >>= 1u;
                }

                int8Status >>= 1u;

                if (int8Status != 0u)
                {
                    /* Prepare pointer for EP8. */
                    ep = UART_EP8;
                    int17Status = int8Status & 0x01u;
                }
            }

            /* `#START EP_DMA_DONE_END_USER_CODE` Place your code here */

            /* `#END` */

    #ifdef UART_EP_DMA_DONE_ISR_EXIT_CALLBACK
        UART_EP_DMA_DONE_ISR_ExitCallback();
    #endif /* (UART_EP_DMA_DONE_ISR_EXIT_CALLBACK) */
        }
    #endif /* (UART_EP_DMA_AUTO_OPT == 0u) */
#endif /* (CY_PSOC4) */
#endif /* (UART_EP_MANAGEMENT_DMA_AUTO) */


#if (CY_PSOC4)
    /***************************************************************************
    * Function Name: UART_IntrHandler
    ************************************************************************//**
    *
    *   Interrupt handler for Hi/Mid/Low ISRs.
    *
    *  regCause - The cause register of interrupt. One of the three variants:
    *       UART_INTR_CAUSE_LO_REG - Low interrupts.
    *       UART_INTR_CAUSE_MED_REG - Med interrupts.
    *       UART_INTR_CAUSE_HI_REG - - High interrupts.
    *
    *
    ***************************************************************************/
    CY_INLINE static void UART_IntrHandler(uint32 intrCause)
    {
        /* Array of pointers to component interrupt handlers. */
        static const cyisraddress UART_isrCallbacks[] =
        {

        };

        uint32 cbIdx = 0u;

        /* Check arbiter interrupt source first. */
        if (0u != (intrCause & UART_INTR_CAUSE_ARB_INTR))
        {
            UART_isrCallbacks[UART_ARB_EP_INTR_NUM]();
        }

        /* Check all other interrupt sources (except arbiter and resume). */
        intrCause = (intrCause  & UART_INTR_CAUSE_CTRL_INTR_MASK) |
                    ((intrCause & UART_INTR_CAUSE_EP1_8_INTR_MASK) >>
                                  UART_INTR_CAUSE_EP_INTR_SHIFT);

        /* Call interrupt handlers for active interrupt sources. */
        while (0u != intrCause)
        {
            if (0u != (intrCause & 0x1u))
            {
                 UART_isrCallbacks[cbIdx]();
            }

            intrCause >>= 1u;
            ++cbIdx;
        }
    }


    /***************************************************************************
    * Function Name: UART_INTR_HI_ISR
    ************************************************************************//**
    *
    *   Interrupt Service Routine for the high group of the interrupt sources.
    *
    *
    ***************************************************************************/
    CY_ISR(UART_INTR_HI_ISR)
    {
        UART_IntrHandler(UART_INTR_CAUSE_HI_REG);
    }

    /***************************************************************************
    * Function Name: UART_INTR_MED_ISR
    ************************************************************************//**
    *
    *   Interrupt Service Routine for the medium group of the interrupt sources.
    *
    *
    ***************************************************************************/
    CY_ISR(UART_INTR_MED_ISR)
    {
       UART_IntrHandler(UART_INTR_CAUSE_MED_REG);
    }

    /***************************************************************************
    * Function Name: UART_INTR_LO_ISR
    ************************************************************************//**
    *
    *   Interrupt Service Routine for the low group of the interrupt sources.
    *
    *
    ***************************************************************************/
    CY_ISR(UART_INTR_LO_ISR)
    {
        UART_IntrHandler(UART_INTR_CAUSE_LO_REG);
    }
#endif /* (CY_PSOC4) */


/* [] END OF FILE */
