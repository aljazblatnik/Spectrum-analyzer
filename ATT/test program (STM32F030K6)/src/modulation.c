#include "stm32f0xx.h"
#include "system_stm32f0xx.h"
#include <stdio.h>
#include <stdlib.h>

#include "pinout.h"

int denominator = 1; // imenovalec ulomka
int numerator = 0; // stevec ulomka
int denominator_i = 0; //register za preracun ulomka

/*
TO-DO ulomek lahko izvaja dodaten stevec, ki je povezan na triger TIM3 - potem je kode prekinitvenih rutin prakticno nic
*/

void timer3_init(void){
    RCC->APB1ENR |= (RCC_APB1ENR_PWREN | RCC_APB1ENR_TIM3EN); // prizgemo uro periferiji

    TIM3->CR1 |= TIM_CR1_ARPE | TIM_CR1_DIR; // preload register ima svoj buffer, stetje navzdol
    TIM3->SMCR = 0; // slave mode off
    TIM3->DIER |= TIM_DIER_UIE; // omogocimo prekinitev za Update, ko se resetira stevec
    TIM3->CCMR1 |= TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_2; // PWM mode 1 - In downcounting, channel 1 is inactive (OC1REF=‘0) as long as TIMx_CNT>TIMx_CCR1 else active (OC1REF=1).
    TIM3->CCMR1 |= TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2;
    TIM3->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE; // Preload register on TIMx_CCR1 enabled. Read/Write operations access the preload register. TIMx_CCR1 preload value is loaded in the active register at each update event
    // CC3 je output brez bufferja, a zakljucen interno (ne vpliva na izhode)
    TIM3->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E; // On - OC1 signal is output on the corresponding output pin

    TIM3->CNT = 0; // reset counter
    TIM3->PSC = 0; //prescaler value
    TIM3->ARR = 0xFFFFFFFF; // vrednost 0 blokira stevec! - Counter overflow/underflow sproži update registrov

    TIM3->CCR1 = 0;
    TIM3->CCR2 = 0;

    TIM3->EGR = TIM_EGR_UG; // Re-initializes the timer counter

    NVIC_EnableIRQ(TIM3_IRQn);
    NVIC_SetPriority(TIM3_IRQn,1);   //enable interrupt

    //TIM3->SR & TIM_SR_CC3IF;
}

void modulationSet(int mod_freq){
    // Ustrezno nastavimo stevec in vkljucimo modulacijo
    /*
    Tu napisemo nekoliko vec kode da nastavimo ustrezen modulo deljenja in tega ne prepuscamo samodejni
    odlocitvi programske kode. V praksi se izkaze da je bolje, ce modula ne spreminjamo konstantno saj tako
    dobimo bolj zaneslivo delovanje
    */
    // premaknemo output AF1 nacin
    MOD_LOW;
    RFEN_LOW;
    GPIOA->MODER &= (~GPIO_MODER_MODER6) | GPIO_MODER_MODER6_1; // MOD output - AF1
    GPIOB->MODER &= (~GPIO_MODER_MODER5) | GPIO_MODER_MODER5_1;; // MOD output - AF1

    if(mod_freq <= 0){
        // Ustavimo stevec
        TIM3->CR1 &= ~TIM_CR1_CEN; // TIMER OFF
        TIM3->EGR = TIM_EGR_UG; // Re-initializes the timer counter
        NVIC_DisableIRQ(TIM3_IRQn); // onemogocimo prekinitve

        // premaknemo output v konstanten izhod
        GPIOA->MODER &= (~GPIO_MODER_MODER6) | GPIO_MODER_MODER6_0; // MOD output - normal output
        GPIOB->MODER &= (~GPIO_MODER_MODER5) | GPIO_MODER_MODER5_0;; // MOD output - normal output
        MOD_HIGH;
        RFEN_HIGH;
    }
    else if(mod_freq < 100){
        // pod 100 Hz se povecamo resolucijo
        // delimo uro s 960 - refereqnca je tako 50 kHz

        // izracunamo kaksen stevec potrebujemo, zaokrozujemo navzdol
        int N_counter = 50000/mod_freq;
        numerator = 50000%mod_freq;
        denominator = mod_freq;
        denominator_i = denominator;

        TIM3->CCR1 = N_counter/2;
        TIM3->CCR2 = N_counter/2; // priblizno polovica periode
        TIM3->ARR = N_counter-1; // celotna perioda

        TIM3->PSC = 960-1; // The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1).

        // vklopimo stevec
        TIM3->EGR = TIM_EGR_UG; // Re-initializes the timer counter
        TIM3->CR1 |= TIM_CR1_CEN; // TIMER ON
        NVIC_EnableIRQ(TIM3_IRQn); // omogocimo prekinitve
    }
    else if(mod_freq <= 1000){
        // Ce smo pod 1 khz moramo zamenjati modulo deljenja ure in faktorje preracunavanja
        // delimo uro s 8 - referenca je tako 6 MHz

        // izracunamo kaksen stevec potrebujemo, zaokrozujemo navzdol
        int N_counter = 6000000/mod_freq;
        numerator = 6000000%mod_freq;
        denominator = mod_freq;
        denominator_i = denominator;

        TIM3->CCR1 = N_counter/2;
        TIM3->CCR2 = N_counter/2; // priblizno polovica periode
        TIM3->ARR = N_counter-1; // celotna perioda

        TIM3->PSC = 8-1; // The counter clock frequency CK_CNT is equal to fCK_PSC / (PSC[15:0] + 1).

        // vklopimo stevec
        TIM3->EGR = TIM_EGR_UG; // Re-initializes the timer counter
        TIM3->CR1 |= TIM_CR1_CEN; // TIMER ON
        NVIC_EnableIRQ(TIM3_IRQn); // omogocimo prekinitve
    }
    else{
        // izracunamo kaksen stevec potrebujemo, zaokrozujemo navzdol
        int N_counter = 48000000/mod_freq;
        numerator = 48000000%mod_freq;
        denominator = mod_freq;
        denominator_i = denominator;

        TIM3->CCR1 = N_counter/2;
        TIM3->CCR2 = N_counter/2; // priblizno polovica periode
        TIM3->ARR = N_counter-1; // celotna perioda

        TIM3->PSC = 0;

        // vklopimo stevec
        TIM3->EGR = TIM_EGR_UG; // Re-initializes the timer counter
        TIM3->CR1 |= TIM_CR1_CEN; // TIMER ON
        NVIC_EnableIRQ(TIM3_IRQn); // omogocimo prekinitve
    }
}

void TIM3_IRQHandler(void){
    // prekinitvena rutina za timer 3
    TIM3->SR = 0; //pocistimo kar vse zastavice. Drugih prekinitev ne uporabljamo

    // potem opravimo preracun registrov
    if(numerator){
        if(denominator_i){
            if(denominator_i == numerator){
                TIM3->ARR++; // povecamo compare vrednost za ena
            }
            denominator_i--;
        }
        else{
            denominator_i = denominator; // vstavimo novo vrednost za stevec
            TIM3->ARR--; // zmanjsajmo nazaj na zacetno vrednost
        }
    }
}
