/***************************************************************************//**
* \file .h
* \version 3.20
*
* \brief
*  This file provides private function prototypes and constants for the 
*  USBFS component. It is not intended to be used in the user project.
*
********************************************************************************
* \copyright
* Copyright 2013-2016, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_USBFS_UART_pvt_H)
#define CY_USBFS_UART_pvt_H

#include "UART.h"
   
#ifdef UART_ENABLE_AUDIO_CLASS
    #include "UART_audio.h"
#endif /* UART_ENABLE_AUDIO_CLASS */

#ifdef UART_ENABLE_CDC_CLASS
    #include "UART_cdc.h"
#endif /* UART_ENABLE_CDC_CLASS */

#if (UART_ENABLE_MIDI_CLASS)
    #include "UART_midi.h"
#endif /* (UART_ENABLE_MIDI_CLASS) */

#if (UART_ENABLE_MSC_CLASS)
    #include "UART_msc.h"
#endif /* (UART_ENABLE_MSC_CLASS) */

#if (UART_EP_MANAGEMENT_DMA)
    #if (CY_PSOC4)
        #include <CyDMA.h>
    #else
        #include <CyDmac.h>
        #if ((UART_EP_MANAGEMENT_DMA_AUTO) && (UART_EP_DMA_AUTO_OPT == 0u))
            #include "UART_EP_DMA_Done_isr.h"
            #include "UART_EP8_DMA_Done_SR.h"
            #include "UART_EP17_DMA_Done_SR.h"
        #endif /* ((UART_EP_MANAGEMENT_DMA_AUTO) && (UART_EP_DMA_AUTO_OPT == 0u)) */
    #endif /* (CY_PSOC4) */
#endif /* (UART_EP_MANAGEMENT_DMA) */

#if (UART_DMA1_ACTIVE)
    #include "UART_ep1_dma.h"
    #define UART_EP1_DMA_CH     (UART_ep1_dma_CHANNEL)
#endif /* (UART_DMA1_ACTIVE) */

#if (UART_DMA2_ACTIVE)
    #include "UART_ep2_dma.h"
    #define UART_EP2_DMA_CH     (UART_ep2_dma_CHANNEL)
#endif /* (UART_DMA2_ACTIVE) */

#if (UART_DMA3_ACTIVE)
    #include "UART_ep3_dma.h"
    #define UART_EP3_DMA_CH     (UART_ep3_dma_CHANNEL)
#endif /* (UART_DMA3_ACTIVE) */

#if (UART_DMA4_ACTIVE)
    #include "UART_ep4_dma.h"
    #define UART_EP4_DMA_CH     (UART_ep4_dma_CHANNEL)
#endif /* (UART_DMA4_ACTIVE) */

#if (UART_DMA5_ACTIVE)
    #include "UART_ep5_dma.h"
    #define UART_EP5_DMA_CH     (UART_ep5_dma_CHANNEL)
#endif /* (UART_DMA5_ACTIVE) */

#if (UART_DMA6_ACTIVE)
    #include "UART_ep6_dma.h"
    #define UART_EP6_DMA_CH     (UART_ep6_dma_CHANNEL)
#endif /* (UART_DMA6_ACTIVE) */

#if (UART_DMA7_ACTIVE)
    #include "UART_ep7_dma.h"
    #define UART_EP7_DMA_CH     (UART_ep7_dma_CHANNEL)
#endif /* (UART_DMA7_ACTIVE) */

#if (UART_DMA8_ACTIVE)
    #include "UART_ep8_dma.h"
    #define UART_EP8_DMA_CH     (UART_ep8_dma_CHANNEL)
#endif /* (UART_DMA8_ACTIVE) */


/***************************************
*     Private Variables
***************************************/

/* Generated external references for descriptors. */
extern const uint8 CYCODE UART_DEVICE0_DESCR[18u];
extern const uint8 CYCODE UART_DEVICE0_CONFIGURATION0_DESCR[67u];
extern const T_UART_EP_SETTINGS_BLOCK CYCODE UART_DEVICE0_CONFIGURATION0_EP_SETTINGS_TABLE[3u];
extern const uint8 CYCODE UART_DEVICE0_CONFIGURATION0_INTERFACE_CLASS[2u];
extern const T_UART_LUT CYCODE UART_DEVICE0_CONFIGURATION0_TABLE[5u];
extern const T_UART_LUT CYCODE UART_DEVICE0_TABLE[3u];
extern const T_UART_LUT CYCODE UART_TABLE[1u];
extern const uint8 CYCODE UART_SN_STRING_DESCRIPTOR[2];
extern const uint8 CYCODE UART_STRING_DESCRIPTORS[159u];


