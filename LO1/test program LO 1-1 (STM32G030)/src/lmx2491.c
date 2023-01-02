#include "stm32g0xx.h"
#include "stm32g0b0xx.h" // used for auto complete function of editor only (included in "stm32g0xx.h"

#include "pinout.h"
#include "delay.h"

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
        if(data & 0x00800000)  // gledamo bit 24
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
}

unsigned int read_LMX2491(unsigned int addr)
{
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
        if(addr & 0x00800000)  // gledamo bit 24
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
    send_LMX2491(39,58); // MUX_out - pull up // pull down output
    send_LMX2491(34,0b10100000); // fpd < 30 MHz
    send_LMX2491(27,0b00001000); // single ended drive, x2 dubbler off - no cycle slip reduction
    send_LMX2491(28,0b00011111); // CP current 3100uA // positive CP polarity
    send_LMX2491(25,2); // R/2

    send_LMX2491(24,0xFF); // max denominator
    send_LMX2491(23,0xFF); // max denominator
    send_LMX2491(22,0xFF); // max denominator
    send_LMX2491(21,0x2F); // numerator = 0
    send_LMX2491(20,0xF2); // numerator = 0
    send_LMX2491(19,0x2F); // numerator = 0
    send_LMX2491(18,0b00110000); // FRAC ORDER: 3, FRAC DITHER: weak, PLL_N[17:16] - in N only, you have to switch to Int mode only 0b00001100 - int mode best performance
    send_LMX2491(16,33); // PLL_N - START f 650 MHz
}

void startup_LMX2491(void)
{
    // ramping function
    // first set starting frequency of 651 MHz
    send_LMX2491(24,0xFF); // max denominator
    send_LMX2491(23,0xFF); // max denominator
    send_LMX2491(22,0xFF); // max denominator: 16777216
    send_LMX2491(21,0x19); //
    send_LMX2491(20,0x99); //
    send_LMX2491(19,0x9A); // NUM: 1677722

    // setting ramp function
    // ramp 1mS - 1MHz - 168 inc - cca 100 HZ
    send_LMX2491(92,0x04); // next ramp
    send_LMX2491(91,0x27); // LEN
    send_LMX2491(90,0x10);
    send_LMX2491(89,0x00);
    send_LMX2491(88,0x00);
    send_LMX2491(87,0x00);
    send_LMX2491(86,0xA8); // increment
    send_LMX2491(85,0x00);
    send_LMX2491(84,0x00);
    send_LMX2491(83,0x00);
    send_LMX2491(82,0x01);
    send_LMX2491(81,0x00);
    send_LMX2491(80,0x00);
    send_LMX2491(79,0x00);
    send_LMX2491(78,0x00);
    send_LMX2491(77,0x00);
    send_LMX2491(76,0x00);
    send_LMX2491(75,0x00);
    send_LMX2491(70,0x03); // samo comp vrednosti na 1

    send_LMX2491(58,0x01); // enable ramp

}

int LMX2491_set_frequency(int frequency)
{
    int N = 0;
    long long FRAC = 0;
    int freq_out = 0;

    N = frequency/10000;
    long long left = frequency%10000;
    FRAC = (left*0xFFFFFF)/10000;

    freq_out = (10000*N)+((FRAC*10000)/0xFFFFFF);

    unsigned char num0, num1, num2 = 0;
    if(left!=0)
    {
        num0 = FRAC & 0xFF;
        num1 = (FRAC>>8) & 0xFF;
        num2 = (FRAC>>16) & 0xFF;
    }

    unsigned char N1 = N & 0xFF;
    unsigned char N2 = (N>>8) & 0xFF;

    send_LMX2491(21,num2);
    send_LMX2491(20,num1);
    send_LMX2491(19,num0);
    send_LMX2491(17,N2);
    send_LMX2491(16,N1);

    return freq_out;
}
