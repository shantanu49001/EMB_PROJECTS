/***************************************************************************//**
* \file UART_cdc.h
* \version 3.20
*
* \brief
*  This file provides function prototypes and constants for the USBFS component 
*  CDC class.
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

#if !defined(CY_USBFS_UART_cdc_H)
#define CY_USBFS_UART_cdc_H

#include "UART.h"


/*******************************************************************************
* Prototypes of the UART_cdc API.
*******************************************************************************/
/**
* \addtogroup group_cdc
* @{
*/
#if (UART_ENABLE_CDC_CLASS_API != 0u)
    uint8 UART_CDC_Init(void)            ;
    void UART_PutData(const uint8* pData, uint16 length) ;
    void UART_PutString(const char8 string[])            ;
    void UART_PutChar(char8 txDataByte) ;
    void UART_PutCRLF(void)             ;
    uint16 UART_GetCount(void)          ;
    uint8  UART_CDCIsReady(void)        ;
    uint8  UART_DataIsReady(void)       ;
    uint16 UART_GetData(uint8* pData, uint16 length)     ;
    uint16 UART_GetAll(uint8* pData)    ;
    uint8  UART_GetChar(void)           ;
    uint8  UART_IsLineChanged(void)     ;
    uint32 UART_GetDTERate(void)        ;
    uint8  UART_GetCharFormat(void)     ;
    uint8  UART_GetParityType(void)     ;
    uint8  UART_GetDataBits(void)       ;
    uint16 UART_GetLineControl(void)    ;
    void UART_SendSerialState (uint16 serialState) ;
    uint16 UART_GetSerialState (void)   ;
    void UART_SetComPort (uint8 comNumber) ;
    uint8 UART_GetComPort (void)        ;
    uint8 UART_NotificationIsReady(void) ;

#endif  /* (UART_ENABLE_CDC_CLASS_API) */
/** @} cdc */

/*******************************************************************************
*  Constants for UART_cdc API.
*******************************************************************************/

/* CDC Class-Specific Request Codes (CDC ver 1.2 Table 19) */
#define UART_CDC_SET_LINE_CODING        (0x20u)
#define UART_CDC_GET_LINE_CODING        (0x21u)
#define UART_CDC_SET_CONTROL_LINE_STATE (0x22u)

/*PSTN Subclass Specific Notifications (CDC ver 1.2 Table 30)*/
#define UART_SERIAL_STATE               (0x20u)

#define UART_LINE_CODING_CHANGED        (0x01u)
#define UART_LINE_CONTROL_CHANGED       (0x02u)

#define UART_1_STOPBIT                  (0x00u)
#define UART_1_5_STOPBITS               (0x01u)
#define UART_2_STOPBITS                 (0x02u)

#define UART_PARITY_NONE                (0x00u)
#define UART_PARITY_ODD                 (0x01u)
#define UART_PARITY_EVEN                (0x02u)
#define UART_PARITY_MARK                (0x03u)
#define UART_PARITY_SPACE               (0x04u)

#define UART_LINE_CODING_SIZE           (0x07u)
#define UART_LINE_CODING_RATE           (0x00u)
#define UART_LINE_CODING_STOP_BITS      (0x04u)
#define UART_LINE_CODING_PARITY         (0x05u)
#define UART_LINE_CODING_DATA_BITS      (0x06u)

#define UART_LINE_CONTROL_DTR           (0x01u)
#define UART_LINE_CONTROL_RTS           (0x02u)

#define UART_MAX_MULTI_COM_NUM          (2u) 

#define UART_COM_PORT1                  (0u) 
#define UART_COM_PORT2                  (1u) 

#define UART_SUCCESS                    (0u)
#define UART_FAILURE                    (1u)

#define UART_SERIAL_STATE_SIZE          (10u)

/* SerialState constants*/
#define UART_SERIAL_STATE_REQUEST_TYPE  (0xA1u)
#define UART_SERIAL_STATE_LENGTH        (0x2u)

/*******************************************************************************
* External data references
*******************************************************************************/
/**
* \addtogroup group_cdc
* @{
*/
extern volatile uint8  UART_linesCoding[UART_MAX_MULTI_COM_NUM][UART_LINE_CODING_SIZE];
extern volatile uint8  UART_linesChanged[UART_MAX_MULTI_COM_NUM];
extern volatile uint16 UART_linesControlBitmap[UART_MAX_MULTI_COM_NUM];
extern volatile uint16 UART_serialStateBitmap[UART_MAX_MULTI_COM_NUM];
extern volatile uint8  UART_cdcDataInEp[UART_MAX_MULTI_COM_NUM];
extern volatile uint8  UART_cdcDataOutEp[UART_MAX_MULTI_COM_NUM];
extern volatile uint8  UART_cdcCommInInterruptEp[UART_MAX_MULTI_COM_NUM];
/** @} cdc */

/*******************************************************************************
* The following code is DEPRECATED and
* must not be used.
*******************************************************************************/


#define UART_lineCoding             UART_linesCoding[0]
#define UART_lineChanged            UART_linesChanged[0]
#define UART_lineControlBitmap      UART_linesControlBitmap[0]
#define UART_cdc_data_in_ep         UART_cdcDataInEp[0]
#define UART_cdc_data_out_ep        UART_cdcDataOutEp[0]

#endif /* (CY_USBFS_UART_cdc_H) */


/* [] END OF FILE */
