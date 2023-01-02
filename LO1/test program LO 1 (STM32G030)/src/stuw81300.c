#include "stm32g0xx.h"
#include "stm32g0b0xx.h" // used for auto complete function of editor only (included in "stm32g0xx.h"

#include "pinout.h"
#include "delay.h"

void send_STUW81300(unsigned char addr, unsigned int data) {
    // structure R/W + ADDR (4bit) + DATA (27bit) - 32 bits

    unsigned int data_out = 0;

    data &= 0x07FFFFFF; // ocistimo podatke
    addr &= 0x0F; // ocistimo naslov
    data_out = addr; // nalozimo naslov v izhodni register
    data_out = data_out<<27; // premaknemo naslov 16 bitov levo
    data_out &= 0xF8000000; // preventivno ocistimo izhodne podatke
    data_out |= data; // pripnemo se podatke
    LE_LOW;
    for(int i = 0; i < 32; i++) {
        CLK_LOW;
        if(data_out & 0x80000000) { // bit 32
            DATA_HIGH;
        } else {
            DATA_LOW;
        }
        delay_us(5); // safety delay - Give enough time for data line to stabilize
        CLK_HIGH;
        delay_us(5);
        data_out = data_out<<1;
    }
    CLK_LOW;
    LE_HIGH;
}

unsigned int read_STUW81300(unsigned int addr) {
    // structure R/W + ADDR (4bit) + DATA (27bit) - 32 bits

    unsigned int data = 0;  // pripravimo za podatke
    addr = addr<<27;     // premaknemo naslov za 16 bitov levo
    addr &= 0x78000000;   // ocistimo naslov +  dodamo bit 32 na 1 za branje
    addr |= 0x80000000;
    LE_LOW;
    for(int i = 0; i < 32; i++){
        CLK_LOW;
        if(addr & 0x80000000) { // bit 24
            DATA_HIGH;
        } else {
            DATA_LOW;
        }
        delay_us(5);
        CLK_HIGH;
        delay_us(5);
        data = data<<1; // najprej premaknemo
        if(MUX) {
            data |= 1; // postavimo LSB bit na ena
        }
        addr = addr<<1;
    }
    CLK_LOW;
    LE_HIGH;
    data &= 0x07FFFFFF; // ocistimo podatke
    return data;
}

void init_STUW81300(void) {
    send_STUW81300(9, 0); // initialization of ST9 register
    send_STUW81300(8, 3); // REG_4V5 = 4.5V
    send_STUW81300(7, 0b001000000000000000000000010); // LD_SDO - CMOS output mode
    send_STUW81300(0, 0b010011000000000000000111101); // 3mA CP
}

void startup_STUW81300(void) {
    /*
    send_STUW81300(9, 0); // initialization of ST9 register
    send_STUW81300(8, 0b000000000000000000000000011); //
    send_STUW81300(7, 0b001000000000000000000000000); //
    send_STUW81300(6, 0b000000000000001000000000000); //
    send_STUW81300(5, 0b000000000000000000000000000); //
    send_STUW81300(4, 0b000000000111001001100100101); //
    send_STUW81300(3, 0b000000000000000000000000001); //
    send_STUW81300(2, 0b000001111111111111111111111); //
    send_STUW81300(1, 0b000000000000000000000000000); // frac 0
    send_STUW81300(0, 0b010011000000000000001111101); // 125 N
    */
    // 3.3V works ok, 4.5V 271 kHZ spurs - check PCB layout
    send_STUW81300(9, 0); // initialization of ST9 register
    send_STUW81300(8, 0b000000000000000000000000010); // 0b000000000000000000000000010 4V5 0b000000000000000000000000011
    send_STUW81300(7, 0b001000000000000000000000011); // LD_SDO - CMOS output mode
    send_STUW81300(6, 0b000000000000001000000000000); //
    send_STUW81300(5, 0b000000000000000000000010000); // RF2 low power mode
    send_STUW81300(4, 0b001100000010101001110100110); // 3.3V reg 0b001100000010101001100100101 4V5 0b000000000100001001100000101 -MUTE on unlock ON (bit 7)
    send_STUW81300(3, 0b000000000000110000000000010); // // REF /4 + R/2 - 81,25 MHz REF
    send_STUW81300(2, 0b000001111111111111111111111); // RF2 disabled, MAX MOD value
    send_STUW81300(1, 0b000000000000000000000000000); // frac 0
    send_STUW81300(0, 0b010011000000000000000111101); // 61 N 0110000 start; 1111111 stop
}

void test_STUW81300(void) {
    send_STUW81300(1, 0b100000000000000000000000000); // frac 0
    send_STUW81300(0, 0b110011000000000000000111101); // 61 N
}

void change_freq_STUW81300(unsigned char N){
    unsigned int data = 0;
    data = 0b010011000000000000000000000;
    data |= N;
    send_STUW81300(0, data);
    send_STUW81300(6, 0b000000000000001000000000000); // 0b000000100000001000000000000 (bolje dela brez auto cal rutine)
}