extern const uint8 CYCODE UART_MSOS_DESCRIPTOR[UART_MSOS_DESCRIPTOR_LENGTH];
extern const uint8 CYCODE UART_MSOS_CONFIGURATION_DESCR[UART_MSOS_CONF_DESCR_LENGTH];
#if defined(UART_ENABLE_IDSN_STRING)
    extern uint8 UART_idSerialNumberStringDescriptor[UART_IDSN_DESCR_LENGTH];
#endif /* (UART_ENABLE_IDSN_STRING) */

extern volatile uint8 UART_interfaceNumber;
extern volatile uint8 UART_interfaceSetting[UART_MAX_INTERFACES_NUMBER];
extern volatile uint8 UART_interfaceSettingLast[UART_MAX_INTERFACES_NUMBER];
extern volatile uint8 UART_deviceAddress;
extern volatile uint8 UART_interfaceStatus[UART_MAX_INTERFACES_NUMBER];
extern const uint8 CYCODE *UART_interfaceClass;

extern volatile T_UART_EP_CTL_BLOCK UART_EP[UART_MAX_EP];
extern volatile T_UART_TD UART_currentTD;

#if (UART_EP_MANAGEMENT_DMA)
    #if (CY_PSOC4)
        extern const uint8 UART_DmaChan[UART_MAX_EP];
    #else
        extern uint8 UART_DmaChan[UART_MAX_EP];
        extern uint8 UART_DmaTd  [UART_MAX_EP];
    #endif /* (CY_PSOC4) */
#endif /* (UART_EP_MANAGEMENT_DMA) */

#if (UART_EP_MANAGEMENT_DMA_AUTO)
#if (CY_PSOC4)
    extern uint8  UART_DmaEpBurstCnt   [UART_MAX_EP];
    extern uint8  UART_DmaEpLastBurstEl[UART_MAX_EP];

    extern uint8  UART_DmaEpBurstCntBackup  [UART_MAX_EP];
    extern uint32 UART_DmaEpBufferAddrBackup[UART_MAX_EP];
    
    extern const uint8 UART_DmaReqOut     [UART_MAX_EP];    
    extern const uint8 UART_DmaBurstEndOut[UART_MAX_EP];
#else
    #if (UART_EP_DMA_AUTO_OPT == 0u)
        extern uint8 UART_DmaNextTd[UART_MAX_EP];
        extern volatile uint16 UART_inLength [UART_MAX_EP];
        extern volatile uint8  UART_inBufFull[UART_MAX_EP];
        extern const uint8 UART_epX_TD_TERMOUT_EN[UART_MAX_EP];
        extern const uint8 *UART_inDataPointer[UART_MAX_EP];
    #endif /* (UART_EP_DMA_AUTO_OPT == 0u) */
#endif /* CY_PSOC4 */
#endif /* (UART_EP_MANAGEMENT_DMA_AUTO) */

extern volatile uint8 UART_ep0Toggle;
extern volatile uint8 UART_lastPacketSize;
extern volatile uint8 UART_ep0Mode;
extern volatile uint8 UART_ep0Count;
extern volatile uint16 UART_transferByteCount;


/***************************************
*     Private Function Prototypes
***************************************/
void  UART_ReInitComponent(void)            ;
void  UART_HandleSetup(void)                ;
void  UART_HandleIN(void)                   ;
void  UART_HandleOUT(void)                  ;
void  UART_LoadEP0(void)                    ;
uint8 UART_InitControlRead(void)            ;
uint8 UART_InitControlWrite(void)           ;
void  UART_ControlReadDataStage(void)       ;
void  UART_ControlReadStatusStage(void)     ;
void  UART_ControlReadPrematureStatus(void) ;
uint8 UART_InitControlWrite(void)           ;
uint8 UART_InitZeroLengthControlTransfer(void) ;
void  UART_ControlWriteDataStage(void)      ;
void  UART_ControlWriteStatusStage(void)    ;
void  UART_ControlWritePrematureStatus(void);
uint8 UART_InitNoDataControlTransfer(void)  ;
void  UART_NoDataControlStatusStage(void)   ;
void  UART_InitializeStatusBlock(void)      ;
void  UART_UpdateStatusBlock(uint8 completionCode) ;
uint8 UART_DispatchClassRqst(void)          ;

