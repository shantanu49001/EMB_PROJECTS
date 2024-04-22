// psoc
//     main.c
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
#include "stdio.h"

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Start USBUART */
    USBUART_Start(0, USBUART_5V_OPERATION);

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */

    for (;;)
    {
        CyDelay(100);
        /* Send 'a' character continuously */
        while (USBUART_GetConfiguration() == 0)
            ;
        if (Pin_1_Read() == 0)
        {
            //            USBUART_PutString("left\r\n");
            USBUART_PutString("right\n");

            //            USBUART_PutChar('l');
            //            USBUART_PutChar('r');
        }
        if (Pin_2_Read() == 0)
        {
            USBUART_PutString("left\n");
            //            USBUART_PutChar('l');
        }

        /* Place your application code here. */
    }
}

/* [] END OF FILE */