














































































































































































































































































































//SORT
#include <stdio.h>
void main (){
   int num[20];
   int i, j, a, n;
   printf("enter number of elements in an array
");
   scanf("%d", &n);
   printf("Enter the elements
");
   for (i = 0; i < n; ++i)
      scanf("%d", &num[i]);
   for (i = 0; i < n; ++i){
      for (j = i + 1; j < n; ++j){
         if (num[i] > num[j]){
            a = num[i];
            num[i] = num[j];
            num[j] = a;
         }
      }
   }
   printf("The numbers in ascending order is:
");
   for (i = 0; i < n; ++i){
      printf("%d
", num[i]);
   }
}



//FACTORIAL 
#include <stdio.h>
int main() {
    int n, i;
    unsigned long long fact = 1;
    printf("Enter an integer: ");
    scanf("%d", &n);

    // shows error if the user enters a negative integer
    if (n < 0)
        printf("Error! Factorial of a negative number doesn't exist.");
    else {
        for (i = 1; i <= n; ++i) {
            fact *= i;
        }
        printf("Factorial of %d = %llu", n, fact);
    }

    return 0;
}







//MAX OF 5 NUMBERS:
#include <stdio.h>

int main()
{
    int a,b,c,d,e;
  
    printf("Enter 1st number: ");
    scanf("%d",&a);

    printf("Enter 2nd number: ");
    scanf("%d",&b);
  
    printf("Enter 3th number: ");
    scanf("%d",&c);
  
    printf("Enter 4th number: ");
    scanf("%d",&d);

    printf("Enter 5th number: ");
    scanf("%d",&e);
  
    int max = a;
  
    if (b > max) max = b;
    if (c > max) max = c;
    if (d > max) max = d;
    if (e > max) max = e;

    printf("\nMax is %d", max);
  
    return 0;
}





//ADC LED LCD ALL INCLUDED
#include "project.h"
#include <stdio.h>
#include<stdlib.h>
#include <math.h>
#include<time.h>
#include <stdbool.h>
#define LED_ON 1u
#define LED_OFF 0u
int main(void)
{
    CyGlobalIntEnable;
    unsigned int p1 = 0;
    unsigned int p2 = 0;
    unsigned int total_score = 50;
    unsigned int current_max_score = 0;
    unsigned int t1 = 0;
    unsigned int t2 = 0;
    unsigned int i;

    srand(time(0));
    int prime[400 + 1];
    bool game_continue = true;
    for (int i = 1; i <= 400; i++)
    {
        prime[i] = i;
    }

    for (int i = 2; i * i <= 400; i++)
    {
        if (prime[i] != -1)
        {

            for (int j = 2 * i; j <= 400; j += i)
                prime[j] = -1;
        }
    }
    ADC_Start();
    ADC_StartConvert();

    LCD_Start();
    LCD_Position(0, 0);
    while (current_max_score != total_score)
    {
        game_continue = true;

        LCD_Position(0, 4);
        LCD_PrintString("Welcome");
        LCD_Position(1, 2);
        LCD_PrintString("P1: ");
        LCD_PrintNumber(p1);
        LCD_PutChar(' ');

        LCD_PrintString("P2: ");
        LCD_PrintNumber(p2);

        CyDelay(1000);

        LCD_ClearDisplay();
        while (game_continue)
        {
            //LCD_Position(0, 4);
            CyDelay(100);
            LCD_ClearDisplay();
            LCD_PrintString("TURN: P1 ");

            CyDelay(2000);

            for (i = 0; i < 2000; i++)
            {

                uint32 j;
                for (j = 0; j < 6000; j++)
                {

                    
                }
            }
            ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
            uint16 adc_reading = ADC_GetResult16();

            t1 = ((int)adc_reading / 255.0) * 100.0;
            //while(t1==0 || t1==96) t1=(rand()%96)+1;
            if(t1==0 || t1==96|| t1==100) t1=(rand()%96)+1;
            LCD_ClearDisplay();
            LCD_Position(0, 4);
            LCD_PrintNumber(t1);
            CyDelay(1000);
            LCD_ClearDisplay();

            if (prime[t1] != -1)
            {

                CyDelay(1000);

                LCD_ClearDisplay();
                LCD_Position(0, 0);
                LCD_PrintString("MASTER MOVE: P1!!");
                Pin_2_Write(LED_ON);
                CyDelay(6000);
                Pin_2_Write(LED_OFF);
               
                p1 += 20;
                LCD_PrintString("P1: ");
                LCD_PrintNumber(p1);

                CyDelay(1000);
                LCD_ClearDisplay();
            }
            else
            {
                LCD_ClearDisplay();
                LCD_Position(0, 4);
                LCD_PrintString("P1: ");
                p1 += 7;
                LCD_PrintNumber(p1);
            }

            if (current_max_score < p1)
            {
                current_max_score = p1;
            }
            
            
            // TODO: PWM LIGHT
            //TODO:
            Pin_2_Write(LED_ON);
                CyDelay(1000);
            Pin_2_Write(LED_OFF);
            
            CyDelay(1000);
            if (current_max_score < total_score)
            {
                LCD_ClearDisplay();
                CyDelay(1000);
                LCD_PrintString("TURN: P2 ");

                CyDelay(2000);

                for (i = 0; i < 2000; i++)
                {

                    uint32 j;
                    for (j = 0; j < 6000; j++)
                    {

                       
                    }
                }
                ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
                uint16 adc_reading = ADC_GetResult16();

                t2 = ((int)adc_reading / 255.0) * 100.0;
                //while(t2==0 || t2==96) t2=(rand()%96)+1;
                if(t2==0 || t2==96|| t2==100) t2=(rand()%96)+1;
                LCD_ClearDisplay();
                LCD_Position(0, 4);
                LCD_PrintNumber(t2);
                CyDelay(1000);
                LCD_ClearDisplay();

                if (prime[t2] != -1)
                {

                    CyDelay(1000);

                    LCD_ClearDisplay();
                    LCD_Position(0, 0);
                    LCD_PrintString("MASTER MOVE: P2!!");
                    Pin_2_Write(LED_ON);
                CyDelay(6000);
                Pin_2_Write(LED_OFF);
                
                LCD_Position(1, 4);
                    p2 += 20;
                    LCD_PrintString("P2: ");
                    LCD_PrintNumber(p2);

                    CyDelay(1000);
                    LCD_ClearDisplay();
                }
                else
                {
                    LCD_ClearDisplay();
                    LCD_Position(0, 4);
                    LCD_PrintString("P2: ");
                    p2 += 7;
                    LCD_PrintNumber(p2);
                }

                if (current_max_score < p2)
                    current_max_score = p2;

                CyDelay(1000);

                // TODO: PWM LED
                //TODO:   
                
                
                // game end
                Pin_2_Write(LED_ON);
                CyDelay(1000);
                Pin_2_Write(LED_OFF);
            }

            if (current_max_score >= total_score)
            {
                if (p1 > p2)
                {

                    LCD_ClearDisplay();
                    LCD_Position(1, 4);
                    LCD_PrintString("Winner: P1 :))");
                    CyDelay(5000);
                    LCD_ClearDisplay();
                }
                else
                {

                    LCD_ClearDisplay();
                    LCD_Position(1, 4);
                    LCD_PrintString("Winner: P2 :))");
                    CyDelay(5000);
                    LCD_ClearDisplay();
                }

                p1 = 0;
                p2 = 0;
                current_max_score = 0;
                game_continue = false;
                break;
            }
            else
            {
                continue;
            }
        }
        if (!game_continue)
        {
            continue;
        }
    }
}

/* [] END OF FILE */