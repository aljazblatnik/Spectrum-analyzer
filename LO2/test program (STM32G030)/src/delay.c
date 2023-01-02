#include "stm32g0xx.h"
#include "stm32g0b0xx.h" // used for auto complete function of editor only (included in "stm32g0xx.h"

// tested/optimized with GCC -02 setting

void timer6_init(void){
    // delay timer 1 ms
    RCC->APBENR1 |= RCC_APBENR1_TIM6EN; // enable clock
    TIM6->PSC = 47999;
    TIM6->ARR = 0;
    TIM6->EGR = TIM_EGR_UG; // Re-initializes the timer counter
}

void delay_ms(int ms){
    TIM6->EGR = TIM_EGR_UG; // Re-initializes the timer counter
    TIM6->ARR = ms;
    TIM6->CNT = 1;
    TIM6->CR1 |= TIM_CR1_CEN; //start counter
    TIM6->SR &= ~TIM_SR_UIF; // Clear flag
    while(!(TIM6->SR & TIM_SR_UIF));
    TIM6->SR &= ~TIM_SR_UIF; // Clear flag
    TIM6->CR1 &= ~TIM_CR1_CEN; //stop counter
}

void timer7_init(void){
    RCC->APBENR1 |= RCC_APBENR1_TIM7EN; // clock enable
    TIM7->PSC = 47;
    TIM7->ARR = 1;
    TIM7->EGR = TIM_EGR_UG; // Re-initializes the timer counter
}

void delay_us(int us){
    // magic ... don´t touch!
    if(us < 2){
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        __NOP();
        if(us == 1){
            return;
        }
    }
    TIM7->EGR = TIM_EGR_UG; // Re-initializes the timer counter
    TIM7->ARR = us;
    TIM7->CNT = 2;
    TIM7->CR1 |= TIM_CR1_CEN; //start counter
    //TIM7->ARR = us;
    TIM7->SR &= ~TIM_SR_UIF; // Clear flag
    //TIM7->CR1 |= TIM_CR1_CEN; //start counter
    while(!(TIM7->SR & TIM_SR_UIF));
    TIM7->CR1 &= ~TIM_CR1_CEN; //stop counter
}

void init_delay(void){
    timer6_init();
    timer7_init();
}
