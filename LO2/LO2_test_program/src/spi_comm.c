#include "stm32g4xx.h"
#include "stm32g441xx.h" // Suggestions
#include "stdint.h"
#include "pinout.h"
#include "delay.h"

extern int16_t counter;

void init_spi(void){
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable SPI clock

    SPI1->CR1 |= SPI_CR1_CPOL | SPI_CR1_CPHA; // Clock 120 MHz / 32 = 3.75 MHz, Slave mode, clock 1 when idle, clock up data transmision - MODE 3 on raspberry
    SPI1->CR1 |= SPI_CR1_SSM; // Enable software slave control - slave not active - logical high
    SPI1->CR2 = 0b0000101100000000; // 12 bit data
    //SPI1->CR2 = 0b0000011100000000;  // 8 bit
    //SPI1->CR2 |= SPI_CR2_TXEIE;
    //SPI1->CR2 |= SPI_CR2_RXNEIE;
    SPI1->CR1 |= SPI_CR1_SPE; // enable spi

    NVIC_EnableIRQ(SPI1_IRQn);
    NVIC_SetPriority(SPI1_IRQn,3);   //enable interrupt
}

void spi_comm_start_frame(void){
    SPI1->DR = 0x000;   // sync bits 10 x 0x000
    SPI1->CR2 |= SPI_CR2_TXEIE; // enable TXE interrupt
}

/*
// OLD
void init_spi(void){
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; // Enable SPI clock

    SPI1->CR1 |= SPI_CR1_BR_2 | SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA; // Clock 120 MHz / 32 = 3.75 MHz, MAster mode, clock 1 when idle, clock up data transmision - MODE 3 on raspberry
    SPI1->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI; // Enable software slave control - slave not active - logical high
    SPI1->CR2 = 0b0000101100000000; // 12 bit data
    SPI1->CR1 |= SPI_CR1_SPE; // enable spi

    NVIC_EnableIRQ(SPI1_IRQn);
    NVIC_SetPriority(SPI1_IRQn,3);   //enable interrupt
}

void spi_comm_start_frame(void){
    for(int i = 0; i<10; i++){
        SPI1->DR = 0x000;   // sync bits 10 x 0x000
    }
    SPI1->CR2 |= SPI_CR2_TXEIE; // enable TXE interrupt
}
*/
