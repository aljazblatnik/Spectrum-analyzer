#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f0xx.h"
#include "system_stm32f0xx.h"

#include "pinout.h"
#include "delay.h"
#include "usart.h"

#include "attenuator.h"


// primitives
void init(void);

// USART - spremenljivke za serijsko komunikacijo
char buffer[20]; //max 20 characters
int buff_index = 0;
int decode_command = 0;

int main(void)
{
    init();
    timer14_init();
    timer16_init();

    usart_init();
    attenuator_init();

    usart_send_string("Attenuator set v1.0");

    while(1){
        if(decode_command){
            if(command_decode(buffer)){
            }
            decode_command = 0;
        }
        attenuator_send(0b00101000);
        delay_ms(10);
    }
}


void init(void){
    RCC->CR |= RCC_CR_HSEON; // Turn HSE oscillator on
    while(!(RCC->CR & RCC_CR_HSERDY)); // Pocakaj da je HSE on
    // TO-DO preveri ce je zunanji oscilator zagnan, drugace delaj na notranjega!
    // nastavimo na hitrost 48 MHz
    RCC->CFGR &= ~RCC_CFGR_SW; // zagotovimo da je ura na notranjem oscilatorju
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI); // pocakamo da je preklop uspesen
    RCC->CR &= ~RCC_CR_PLLON; // onemogocimo PLL
    while((RCC->CR & RCC_CR_PLLRDY) != 0); // pocakajmo da je res izklopljen
    RCC->CFGR = (RCC->CFGR & (~RCC_CFGR_PLLMUL)) | RCC_CFGR_PLLMUL12; // PLL HSE/6 x 12
    RCC->CFGR2 = RCC_CFGR2_PREDIV_DIV6; // Delimo HSE s 6 da dobimo 4 MHz notranjo uro, enako kot HSI
    RCC->CFGR |= RCC_CFGR_PLLSRC; // HSE to PLL input
    RCC->CR |= RCC_CR_PLLON; // prizgemo PLL
    while((RCC->CR & RCC_CR_PLLRDY) == 0); // pocakajmo da se PLL zazene
    RCC->CFGR |= RCC_CFGR_SW_PLL; // PLL kot ura - ne cakamo da je stabilna!
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL); // pocakajmo dokler preklop ni uspesen

    RCC->AHBENR |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN);  // prizgemo uro za GPIO
    GPIOA->OSPEEDR |= 0xC3FFFFFF; // full speed (leave PA13 and PA14 as is)
    GPIOB->OSPEEDR |= 0xFFFFFFFF; // full speed

    // ATTEN - F1958
    GPIOA->MODER |= (GPIO_MODER_MODER0_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER2_0); //General purpose output mode

    // RS-232
    //GPIOA->MODER |= (GPIO_MODER_MODER2_1 | GPIO_MODER_MODER3_1); // pin setup PA2 - TX | PA3 - RX - alternate function mode
    //GPIOA->AFR[0] |= 0x00001100; // alternate function 1 on pin PA2 and PA3
}

/******************************************************************************/
/*            Cortex-M0 Processor Exceptions Handlers                         */
/******************************************************************************/

void NMI_Handler(void);
void HardFault_Handler(void){
    while (1);
}
void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

void USART1_IRQHandler(void){
    if(USART1->ISR & USART_ISR_RXNE){ // new data in
        unsigned char data = USART1->RDR;
        if((data == 8 || data == 127) && (buff_index > 0)){
            buff_index--;
            USART1->TDR = data;
            return;
        }
        else if((data == 8 || data == 127) && buff_index<=0){
            return;
        }
        if(data>=32 || data == 13){
            USART1->TDR = data; // echo
            buffer[buff_index] = data;
            if(data == 13){
                buffer[buff_index] = 0;
                decode_command = 1;
            }
            else{
                buff_index++;
            }
        }
    }
    else if(USART1->ISR & USART_ISR_ORE){ // overrun error
        USART1->ICR |= USART_ICR_ORECF; // clear overrun error
        // do nothing
    }
}
