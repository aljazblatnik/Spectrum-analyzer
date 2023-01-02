#ifndef PINOUT_H_INCLUDED
#define PINOUT_H_INCLUDED

// PLL - STuW81300
#define CLK_HIGH GPIOA->BSRR|=GPIO_BSRR_BS5  //PA5
#define CLK_LOW GPIOA->BSRR|=GPIO_BSRR_BR5   //PA5
#define DATA_HIGH GPIOA->BSRR|=GPIO_BSRR_BS4 //PA4
#define DATA_LOW GPIOA->BSRR|=GPIO_BSRR_BR4  //PA4
#define LE_HIGH GPIOA->BSRR|=GPIO_BSRR_BS6   //PA6
#define LE_LOW GPIOA->BSRR|=GPIO_BSRR_BR6    //PA6
#define MUX (GPIOA->IDR & GPIO_IDR_ID3)      //PA3

#define HW_PD_HIGH GPIOB->BSRR|=GPIO_BSRR_BS1     //PB1
#define HW_PD_LOW GPIOB->BSRR|=GPIO_BSRR_BR1      //PB1
#define PDF_RF1_HIGH GPIOB->BSRR|=GPIO_BSRR_BS0    //PB0
#define PDF_RF1_LOW GPIOB->BSRR|=GPIO_BSRR_BR0     //PB0
#define PDF_RF2_HIGH GPIOA->BSRR|=GPIO_BSRR_BS7    //PA7
#define PDF_RF2_LOW GPIOA->BSRR|=GPIO_BSRR_BR7     //PA7

// PLL - LMX2491
#define R_CLK_HIGH GPIOB->BSRR|=GPIO_BSRR_BS7  //PB7
#define R_CLK_LOW GPIOB->BSRR|=GPIO_BSRR_BR7   //PB7
#define R_DATA_HIGH GPIOB->BSRR|=GPIO_BSRR_BS6 //PB6
#define R_DATA_LOW GPIOB->BSRR|=GPIO_BSRR_BR6  //PB6
#define R_LE_HIGH GPIOB->BSRR|=GPIO_BSRR_BS5   //PB5
#define R_LE_LOW GPIOB->BSRR|=GPIO_BSRR_BR5    //PB5
#define R_MUX (GPIOB->IDR & GPIO_IDR_ID4)      //PB4

#define R_TRIG1_HIGH GPIOB->BSRR|=GPIO_BSRR_BS3    //PB3
#define R_TRIG1_LOW GPIOB->BSRR|=GPIO_BSRR_BR3     //PB3

#define R_MOD_HIGH GPIOB->BSRR|=GPIO_BSRR_BS8   //PB8
#define R_MOD_LOW GPIOB->BSRR|=GPIO_BSRR_BR8    //PB8


// MAIN System

// RS-232
// PA9, PA10

#endif /* PINOUT_H_INCLUDED */