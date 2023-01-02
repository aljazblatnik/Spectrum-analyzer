#include "stm32g0xx.h"
#include "stm32g0b0xx.h"

#include "pinout.h"
#include "delay.h"

struct MAX2870_reg {
    unsigned int R0;
    unsigned int R1;
    unsigned int R2;
    unsigned int R3;
    unsigned int R4;
    unsigned int R5;
    unsigned int R6;
    unsigned char R0_adr;
    unsigned char R1_adr;
    unsigned char R2_adr;
    unsigned char R3_adr;
    unsigned char R4_adr;
    unsigned char R5_adr;
    unsigned char R6_adr;
} MAX2870_reg;

struct MAX2870_reg MAX2870_reg;

void send_MAX2870(unsigned char addr, unsigned int data){
    data &= 0xFFFFFFF8;
    data |= (addr&0x00000007); // prepare data
    int i = 32;
    LE_LOW;
    DATA_LOW;
    //send data to MAX2870
    while(i){
        CLK_LOW;
        if(data & 0x80000000){
            DATA_HIGH;
        }
        else{
            DATA_LOW;
        }
        data = data<<1;
        delay_us(10);
        CLK_HIGH;
        delay_us(10);
        i--;
    }
    CLK_LOW;
    delay_us(10);
    LE_HIGH;
    delay_us(10);
    LE_LOW;
    delay_us(10);
}

void send_MAX2870_all(struct MAX2870_reg data){
    send_MAX2870(data.R5_adr,data.R5);
    send_MAX2870(data.R4_adr,data.R4);
    send_MAX2870(data.R3_adr,data.R3);
    send_MAX2870(data.R2_adr,data.R2);
    send_MAX2870(data.R1_adr,data.R1);
    send_MAX2870(data.R0_adr,data.R0);
}

void MAX2870_init(void){
    // Register address
    MAX2870_reg.R0_adr = 0x00;
    MAX2870_reg.R1_adr = 0x01;
    MAX2870_reg.R2_adr = 0x02;
    MAX2870_reg.R3_adr = 0x03;
    MAX2870_reg.R4_adr = 0x04;
    MAX2870_reg.R5_adr = 0x05;
    MAX2870_reg.R6_adr = 0x06;

    // Default values
    MAX2870_reg.R0 = 0x80D38000; // old: 0x004B0000
    MAX2870_reg.R1 = 0x80008011; // old: 0x2000FFF9 - CPL=01,CPT=00,P=000000000001,M=4000=0xFA0=111110100000
    MAX2870_reg.R2 = 0x58011FC2; // old: 0x00004042 - LDS=0,SDN=11,MUX=110,DBR=0,RDIV2=0,R=0000000001,REG4DB=1,CP=1100,LDF=0,LDP=0,PDP=1,SHDN=0,TRI=0,RST=0
    MAX2870_reg.R3 = 0x0100000B; // old: 0x0000000B - VCO=000000,VASSHDN=0,VASTEMP=0,CSM=0,MUTEDEL=0,CDM=00,CDIV=000100000000
    MAX2870_reg.R4 = 0x608C8234; // old: 0x6180B23C - 011,SDLDO=0,SDDIV=0,SDREF=0,BS=0x280,FB=1,SDVCO=0,MTLD=0,BDIV=0,RFBEN=0,BPWR=0,RFAEN=1,APWR=11
    MAX2870_reg.R5 = 0x01400005; // old: 0x00400005 - VAS_DLY=00,SDPLL=0,F01=1,LD=01,MUX3=0,ADCS=0,ADCM=000
    MAX2870_reg.R6 = 0; // read register

    send_MAX2870(MAX2870_reg.R5_adr,MAX2870_reg.R5);
    delay_ms(20);

    send_MAX2870(MAX2870_reg.R4_adr,MAX2870_reg.R4);
    send_MAX2870(MAX2870_reg.R3_adr,MAX2870_reg.R3);
    send_MAX2870(MAX2870_reg.R2_adr,MAX2870_reg.R2);
    send_MAX2870(MAX2870_reg.R1_adr,MAX2870_reg.R1);
    send_MAX2870(MAX2870_reg.R0_adr,MAX2870_reg.R0);

    send_MAX2870(MAX2870_reg.R5_adr,MAX2870_reg.R5);
    send_MAX2870(MAX2870_reg.R4_adr,MAX2870_reg.R4);
    send_MAX2870(MAX2870_reg.R3_adr,MAX2870_reg.R3);
    send_MAX2870(MAX2870_reg.R2_adr,MAX2870_reg.R2);
    send_MAX2870(MAX2870_reg.R1_adr,MAX2870_reg.R1);
    send_MAX2870(MAX2870_reg.R0_adr,MAX2870_reg.R0);
}

