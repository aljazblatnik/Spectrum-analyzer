#include "main.h"
#include "stm32g4xx.h"
#include "stm32g441xx.h"
#include "stdint.h"
#include "pinout.h"
#include "usart.h"
#include "delay.h"
#include "max2870.h"

// USART - variables for serial communication
char buffer[20]; //max 20 characters
int buff_index = 0;
int decode_command = 0;

// 120 MHz main CPU freq.
void init();

int main(void){
    init();
    init_delay();
    init_usart();

    // System startup
    LED_ON;

    // Start test procedure
    usart_send_string("LO2 test program\r\n");
    usart_send_string("A.Blatnik 25/02/2024\r\n");

    usart_send_string("\r\nTesting - START\r\n");
    usart_send_string("LO2-SA:PLL loop - ");
    MAX2871_init();
    delay_ms(100);
    if(MUX) {
        usart_send_string("LOCKED - OK\n\r");
    } else {
        usart_send_string("UNLOCKED\n\r");
        usart_send_string("ERROR:LO2 PLL UNLOCKED\n\r");
    }
    read_MAX2871();

    usart_send_string("\r\nTesting - END\r\n");
    usart_send_string("\r\nNormal command prompt mode (no remote echo)\r\n");
    usart_send_string("USAGE:\r\nF xxxxxxx - frequency [kHz]\r\n\r\n");
    while(1){
        if(decode_command){
            command_decode(buffer);
        }
    }
}

void init()
{
    // Power set to Range 1
    RCC->APB1ENR1 |= RCC_APB1ENR1_PWREN; // enable clock
    PWR->CR1 = PWR_CR1_VOS_0; // Range 1
    while((PWR->SR2 & PWR_SR2_VOSF) != 0); // Wait for range switch
    FLASH->ACR |= FLASH_ACR_LATENCY_3WS; // (3WS - 4 CPU cycles) - default = 1
    PWR->CR5 |= PWR_CR5_R1MODE; // Normal MODE 1
    // Configure PLL to 120 MHz - 10 MHz crystal oscillator
    RCC->CR |= RCC_CR_HSEON; // turn on HSE
    while(!(RCC->CR & RCC_CR_HSERDY)); // Wait to be ready
    RCC->CR &= ~RCC_CR_PLLON; // turn PLL off
    while((RCC->CR & RCC_CR_PLLRDY) != 0); // wait for PLL to become off
    RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE | RCC_PLLCFGR_PLLN_3 | RCC_PLLCFGR_PLLN_4 | RCC_PLLCFGR_PLLPEN | RCC_PLLCFGR_PLLREN | RCC_PLLCFGR_PLLPDIV_1 | RCC_PLLCFGR_PLLPDIV_2; // M = 1; N = 24; P = 6 on; Q off; R = 2 on; HSE input clock
    RCC->CR |= RCC_CR_PLLON; // turn PLL ON
    while((RCC->CR & RCC_CR_PLLRDY) == 0); // Wait for PLL clock to start
    RCC->CICR |= RCC_CICR_PLLRDYC; // clear PLLRDY Flag
    RCC->CFGR |= RCC_CFGR_SW_PLL;
    while(!(RCC->CFGR & RCC_CFGR_SWS_PLL)); // Wait for clock switch to be done
    // GPIO power enable
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN | RCC_AHB2ENR_GPIOBEN | RCC_AHB2ENR_GPIOCEN;

    // PORTS SPEED
    GPIOA->OSPEEDR = 0xCFFFFFFF; // PA13 full speed, PA14 low speed - default
    GPIOB->OSPEEDR = 0xFFFFFFFF; // all B ports full speed
    GPIOC->OSPEEDR = 0x00000000; // Port C low speed

    GPIOA->PUPDR &= ~GPIO_PUPDR_PUPD15; // no pullup or down on PA15
    GPIOB->PUPDR = 0x00000000; //No pull-up/pull-down on all ports
    GPIOA->MODER = 0x2BFFFFFF; // PA13 and PA14 for SWD on AF0
    GPIOB->MODER = 0xFFFFFFFF; // Turn off default PB3 AF function
    GPIOB->AFR[0] = 0x00000000; // All AF0 as default state

    // MCU-LED
    GPIOA->MODER &= ~GPIO_MODER_MODE15; // clear default state
    GPIOA->MODER |= GPIO_MODER_MODE15_0; // output

    // RS-232
    GPIOB->MODER &= ~(GPIO_MODER_MODE6_0 | GPIO_MODER_MODE7_0);  // pin setup PB6 - TX | PB7 - RX - alternate function mode
    GPIOB->AFR[0] |= 0x77000000; // alternate function 7 on pin PB6 and PB7

    // PLL2
    GPIOA->MODER &= ~GPIO_MODER_MODE7; // MISO
    GPIOB->MODER &= ~(GPIO_MODER_MODE0_1 | GPIO_MODER_MODE1_1 | GPIO_MODER_MODE2_1); // CLK, MOSI, LE
    GPIOB->MODER &= ~GPIO_MODER_MODE10_1; // RF out EN
}


/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/

void NMI_Handler(void){
    while (1);
}

void HardFault_Handler(void){
    while (1);
}

void BusFault_Handler(void){
    while (1);
}

void UsageFault_Handler(void){
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

void SPI1_IRQHandler(void);

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
            //USART1->TDR = data; // echo
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
    else if(USART1->ISR & USART_ISR_ORE){ // overrun error - do nothing
        USART1->ICR |= USART_ICR_ORECF; // clear overrun error
    }
}

