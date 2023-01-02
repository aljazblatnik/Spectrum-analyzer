#include "stm32f0xx.h"
#include "system_stm32f0xx.h"

#include "pinout.h"
#include "delay.h"

void attenuator_send(unsigned char data){
    // send data to attenuator
    aLE_LOW;
    data &= 0b011111111; // clear bit 7 - must be 0
    int i;
    for(i = 0; i < 8; i++){
        if(data & 0x1){
            aMOSI_HIGH;
        }
        else{
            aMOSI_LOW;
        }
        aCLK_HIGH;
        data = data>>1;
        delay_us(1);
        aCLK_LOW;
    }
    aLE_HIGH;
    delay_us(1);
    aLE_LOW;
}

void attenuator_init(void){
    aLE_LOW;
    aCLK_LOW;
    aMOSI_LOW;

    // set maximum attenuation
    attenuator_send(0b01111111);
}