unsigned int read_MAX2870(void){
    // pripravimo registre R2 in R5 da MUX spremenimo v SPI OUT
    //MAX2870_reg.R2 &= 0xE3FFFFFF; // pocistimo MUX 2-0
    //MAX2870_reg.R5 &= 0xFFFBFFFF; // pocistimo MUX 3
    //MAX2870_reg.R2 |= 0x10000000; // MUX 2 - true (MUX 1100)
    //MAX2870_reg.R5 |= 0x00040000; // MUX 3 - true

    MAX2870_reg.R2 = 0x70007842;
    MAX2870_reg.R5 = 0x01440005;

    send_MAX2870_all(MAX2870_reg);
    send_MAX2870_all(MAX2870_reg);
    delay_ms(1);

    unsigned int data = 0x00000006; // samo naslov R6
    int i = 32;
    DATA_LOW;
    LE_LOW;
    //shift R6 address to MAX2870
    while(i){
        CLK_LOW;
        if(data & 0x80000000){
            DATA_HIGH;
        }
        else{
            DATA_LOW;
        }
        data = data<<1;
        delay_us(1);
        CLK_HIGH;
        delay_us(1);
        i--;
    }
    CLK_LOW;
    delay_us(1);
    LE_HIGH;
    delay_us(1);
    CLK_HIGH;
    delay_us(1);
    CLK_LOW;
    // sedaj preberemo podatke
    i = 32;
    unsigned data_out = 0;
    while(i){
        CLK_HIGH;
        delay_us(1);
        if(MUX){
            data_out |= 1;
        }
        data_out = data_out<<1;
        CLK_LOW;
        delay_us(1);
        i--;
    }
    LE_LOW;
    // spravimo pin nazaj na LD
    MAX2870_reg.R2 = 0x78007842;
    MAX2870_reg.R5 = 0x01400005;
    //MAX2870_reg.R2 &= 0xE3FFFFFF; // pocistimo MUX 2-0
    //MAX2870_reg.R5 &= 0xFFFBFFFF; // pocistimo MUX 3
    //MAX2870_reg.R2 |= 0x18000000; // MUX 0110
    send_MAX2870_all(MAX2870_reg);

    //read MAX2870 R6 register
    MAX2870_reg.R6 = data_out; // spravimo register v struct
    return data_out;
}

int MAX2870_set_frequency(int frequency){
    int REG0 = 0; // all 0
    int N = 0;
    int FRAC = 0;
    int DIV = 0;
    int freq_out = 0;

    while(DIV <= 7){
        if(frequency > (3000000>>DIV)) break; // delimo frekvenco vsakic z dve
        DIV++;
    }

    N = (frequency<<DIV)/32000; // prej frekvenco ustrezno mnozimo z izbranim delilnikom, da nastavimo vrednost
    FRAC = (((frequency<<DIV)%32000)*4000)/32000;

    freq_out = ((32000*N)+((FRAC*32000)/4000))>>DIV;

    FRAC = FRAC<<3;
    N = N<<15;
    REG0 |= (FRAC & 0x00007FF8);
    REG0 |= (N & 0x7FFF8000);

    MAX2870_reg.R0 = REG0;
    MAX2870_reg.R4 &= 0b11111111100011111111111111111111; //resetiramo DIVA bite
    MAX2870_reg.R4 |= DIV<<20; // Dodamo DIV

    send_MAX2870_all(MAX2870_reg);

    return freq_out;
}
