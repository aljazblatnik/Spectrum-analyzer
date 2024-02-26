#include "stm32g4xx.h"
#include "stm32g441xx.h" // Suggestions

#include "pinout.h"
#include "delay.h"
#include "usart.h"

unsigned int ST1 = 0b100010000000000000000000000; // holds last value
unsigned int ST0 = 0b010011000000000000000101000; // holds last value

extern unsigned int R_FREQ; // in LMX2491.c

void send_STUW81300(unsigned char addr, unsigned int data)
{
    // structure R/W + ADDR (4bit) + DATA (27bit) - 32 bits
    unsigned int data_out = 0;

    LE_LOW;
    data &= 0x07FFFFFF; // ocistimo podatke
    addr &= 0x0F; // ocistimo naslov
    data_out = addr; // nalozimo naslov v izhodni register
    data_out = data_out<<27; // premaknemo naslov 16 bitov levo
    data_out &= 0xF8000000; // preventivno ocistimo izhodne podatke
    data_out |= data; // pripnemo se podatke
    for(int i = 0; i < 32; i++)
    {
        CLK_LOW;
        if(data_out & 0x80000000)   // bit 32
        {
            DATA_HIGH;
        }
        else
        {
            DATA_LOW;
        }
        delay_us(5); // safety delay - Give enough time for data line to stabilize
        CLK_HIGH;
        delay_us(5);
        data_out = data_out<<1;
    }
    CLK_LOW;
    delay_us(5);
    LE_HIGH;
    delay_us(5);
}

unsigned int read_STUW81300(unsigned int addr)
{
    // structure R/W + ADDR (4bit) + DATA (27bit) - 32 bits

    unsigned int data = 0;  // pripravimo za podatke
    addr = addr<<27;     // premaknemo naslov za 16 bitov levo
    addr &= 0x78000000;   // ocistimo naslov +  dodamo bit 32 na 1 za branje
    addr |= 0x80000000;
    LE_LOW;
    for(int i = 0; i < 32; i++)
    {
        CLK_LOW;
        if(addr & 0x80000000)   // bit 24
        {
            DATA_HIGH;
        }
        else
        {
            DATA_LOW;
        }
        delay_us(10);
        CLK_HIGH;
        delay_us(10);
        data = data<<1; // najprej premaknemo
        if(MUX)
        {
            data |= 1; // postavimo LSB bit na ena
        }
        addr = addr<<1;
    }
    CLK_LOW;
    delay_us(5);
    LE_HIGH;
    delay_us(5);
    data &= 0x07FFFFFF; // ocistimo podatke
    return data;
}

void init_STUW81300(void)
{
    send_STUW81300(9, 0); // initialization of ST9 register
    send_STUW81300(8, 2); // REG_4V5 = 3.3V
    send_STUW81300(7, 0b001000000000000000000000010); // LD_SDO - CMOS output mode
    send_STUW81300(0, 0b010011000000000000000101000); // 3mA CP
}

void startup_STUW81300(void)
{
    send_STUW81300(9, 0);
    send_STUW81300(8, 0b000000000000000000000000010); //  3.3V works ok, 4.5V 271 kHZ spurs - check PCB layout
    send_STUW81300(7, 0b001000000000000000000000010); // LD_SDO - CMOS output mode
    send_STUW81300(6, 0b000000000000000000001010001); // bit 12 - 0 disable temp vco calibration - no need in switching frequency operation, CAL_DIV /81
    send_STUW81300(5, 0b000000000000000000000010000); // RF2 low power mode
    send_STUW81300(4, 0b001100000010101001110100110); // 3.3V reg 0b001100000010101001100100101 4V5 0b000000000100001001100000101 -MUTE on unlock ON (bit 7)
    send_STUW81300(3, 0b100000000000110000000000010); // // REF /4 + R/2 - 81,25 MHz REF, double buffering
    send_STUW81300(2, 0b100001111111111111111111111); // RF2 disabled, MAX MOD value, double buffering
    send_STUW81300(1, 0b100010000000000000000000000); // frac 0. N divided by /2, double buffering
    send_STUW81300(0, 0b010011000000000000000101000); // N = 40
}

// N - N register value
void change_N_STUW81300(unsigned int N)
{
    ST0 &= 0b111111111100000000000000000; // clear N value
    N &= 0b11111111111111111; // clear N
    ST0 |= N;
    send_STUW81300(0, ST0);
}

// 0 = integer mode
void change_F_STUW81300(unsigned int F)
{
    ST1 &= 0b111111000000000000000000000; // clear F value
    F &= 0b111111111111111111111; // clear F
    ST1 |= F;
    send_STUW81300(1, ST1);
}

