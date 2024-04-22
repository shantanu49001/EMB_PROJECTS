/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/


    
#include "project.h"
#include <stdio.h>
#include <string.h>
// Define LED states
#define LED_ON  (1u)
#define LED_OFF (0u)
#define DELAY_TIME_MS 2000 



#define PASSWORD_LENGTH 5
#define USBUART_BUFFER_SIZE (64u)
unsigned char setPassword[PASSWORD_LENGTH + 1] = "1234#"; // Set password
unsigned char Password[PASSWORD_LENGTH + 1] = "*****"; // Set password

unsigned char receivedPassword[PASSWORD_LENGTH + 1] = {0}; // Received password

void processReceivedData()
{
    // Process received data here
    // For example, check if received password matches the set password
    if (strcmp(receivedPassword, setPassword) == 0)
    {
        // Password matches, display it on the LCD
        LCD_ClearDisplay();
        LCD_Position(0, 0);
        LCD_PrintString("Password Match:");
        Pin_1_Write(LED_ON);
        Pin_2_Write(LED_ON);
        LCD_Position(1, 0);
        LCD_PrintString(Password);
        
         CyDelay(DELAY_TIME_MS);
    
    // Show lock is open
    LCD_ClearDisplay();
    LCD_Position(0, 0);
    LCD_PrintString("Lock is open     ");
    LCD_Position(1, 0);
    LCD_PrintString("Access granted");
    }
    else
    {
        // Password does not match, display an error message on the LCD
        LCD_ClearDisplay();
        LCD_Position(0, 0);
        LCD_PrintString("Invalid Password");
         Pin_1_Write(LED_ON);
         Pin_2_Write(LED_OFF);
        
        
         CyDelay(DELAY_TIME_MS);
    
    // Show lock is open
    LCD_ClearDisplay();
    LCD_Position(0, 0);
    LCD_PrintString("Access denied");
    
    CyDelay(DELAY_TIME_MS);
    LCD_ClearDisplay();
     LCD_Position(0, 0);
     LCD_PrintString("Enter Correct   ");
    LCD_Position(1,0);
    LCD_PrintString("Password                 ");
    
    }
 }


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    USBUART_Start(0, USBUART_3V_OPERATION); /* Start USBUART operation */
    LCD_Start(); // Start LCD
    
    uint16 count ;
    
    uint8 buffer[USBUART_BUFFER_SIZE];
    int i;
    for (;;)
    {
        
       
        
        if(0u != USBUART_IsConfigurationChanged())
        {
            if(0u != USBUART_GetConfiguration())
            {
                USBUART_CDC_Init();
            }
        }
        
        if(0u != USBUART_GetConfiguration())
        {
            if(0u != USBUART_DataIsReady())
            {
                
                
               
                
                
                char rcv = USBUART_GetChar();
                receivedPassword[i++]=rcv;
                 LCD_PutChar(rcv);
                
                 if (rcv =='#') // Check if end of password
            {
                receivedPassword[i] = '\0'; // Null-terminate the string
                processReceivedData(); // Process received data
                i = 0; // Reset index for next password
            }
                
               // processReceivedData();
                
                
                
                if(0u != rcv)
                {
                    
                   
                    
                    while(0u == USBUART_CDCIsReady())
                    {
                    }
                    
                    USBUART_PutChar(rcv);
                    
                    if(USBUART_BUFFER_SIZE == count)
                    {
                        while(0u == USBUART_CDCIsReady())
                        {
                        }
                        
                        USBUART_PutData(NULL, 0u);
                    }
                }
            }
        }
        
        
    
    }
    
    
    
   
}


    






/* [] END OF FILE */
