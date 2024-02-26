#include "stm32g4xx.h"
#include "stm32g441xx.h" // Suggestions

#include "pinout.h"
#include "delay.h"
#include "usart.h"

long long R_FREQ = 650000000; //reference frequency in kHz - only in CW mode

void send_LMX2491(unsigned int addr, unsigned int data)
{
    // struktura 0x00 + R/W + ADDR (15bit) + DATA (8bit)
    // posljemo en register na enkrat
    data &= 0x000000FF; // ocistimo podatke
    addr = addr<<8;     // premaknemo naslov na mesto 1 byte levo
    data |= (addr&0x00FFFF00);   // ocistimo naslov in zdruzimo
    data &= 0xFF7FFFFF; // bit 24 na 0 - pisanje
    R_LE_LOW;
    R_DATA_LOW;
    for(int i = 0; i<24; i++)
    {
        R_CLK_LOW;
        if(data & 0x00800000)   // gledamo bit 24
        {
            R_DATA_HIGH;
        }
        else
        {
            R_DATA_LOW;
        }
        data = data<<1;
        delay_us(10);
        R_CLK_HIGH;
        delay_us(10);
    }
    R_CLK_LOW;
    delay_us(10);
    R_LE_HIGH;
    delay_us(10);  // obvezen delay! drugaèe ne dela
}

unsigned int read_LMX2491(unsigned int addr)
{
    send_LMX2491(39,0b00111010); // Nastavimo MUX pin za digitalni izhod MISO
    // struktura 0x00 + R/W + ADDR (15bit) + DATA (8bit)
    // beremo en register na enkrat
    unsigned char data = 0;  // pripravimo za podatke
    addr = addr<<8;     // premaknemo naslov na mesto 1 byte levo
    addr &= 0x00FFFF00;   // ocistimo naslov
    addr |= 0x00800000; // bit 24 na 1 - branje
    R_LE_LOW;
    R_DATA_LOW;
    for(int i = 0; i<24; i++)
    {
        R_CLK_LOW;
        if(addr & 0x00800000)   // gledamo bit 24
        {
            R_DATA_HIGH;
        }
        else
        {
            R_DATA_LOW;
        }
        addr = addr<<1;
        delay_us(10);
        R_CLK_HIGH;
        delay_us(10);
        data = data<<1; // najprej premaknemo
        if(R_MUX)
        {
            data |= 0x00000001; // postavimo LSB bit na ena
        }
    }
    R_CLK_LOW;
    delay_us(10);
    R_LE_HIGH;
    return data;
    send_LMX2491(39,0b01010010); // Nastavimo MUX pin za LD
}

void init_LMX2491(void)
{
    R_LE_HIGH;
    R_DATA_LOW;
    R_CLK_LOW;
    R_MOD_LOW;
    send_LMX2491(2,0b00000101); // power up, ignore CE, reset the device
    delay_ms(10); // wait for registers to reset
    send_LMX2491(2,0b00000001); // reset off
    send_LMX2491(39,0b01010010); // Nastavimo MUX pin za LD
    send_LMX2491(38,0b00011111); // Nastavitev MOD za ramp vhod - Input MOD
    send_LMX2491(36,0b00001111); // Nastavitev TRIG1 input- as TRIG1
    send_LMX2491(34,0b10100000); // fpd < 30 MHz
    send_LMX2491(27,0b00001000); // single ended drive, x2 dubbler off - no cycle slip reduction
    send_LMX2491(28,0b00011111); // CP current 3100uA // positive CP polarity
    send_LMX2491(25,2); // R/2

    send_LMX2491(24,0xFF); // max denominator
    send_LMX2491(23,0xFF); // max denominator
    send_LMX2491(22,0xFF); // max denominator
    send_LMX2491(21,0x7F); // numerator = 0
    send_LMX2491(20,0xFF); // numerator = 0
    send_LMX2491(19,0xFF); // numerator = 0
    send_LMX2491(18,0b00110000); // FRAC ORDER: 3, FRAC DITHER: weak, PLL_N[17:16] - in N only, you have to switch to Int mode only 0b00001100 - int mode best performance
    send_LMX2491(16,32); // PLL_N - START f 640 MHz
}

void setup_ramp_0(unsigned int F_start, unsigned int F_step, unsigned int ramp_length)
{
    unsigned char temp;
    temp = (ramp_length>>8) & 0xFF;
    send_LMX2491(91,temp);
    temp = ramp_length & 0xFF;
    send_LMX2491(90,temp);

    temp = (F_step>>24) & 0x3F; // No delay and fast lock
    send_LMX2491(89,temp);
    temp = (F_step>>16) & 0xFF;
    send_LMX2491(88,temp);
    temp = (F_step>>8) & 0xFF;
    send_LMX2491(87,temp);
    temp = F_step & 0xFF;
    send_LMX2491(86,temp);

    temp = (F_start>>16) & 0xFF;
    send_LMX2491(21,temp);
    temp = (F_start>>8) & 0xFF;
    send_LMX2491(20,temp);
    temp = F_start & 0xFF;
    send_LMX2491(19,temp);
}

// first set up Ramp 0 and Ramp 1
void enable_sweep_mode(void)
{
    send_LMX2491(92,0b00001100); // next ramp = ramp 0, Next trigger: TRIG A - Trig 1, Start flags zero
    send_LMX2491(58,0b00010010); // MOD as clock source, freq. modulation, TRIG1 as Trigger A
    send_LMX2491(58,0b00010011); // enable RAMP
}

void enable_CW_mode(void)
{
    send_LMX2491(58,0); // disable sweep settings - CW mode
}

void change_F_LMX2491(int F)
{
    unsigned char num0, num1, num2 = 0;
    num0 = (F & 0xFF);
    F = F>>8;
    num1 = (F & 0xFF);
    F = F>>8;
    num2 = (F & 0xFF);
    send_LMX2491(21,num2);
    send_LMX2491(20,num1);
    send_LMX2491(19,num0);
}

// INPUT: frequency in HZ 640 MHz - 660 MHz
// OUTPUT: actual frequency in kHZ
// N fixed at 32 - 640 MHz
unsigned int set_CW_frequency_LMX2491(unsigned int frequency)
{
    long long FRAC = 0;

    long long left = (frequency%20000)*1000;
    FRAC = (left*0xFFFFFF)/20000000;
    change_F_LMX2491(FRAC);

    R_FREQ = 640000000+((long long)(FRAC*20000000)/0xFFFFFF);
    return R_FREQ;
}

// print_result 1 = true, 0 = false
// output 0 - OK (LOCKED), 1 - ERROR (UNLOCKED)
char read_lock_status_LMX2491(char print_result)
{
    char output_status = 0;
    if(print_result) usart_send_string("LO1-1:PLL loop - ");
    if(R_MUX)
    {
        if(print_result) usart_send_string("LOCKED\n\r");
    }
    else
    {
        if(print_result) usart_send_string("UN-LOCKED\n\r");
        usart_send_string("ERROR:LO1-1 PLL UN-LOCKED\n\r");
        output_status = 1;
    }
    return output_status;
}
