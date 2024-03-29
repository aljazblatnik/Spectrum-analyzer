#ifndef PINOUT_H_INCLUDED
#define PINOUT_H_INCLUDED

// MAIN System
#define LED_ON GPIOA->BSRR|=GPIO_BSRR_BS15     //PA15
#define LED_OFF GPIOA->BSRR|=GPIO_BSRR_BR15    //PA15

// UART
/*
PB6 - TX
PB7 - RX
*/

// SPI Comm
/*
PB3 - SPI_CLK
PB5 - SPI_MOSI
*/
#define DATA_RDY_ON GPIOB->BSRR|=GPIO_BSRR_BS4 //PB4
#define DATA_RDY_OFF GPIOB->BSRR|=GPIO_BSRR_BR4 //PB4

// PLL - MAX2871
#define CLK_HIGH GPIOB->BSRR|=GPIO_BSRR_BS_0  //PB0
#define CLK_LOW GPIOB->BSRR|=GPIO_BSRR_BR_0   //PB0
#define DATA_HIGH GPIOB->BSRR|=GPIO_BSRR_BS_1 //PB1
#define DATA_LOW GPIOB->BSRR|=GPIO_BSRR_BR_1  //PB1
#define LE_HIGH GPIOB->BSRR|=GPIO_BSRR_BS_2   //PB2
#define LE_LOW GPIOB->BSRR|=GPIO_BSRR_BR_2    //PB2
#define RFEN_HIGH GPIOB->BSRR|=GPIO_BSRR_BS_10 //PB10
#define RFEN_LOW GPIOB->BSRR|=GPIO_BSRR_BR_10  //PB10
#define MUX (GPIOA->IDR & GPIO_IDR_ID7) // PA7

#endif /* PINOUT_H_INCLUDED */
