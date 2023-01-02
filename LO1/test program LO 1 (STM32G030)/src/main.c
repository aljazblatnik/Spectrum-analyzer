#include "stm32g0xx.h"
#include "stm32g0b0xx.h" // used for auto complete function of editor only (included in "stm32g0xx.h"

#include "pinout.h"
#include "usart.h"
#include "delay.h"
#include "stuw81300.h"
#include "lmx2491.h"

/***** GLOBAL VARIABLES *****/
char buffer[usart_buffer]; // defined in .h file
int buff_index;
int decode_command;
/*****  END VARIABLES   *****/

void init();

int main(void)
{
    // startup values
    buff_index = 0;
    decode_command = 0;

    init();
    init_delay();
    init_usart();


    delay_ms(100); // Delay for LDO to stabilize
    init_LMX2491();
    delay_ms(10); // wait for Ref PLL to lock
    init_STUW81300();

    usart_send_string("\rMCU ON. System ready.\r\nARM > ");
    delay_ms(10);

    startup_STUW81300();
    HW_PD_LOW;
    PDF_RF1_LOW;
    PDF_RF2_LOW;
    unsigned char freq = 0b0110000;
    while(1){
            /*
        if(decode_command){
            command_decode(buffer);
            decode_command = 0;
        }
        change_freq_STUW81300(freq);
        delay_ms(1000);
        freq++;
        if(freq>0b1011111){
            freq = 0b0110000;
        }
        */
    }
}

void init()
{
    // 48 MHz internal clock
    RCC->CR &= ~RCC_CR_PLLON; // turn PLL off
    while((RCC->CR & RCC_CR_PLLRDY) != 0); // wait for PLL to become off
    RCC->PLLCFGR = 0b00110010000000000000110000010010; // PLLR /2, N *12, M /2, HSI16 in, PLLR_en
    RCC->CR |= RCC_CR_PLLON; // turn PLL ON
    while((RCC->CR & RCC_CR_PLLRDY) == 0); // Wait for PLL clock to start
    RCC->CICR |= RCC_CICR_PLLRDYC; // clear PLLRDY Flag
    RCC->CFGR |= RCC_CFGR_SW_1; // PLL as System clock
    while(!(RCC->CFGR & RCC_CFGR_SWS_1)); // Wait for clock switch to be done

    // GPIO power enable
    RCC->IOPENR |= RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOAEN; // GPIO Clocks on both ports

    // PORTS SPEED
    GPIOB->OSPEEDR = 0xFFFFFFFF; // all B ports full speed
    GPIOA->OSPEEDR = 0x0C000FFC; // PA9 and PA10 full speed

    // PLL - STuW81300
    GPIOA->MODER &= ~GPIO_MODER_MODE3; // input
    GPIOA->MODER &= ~(GPIO_MODER_MODE4_1 | GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1); // output

    // PLL - LMX2481
    GPIOB->MODER &= ~(GPIO_MODER_MODE3_1 | GPIO_MODER_MODE5_1 | GPIO_MODER_MODE6_1 | GPIO_MODER_MODE7_1 | GPIO_MODER_MODE8_1); // outputs
    GPIOB->MODER &= ~GPIO_MODER_MODE4; // input

    // RS-232
    GPIOA->MODER &= ~(GPIO_MODER_MODE9_0 | GPIO_MODER_MODE10_0);  // pin setup PA9 - TX | PA10 - RX - alternate function mode
    GPIOA->AFR[1] = 0x00000110; // alternate function 1 on pin PA9 and PA10
}


/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers          */
/******************************************************************************/

void NMI_Handler(void)
{
    while (1);
}

void HardFault_Handler(void)
{
    while (1);
}

void SVC_Handler(void);
void PendSV_Handler(void);
void SysTick_Handler(void);

/******************************************************************************/
/* STM32G0xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32g0xx.s).                    */
/******************************************************************************/

void USART1_IRQHandler(void){
    if(USART1->ISR & USART_ISR_RXNE_RXFNE){ // new data in
        unsigned char data = USART1->RDR;
        if(data == 9){
            usart_send_string("\r\nTAB unsupported!");
            data = 13; // force new line
        }
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
