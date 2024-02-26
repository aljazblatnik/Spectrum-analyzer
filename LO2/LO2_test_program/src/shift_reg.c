#include "stm32g4xx.h"
#include "stm32g441xx.h" // Suggestions
#include "stdint.h"
#include "pinout.h"
#include "delay.h"
#include "shift_reg.h"
#include "structs.h"

struct shift_reg_srtuct shift_reg;

void packDataToRegisters(void){
    unsigned char temp1, temp2, temp3;
    shift_reg.LE1 = 0; // init latches
    shift_reg.LE2 = 0;

    temp1 = shift_reg.Register1; //save last Reg. value
    shift_reg.Register1 = 0;
    shift_reg.Register1 |= (shift_reg.PLL_TG_RF_EN) |
                           (shift_reg.PLL_TG_LE << 1) |
                           (shift_reg.PLL_TG_DATA << 2) |
                           (shift_reg.PLL_TG_CLK << 3) |
                           (shift_reg.PLL_SA_RF_EN << 4) |
                           (shift_reg.PLL_SA_LE << 5) |
                           (shift_reg.PLL_SA_DATA << 6) |
                           (shift_reg.PLL_SA_CLK << 7);
    if(temp1 != shift_reg.Register1){
        shift_reg.LE1 = 1;   // data change for reg1
    }

    temp1 = shift_reg.Register2; //save last Reg. value
    shift_reg.Register2 = 0;
    shift_reg.Register2 |= (shift_reg.LED_reg2) |
                           (shift_reg.LC_3M << 1) |
                           (shift_reg.LC_1M << 2) |
                           (shift_reg.LC_300k << 3) |
                           (shift_reg.LC_100k << 4) |
                           (shift_reg.LC_30k << 5);

    temp2 = shift_reg.Register3; //save last Reg. value
    shift_reg.Register3 = 0;
    shift_reg.Register3 |= (shift_reg.LED_reg3) |
                           (shift_reg.Xtal_10k << 1) |
                           (shift_reg.Xtal_3k << 2) |
                           (shift_reg.Xtal_1k << 3) |
                           (shift_reg.Xtal_ON << 4);

    temp3 = shift_reg.Register4; //save last Reg. value
    shift_reg.Register4 = 0;
    shift_reg.Register4 |= (shift_reg.LED_reg4) |
                           (shift_reg.Att_SA_LE << 1) |
                           (shift_reg.Att_SA_CLK << 2) |
                           (shift_reg.Att_SA_DATA << 3) |
                           (shift_reg.Att_TG_DATA << 4) |
                           (shift_reg.Att_TG_CLK << 5) |
                           (shift_reg.Att_TG_LE << 6);

    if((temp1 != shift_reg.Register2) || (temp2 != shift_reg.Register3) || (temp3 != shift_reg.Register2)){
        shift_reg.LE2 = 1;   // data change for reg2, reg3 or reg4
    }
}

void init_shift_reg(void){
    // reset comm lines
    SHIFT_DATA_OFF;
    SHIFT_CLK_OFF;
    SHIFT_LE1_OFF;
    SHIFT_LE2_OFF;

    shift_reg.PLL_SA_CLK = 0;
    shift_reg.PLL_SA_DATA = 0;
    shift_reg.PLL_SA_LE = 1;
    shift_reg.PLL_SA_RF_EN = 0;

    shift_reg.PLL_TG_CLK = 0;
    shift_reg.PLL_TG_DATA = 0;
    shift_reg.PLL_TG_LE = 1;
    shift_reg.PLL_TG_RF_EN = 0;

    shift_reg.Xtal_ON = 0;
    shift_reg.Xtal_10k = 0;
    shift_reg.Xtal_3k = 0;
    shift_reg.Xtal_1k = 0;

    shift_reg.LC_3M = 1;
    shift_reg.LC_1M = 0;
    shift_reg.LC_300k = 0;
    shift_reg.LC_100k = 0;
    shift_reg.LC_30k = 0;

    shift_reg.Att_SA_CLK = 0;
    shift_reg.Att_SA_DATA = 0;
    shift_reg.Att_SA_LE = 0;

    shift_reg.Att_TG_CLK = 0;
    shift_reg.Att_TG_DATA = 0;
    shift_reg.Att_TG_LE = 0;

    shift_reg.LED_reg2 = 0;
    shift_reg.LED_reg3 = 0;
    shift_reg.LED_reg4 = 0;

    shift_reg.LE1 = 0; // init latches
    shift_reg.LE2 = 0;
}

void sendDataToShiftRegisters(void){
    packDataToRegisters(); // pack data into variables

    // reset comm lines
    SHIFT_DATA_OFF;
    SHIFT_CLK_OFF;
    SHIFT_LE1_OFF;
    SHIFT_LE2_OFF;

    if(shift_reg.LE2){
        // New data for 2/2 shift registers
        unsigned char temp = shift_reg.Register4;
        for(int i=0; i<8; i++){
            if(temp & 0b10000000){
                SHIFT_DATA_ON;
            }
            else{
                SHIFT_DATA_OFF;
            }
            SHIFT_CLK_ON;
            temp = temp << 1;
            //delay_us(1); // just to be an a safe side
            SHIFT_CLK_OFF;
        }
        temp = shift_reg.Register3;
        for(int i=0; i<8; i++){
            if(temp & 0b10000000){
                SHIFT_DATA_ON;
            }
            else{
                SHIFT_DATA_OFF;
            }
            SHIFT_CLK_ON;
            temp = temp << 1;
            //delay_us(1); // just to be an a safe side
            SHIFT_CLK_OFF;
        }
        temp = shift_reg.Register2;
        for(int i=0; i<8; i++){
            if(temp & 0b10000000){
                SHIFT_DATA_ON;
            }
            else{
                SHIFT_DATA_OFF;
            }
            SHIFT_CLK_ON;
            temp = temp << 1;
            //delay_us(1); // just to be an a safe side
            SHIFT_CLK_OFF;
        }
    }
    if(shift_reg.LE1){
        // new data dot 1/2 shift registers
        unsigned char temp = shift_reg.Register1;
        for(int i=0; i<8; i++){
            if(temp & 0b10000000){
                SHIFT_DATA_ON;
            }
            else{
                SHIFT_DATA_OFF;
            }
            SHIFT_CLK_ON;
            temp = temp << 1;
            //delay_us(1); // just to be an a safe side
            SHIFT_CLK_OFF;
        }
    }
    else{
        SHIFT_DATA_OFF; // send dummy end data
        for(int i=0; i<8; i++){
            SHIFT_CLK_ON;
            //delay_us(2); // just to be an a safe side
            SHIFT_CLK_OFF;
        }
    }
    // strobe
    if(shift_reg.LE1){
        SHIFT_LE1_ON;
    }
    if(shift_reg.LE2){
        SHIFT_LE2_ON;
    }
    //delay_us(2); // small delay
    SHIFT_LE1_OFF;
    SHIFT_LE2_OFF;
}

/*
BIT 0 - LED Register 2
BIT 1 - LED Register 3
BIT 2 - LED Register 4
*/
void LEDShiftRegister(unsigned char led_data){
    shift_reg.LED_reg2 = 0;
    shift_reg.LED_reg3 = 0;
    shift_reg.LED_reg4 = 0;
    if(led_data & 0b00000001){
        shift_reg.LED_reg2 = 1;
    }
    if(led_data & 0b00000010){
        shift_reg.LED_reg3 = 1;
    }
    if(led_data & 0b00000100){
        shift_reg.LED_reg4 = 1;
    }
    sendDataToShiftRegisters();
}
