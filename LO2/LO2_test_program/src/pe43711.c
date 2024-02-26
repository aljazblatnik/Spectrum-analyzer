#include "stm32g4xx.h"
#include "stm32g441xx.h" // Suggestions

#include "pinout.h"
#include "structs.h"
#include "shift_reg.h"
#include "delay.h"

extern struct shift_reg_srtuct shift_reg;

void send_attenuator_SA(unsigned char data){
    // send data to attenuator
    shift_reg.Att_SA_LE = 0;
    shift_reg.Att_SA_CLK = 0;
    sendDataToShiftRegisters();
    data &= 0b011111111; // clear bit 7 - must be 0
    int i;
    for(i = 0; i < 8; i++){
        if(data & 0x1){
            shift_reg.Att_SA_DATA = 1;
        }
        else{
            shift_reg.Att_SA_DATA = 0;
        }
        sendDataToShiftRegisters();
        shift_reg.Att_SA_CLK = 1;
        sendDataToShiftRegisters();
        data = data>>1;
        shift_reg.Att_SA_CLK = 0;
    }
    shift_reg.Att_SA_LE = 1;
    sendDataToShiftRegisters();
    shift_reg.Att_SA_LE = 0;
    shift_reg.Att_SA_DATA = 0;
    sendDataToShiftRegisters();
}

void send_attenuator_TG(unsigned char data){
    // send data to attenuator
    shift_reg.Att_TG_LE = 0;
    shift_reg.Att_TG_CLK = 0;
    sendDataToShiftRegisters();
    data &= 0b011111111; // clear bit 7 - must be 0
    int i;
    for(i = 0; i < 8; i++){
        if(data & 0x1){
            shift_reg.Att_TG_DATA = 1;
        }
        else{
            shift_reg.Att_TG_DATA = 0;
        }
        sendDataToShiftRegisters();
        shift_reg.Att_TG_CLK = 1;
        sendDataToShiftRegisters();
        data = data>>1;
        shift_reg.Att_TG_CLK = 0;
    }
    shift_reg.Att_TG_LE = 1;
    sendDataToShiftRegisters();
    shift_reg.Att_TG_LE = 0;
    shift_reg.Att_TG_DATA = 0;
    sendDataToShiftRegisters();
}

void init_attenuators(void){
    shift_reg.Att_SA_LE = 0;
    shift_reg.Att_SA_CLK = 0;
    shift_reg.Att_SA_DATA = 0;

    shift_reg.Att_TG_LE = 0;
    shift_reg.Att_TG_CLK = 0;
    shift_reg.Att_TG_DATA = 0;

    sendDataToShiftRegisters();

    send_attenuator_SA(0b00000000); // minimum att
    send_attenuator_TG(0b00000000); // minimum att

    // set maximum attenuation
    //attenuator_send(0b01111111); //default state
}