void UART_Config(uint8 clearAltSetting) ;
void UART_ConfigAltChanged(void)        ;
void UART_ConfigReg(void)               ;
void UART_EpStateInit(void)             ;


const T_UART_LUT CYCODE *UART_GetConfigTablePtr(uint8 confIndex);
const T_UART_LUT CYCODE *UART_GetDeviceTablePtr(void)           ;
#if (UART_BOS_ENABLE)
    const T_UART_LUT CYCODE *UART_GetBOSPtr(void)               ;
#endif /* (UART_BOS_ENABLE) */
const uint8 CYCODE *UART_GetInterfaceClassTablePtr(void)                    ;
uint8 UART_ClearEndpointHalt(void)                                          ;
uint8 UART_SetEndpointHalt(void)                                            ;
uint8 UART_ValidateAlternateSetting(void)                                   ;

void UART_SaveConfig(void)      ;
void UART_RestoreConfig(void)   ;

#if (CY_PSOC3 || CY_PSOC5LP)
    #if (UART_EP_MANAGEMENT_DMA_AUTO && (UART_EP_DMA_AUTO_OPT == 0u))
        void UART_LoadNextInEP(uint8 epNumber, uint8 mode)  ;
    #endif /* (UART_EP_MANAGEMENT_DMA_AUTO && (UART_EP_DMA_AUTO_OPT == 0u)) */
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

#if defined(UART_ENABLE_IDSN_STRING)
    void UART_ReadDieID(uint8 descr[])  ;
#endif /* UART_ENABLE_IDSN_STRING */

#if defined(UART_ENABLE_HID_CLASS)
    uint8 UART_DispatchHIDClassRqst(void) ;
#endif /* (UART_ENABLE_HID_CLASS) */

#if defined(UART_ENABLE_AUDIO_CLASS)
    uint8 UART_DispatchAUDIOClassRqst(void) ;
#endif /* (UART_ENABLE_AUDIO_CLASS) */

#if defined(UART_ENABLE_CDC_CLASS)
    uint8 UART_DispatchCDCClassRqst(void) ;
#endif /* (UART_ENABLE_CDC_CLASS) */

#if (UART_ENABLE_MSC_CLASS)
    #if (UART_HANDLE_MSC_REQUESTS)
        uint8 UART_DispatchMSCClassRqst(void) ;
    #endif /* (UART_HANDLE_MSC_REQUESTS) */
#endif /* (UART_ENABLE_MSC_CLASS */

CY_ISR_PROTO(UART_EP_0_ISR);
CY_ISR_PROTO(UART_BUS_RESET_ISR);

#if (UART_SOF_ISR_ACTIVE)
    CY_ISR_PROTO(UART_SOF_ISR);
#endif /* (UART_SOF_ISR_ACTIVE) */

#if (UART_EP1_ISR_ACTIVE)
    CY_ISR_PROTO(UART_EP_1_ISR);
#endif /* (UART_EP1_ISR_ACTIVE) */

#if (UART_EP2_ISR_ACTIVE)
    CY_ISR_PROTO(UART_EP_2_ISR);
#endif /* (UART_EP2_ISR_ACTIVE) */

#if (UART_EP3_ISR_ACTIVE)
    CY_ISR_PROTO(UART_EP_3_ISR);
#endif /* (UART_EP3_ISR_ACTIVE) */

#if (UART_EP4_ISR_ACTIVE)
    CY_ISR_PROTO(UART_EP_4_ISR);
#endif /* (UART_EP4_ISR_ACTIVE) */

#if (UART_EP5_ISR_ACTIVE)
    CY_ISR_PROTO(UART_EP_5_ISR);
#endif /* (UART_EP5_ISR_ACTIVE) */

#if (UART_EP6_ISR_ACTIVE)
    CY_ISR_PROTO(UART_EP_6_ISR);
#endif /* (UART_EP6_ISR_ACTIVE) */

#if (UART_EP7_ISR_ACTIVE)
    CY_ISR_PROTO(UART_EP_7_ISR);
#endif /* (UART_EP7_ISR_ACTIVE) */

#if (UART_EP8_ISR_ACTIVE)
    CY_ISR_PROTO(UART_EP_8_ISR);
#endif /* (UART_EP8_ISR_ACTIVE) */

#if (UART_EP_MANAGEMENT_DMA)
    CY_ISR_PROTO(UART_ARB_ISR);
#endif /* (UART_EP_MANAGEMENT_DMA) */