// division = 1 -> N/2, 0 -> N - Fractional part = 0 -> INT mode
void set_N_division_int_mode(char division)
{
    ST1 &= 0b011100000000000000000000000;
    if(division)
    {
        ST1 |= 0b000010000000000000000000000;
    }
    send_STUW81300(1, ST1);
}

// INPUT: frequency in HZ 4200000 kHz - 8300000 kHz
// OUTPUT: actual frequency in kHZ
unsigned int set_CW_frequency_STUW81300(unsigned int frequency)
{
    long long freq_out = 0;
    float R_CLK = (float)R_FREQ/8000;

    int N = frequency/R_CLK;
    int left = (float)frequency - (N*R_CLK);
    int FRAC = ((long long)left*0x1FFFFF)/R_CLK;

    if(frequency>6000000)
    {
        char N_left = N % 2;
        N = N / 2;
        FRAC = (FRAC / 2) + (N_left*0xFFFFF);
        ST1 |= 0b000010000000000000000000000;
        freq_out = (N*2*R_CLK)+((FRAC*2*R_CLK)/0x1FFFFF);
    }
    else
    {
        ST1 &= 0b111101111111111111111111111;
        freq_out = (N*R_CLK)+((FRAC*R_CLK)/0x1FFFFF);
    }

    change_F_STUW81300(FRAC);
    change_N_STUW81300(N);

    return freq_out;
}

// print_result 1 = true, 0 = false
// return 0 - no fault, 1 - fault
char read_status_STUW81300(char print_result)
{
    char output_status = 0; // no error
    int result = read_STUW81300(10);
    if(print_result) usart_send_string("LO1-2:REG 4V5 startup - ");
    if(result & (0b1<<13))
    {
        if(print_result) usart_send_string("OK\n\r");
    }
    else
    {
        if(print_result) usart_send_string("ERROR\n\r");
        output_status = 1;
    }
    if(print_result) usart_send_string("LO1-2:REG VCO startup - ");
    if(result & (0b1<<14))
    {
        if(print_result) usart_send_string("OK\n\r");
    }
    else
    {
        if(print_result) usart_send_string("ERROR\n\r");
        output_status = 1;
    }
    if(print_result) usart_send_string("LO1-2:REG RF  startup - ");
    if(result & (0b1<<15))
    {
        if(print_result) usart_send_string("OK\n\r");
    }
    else
    {
        if(print_result) usart_send_string("ERROR\n\r");
        output_status = 1;
    }
    if(print_result) usart_send_string("LO1-2:REG REF startup - ");
    if(result & (0b1<<16))
    {
        if(print_result) usart_send_string("OK\n\r");
    }
    else
    {
        if(print_result) usart_send_string("ERROR\n\r");
        output_status = 1;
    }
    if(print_result) usart_send_string("LO1-2:REG DIG startup - ");
    if(result & (0b1<<17))
    {
        if(print_result) usart_send_string("OK\n\r");
    }
    else
    {
        if(print_result) usart_send_string("ERROR\n\r");
        output_status = 1;
    }

    if(result & (0b1<<8))
    {
        usart_send_string("ERROR:LO1-2 REG 4V5 over current!\n\r");
        output_status = 1;
    }
    if(result & (0b1<<9))
    {
        usart_send_string("ERROR:LO1-2 REG VCO over current!\n\r");
        output_status = 1;
    }
    if(result & (0b1<<10))
    {
        usart_send_string("ERROR:LO1-2 REG RF  over current!\n\r");
        output_status = 1;
    }
    if(result & (0b1<<11))
    {
        usart_send_string("ERROR:LO1-2 REG REF over current!\n\r");
        output_status = 1;
    }
    if(result & (0b1<<12))
    {
        usart_send_string("ERROR:LO1-2 REG DIG over current!\n\r");
        output_status = 1;
    }
    return output_status;
}

// print_result 1 = true, 0 = false
// output 0 - OK (LOCKED), 1 - ERROR (UNLOCKED)
char read_lock_status_STUW81300(char print_result)
{
    char output_status = 0;
    if(print_result) usart_send_string("LO1-2:PLL loop - ");
    if(MUX)
    {
        if(print_result) usart_send_string("LOCKED\n\r");
    }
    else
    {
        if(print_result) usart_send_string("UN-LOCKED\n\r");
        usart_send_string("ERROR:LO1-2 PLL UN-LOCKED\n\r");
        output_status = 1;
    }
    return output_status;
}
