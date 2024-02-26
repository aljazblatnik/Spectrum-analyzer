#include "stm32g4xx.h"


// tested/optimized with GCC -02 setting

void timer6_init(void){
    // delay timer 1 ms
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM6EN; // enable clock
    TIM6->PSC = 60000;
    TIM6->ARR = 0;
    TIM6->EGR = TIM_EGR_UG; // Re-initializes the timer counter
}

void delay_ms(int ms){
    // Max 30000 mS - 30S !!
    TIM6->EGR = TIM_EGR_UG; // Re-initializes the timer counter
    TIM6->ARR = ms*2;
    TIM6->CNT = 1;
    TIM6->CR1 |= TIM_CR1_CEN; //start counter
    TIM6->SR &= ~TIM_SR_UIF; // Clear flag
    while(!(TIM6->SR & TIM_SR_UIF));
    //TIM6->SR &= ~TIM_SR_UIF; // Clear flag
    TIM6->CR1 &= ~TIM_CR1_CEN; //stop counter
}

void timer7_init(void){
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM7EN; // clock enable
    TIM7->PSC = 120;
    TIM7->ARR = 1;
    TIM7->EGR = TIM_EGR_UG; // Re-initializes the timer counter
}

void delay_us(int us){
    TIM7->EGR = TIM_EGR_UG; // Re-initializes the timer counter
    TIM7->ARR = us;
    TIM7->CNT = 2;
    TIM7->SR &= ~TIM_SR_UIF; // Clear flag
    TIM7->CR1 |= TIM_CR1_CEN; //start counter
    while(!(TIM7->SR & TIM_SR_UIF));
    TIM7->CR1 &= ~TIM_CR1_CEN; //stop counter
}

void init_delay(void){
    timer6_init();
    timer7_init();
}