#if (UART_DP_ISR_ACTIVE)
    CY_ISR_PROTO(UART_DP_ISR);
#endif /* (UART_DP_ISR_ACTIVE) */

#if (CY_PSOC4)
    CY_ISR_PROTO(UART_INTR_HI_ISR);
    CY_ISR_PROTO(UART_INTR_MED_ISR);
    CY_ISR_PROTO(UART_INTR_LO_ISR);
    #if (UART_LPM_ACTIVE)
        CY_ISR_PROTO(UART_LPM_ISR);
    #endif /* (UART_LPM_ACTIVE) */
#endif /* (CY_PSOC4) */

#if (UART_EP_MANAGEMENT_DMA_AUTO)
#if (CY_PSOC4)
    #if (UART_DMA1_ACTIVE)
        void UART_EP1_DMA_DONE_ISR(void);
    #endif /* (UART_DMA1_ACTIVE) */

    #if (UART_DMA2_ACTIVE)
        void UART_EP2_DMA_DONE_ISR(void);
    #endif /* (UART_DMA2_ACTIVE) */

    #if (UART_DMA3_ACTIVE)
        void UART_EP3_DMA_DONE_ISR(void);
    #endif /* (UART_DMA3_ACTIVE) */

    #if (UART_DMA4_ACTIVE)
        void UART_EP4_DMA_DONE_ISR(void);
    #endif /* (UART_DMA4_ACTIVE) */

    #if (UART_DMA5_ACTIVE)
        void UART_EP5_DMA_DONE_ISR(void);
    #endif /* (UART_DMA5_ACTIVE) */

    #if (UART_DMA6_ACTIVE)
        void UART_EP6_DMA_DONE_ISR(void);
    #endif /* (UART_DMA6_ACTIVE) */

    #if (UART_DMA7_ACTIVE)
        void UART_EP7_DMA_DONE_ISR(void);
    #endif /* (UART_DMA7_ACTIVE) */

    #if (UART_DMA8_ACTIVE)
        void UART_EP8_DMA_DONE_ISR(void);
    #endif /* (UART_DMA8_ACTIVE) */

#else
    #if (UART_EP_DMA_AUTO_OPT == 0u)
        CY_ISR_PROTO(UART_EP_DMA_DONE_ISR);
    #endif /* (UART_EP_DMA_AUTO_OPT == 0u) */
#endif /* (CY_PSOC4) */
#endif /* (UART_EP_MANAGEMENT_DMA_AUTO) */


/***************************************
*         Request Handlers
***************************************/

uint8 UART_HandleStandardRqst(void) ;
uint8 UART_DispatchClassRqst(void)  ;
uint8 UART_HandleVendorRqst(void)   ;


/***************************************
*    HID Internal references
***************************************/

#if defined(UART_ENABLE_HID_CLASS)
    void UART_FindReport(void)            ;
    void UART_FindReportDescriptor(void)  ;
    void UART_FindHidClassDecriptor(void) ;
#endif /* UART_ENABLE_HID_CLASS */


/***************************************
*    MIDI Internal references
***************************************/

#if defined(UART_ENABLE_MIDI_STREAMING)
    void UART_MIDI_IN_EP_Service(void)  ;
#endif /* (UART_ENABLE_MIDI_STREAMING) */


/***************************************
*    CDC Internal references
***************************************/

#if defined(UART_ENABLE_CDC_CLASS)

    typedef struct
    {
        uint8  bRequestType;
        uint8  bNotification;
        uint8  wValue;
        uint8  wValueMSB;
        uint8  wIndex;
        uint8  wIndexMSB;
        uint8  wLength;
        uint8  wLengthMSB;
        uint8  wSerialState;
        uint8  wSerialStateMSB;
    } t_UART_cdc_notification;

    uint8 UART_GetInterfaceComPort(uint8 interface) ;
    uint8 UART_Cdc_EpInit( const T_UART_EP_SETTINGS_BLOCK CYCODE *pEP, uint8 epNum, uint8 cdcComNums) ;

    extern volatile uint8  UART_cdc_dataInEpList[UART_MAX_MULTI_COM_NUM];
    extern volatile uint8  UART_cdc_dataOutEpList[UART_MAX_MULTI_COM_NUM];
    extern volatile uint8  UART_cdc_commInEpList[UART_MAX_MULTI_COM_NUM];
#endif /* (UART_ENABLE_CDC_CLASS) */


#endif /* CY_USBFS_UART_pvt_H */


/* [] END OF FILE */
