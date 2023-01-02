#include "stm32g0xx.h"
#include "stm32g0b0xx.h" // used for auto complete function of editor only (included in "stm32g0xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "usart.h"
#include "max2870.h"

extern int buff_index;
extern char buffer[usart_buffer]; // defined in .h file
extern int decode_command;

void init_usart(void)
{
    RCC->APBENR2 |= RCC_APBENR2_USART1EN; // clock

    USART1->BRR = 0x1388; // 9600 bits/s
    USART1->CR1 |= USART_CR1_RE; // receive enable
    USART1->CR1 |= USART_CR1_TE; // transmit enable
    USART1->RTOR = 0xFFFFFF; // timeout value - timeout not used
    USART1->CR1 |= USART_CR1_UE; // usart enable

    // receive interrupt setup
    USART1->CR1 |= USART_CR1_RXNEIE_RXFNEIE;

    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn,2);   //enable interrupt
}

void usart_send_string(char *str)
{
    int i = 0;
    while(str[i] != '\0')
    {
        USART1->TDR = str[i] ;
        i++;
        while(!(USART1->ISR & USART_ISR_TC));
    }
}

void usart_send_byte(unsigned char byte)
{
    USART1->TDR = byte;
    while(!(USART1->ISR & USART_ISR_TC));
}

char* int_to_dec_string(int i, char b[])
{
    char const digit[] = "0123456789";
    char* p = b;
    // negative number
    if(i<0)
    {
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do   //Move to where representation ends
    {
        ++p;
        shifter = shifter/10;
    }
    while(shifter);
    *p = '\0';
    do   //Move back, inserting digits as u go
    {
        *--p = digit[i%10];
        i = i/10;
    }
    while(i);
    return b;
}

int string_to_int(char *str, int length)
{
    // dolzina je stevilo znakov, ki jih vzame v obdelavo, vkljucno s predznakom!
    unsigned int result = 0;
    char const digit[] = "0123456789";
    int i = 0;
    int sign = 1;
    if(str[0] == '-')
    {
        sign = -1;
        length--;
        i = 1;
    }
    else if(str[0] == '+')
    {
        sign = 1;
        length--;
        i = 1;
    }
    while(length)
    {
        int n;
        for(n=0; digit[n]; n++)
        {
            if(digit[n] == str[i])
            {
                break;
            }
        }
        result *= 10;
        result += n;
        i++;
        length--;
        if(str[i] == '\0')
        {
            break;
        }
    }
    result *= sign;
    return result;
}

int register_bin_to_int(char *str)
{
    // podatek brez markerja 0b!
    unsigned int result = 0;
    int i = 0;
    while(str[i] != '\0')
    {
        int n = 0;
        if(str[i] == '1')
        {
            n = 1;
        }
        result *= 2;
        result += n;
        i++;
    }
    return result;
}

int register_hex_to_int(char *str)
{
    // podatek brez markerja 0b!
    unsigned int result = 0;
    char const digit[] = "0123456789ABCDEF";
    int i = 0;
    while(str[i] != '\0')
    {
        int n;
        for(n=0; digit[n]; n++)
        {
            if(digit[n] == str[i])
            {
                break;
            }
        }
        result *= 16;
        result += n;
        i++;
    }
    return result;
}

int compare_string(char *first, char *second)   // substring
{
    while (*first == *second)
    {
        if (*first == '\0' || *second == '\0')
            break;
        first++;
        second++;
    }

    if (*second == '\0')
        return 0;
    else
        return -1;
}

char* itoa(int value, char* result, int base)
{
    // check that the base if valid
    if (base < 2 || base > 36)
    {
        *result = '\0';
        return result;
    }

    char* ptr = result, *ptr1 = result, tmp_char;
    int tmp_value;

    do
    {
        tmp_value = value;
        value /= base;
        *ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz" [35 + (tmp_value - value * base)];
    }
    while ( value );

    // Apply negative sign
    if (tmp_value < 0) *ptr++ = '-';
    *ptr-- = '\0';
    while(ptr1 < ptr)
    {
        tmp_char = *ptr;
        *ptr--= *ptr1;
        *ptr1++ = tmp_char;
    }
    return result;
}

char* int_to_hex_string(unsigned int i, char b[])
{
    char const digit[] = "0123456789ABCDEF";
    char* p = b;
    unsigned int shifter = i;
    do   //Move to where representation ends
    {
        ++p;
        shifter = shifter/16;
    }
    while(shifter);
    *p = '\0';
    do   //Move back, inserting digits as u go
    {
        *--p = digit[i%16];
        i = i/16;
    }
    while(i);
    return b;
}

char* int_to_int_string(unsigned int i, char b[])
{
    char const digit[] = "0123456789";
    char* p = b;
    /*
    // negative number
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    */
    if(i == 0)
    {
        *p = '0';
        p++;
        *p = '\0';
        return b;
    }
    int shifter = i;
    do   //Move to where representation ends
    {
        ++p;
        shifter = shifter/10;
    }
    while(shifter);
    *p = '\0';
    do   //Move back, inserting digits as u go
    {
        *--p = digit[i%10];
        i = i/10;
    }
    while(i);
    return b;
}

char* int_to_bin_string(unsigned int i, char b[])
{
    char const digit[] = "01";
    char* p = b;
    /*
    // negative number
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    */
    int shifter = i;
    do   //Move to where representation ends
    {
        ++p;
        shifter = shifter/2;
    }
    while(shifter);
    *p = '\0';
    do   //Move back, inserting digits as u go
    {
        *--p = digit[i%2];
        i = i/2;
    }
    while(i);
    return b;
}

int command_decode(char *str)
{
    if(buff_index == 0)
    {
        // no command entered, return empty console
        usart_send_string("\r\nARM > ");
        return 1;
    }
    else
    {

        if(compare_string(buffer, "help") == 0)
        {
            usart_send_string("\r\nNo help.");
        }
        else if(compare_string(buffer, "R -all") == 0)
        {
            char str[34];
            int a = 141;
            int b = 0;
            while(b <= a)
            {
                //int result = read_LMX2491(b);
                int result = 0;
                int_to_hex_string(b, str);
                usart_send_string("\r\nR");
                usart_send_string(str);
                int_to_hex_string(result, str);
                usart_send_string(": 0x");
                usart_send_string(str);
                int_to_bin_string(result, str);
                usart_send_string(" - 0b");
                usart_send_string(str);
                b++;
            }
        }
        else if(compare_string(buffer, "R") == 0)
        {
            // get register number
            int a = 1;
            char str[12];
            while(buffer[a] != ' ')
            {
                str[a-1] = buffer[a];
                a++;
            }
            str[a-1] = '\0';
            unsigned int register_num = string_to_int(str, 4);
            if(compare_string(&buffer[a+1], "?") == 0)
            {
                // get data
                char str_a[34];
                //unsigned int result = read_LMX2491(register_num);
                unsigned int result = 0;
                int_to_hex_string(register_num, str_a);
                usart_send_string("\r\nR");
                usart_send_string(str_a);
                int_to_hex_string(result, str_a);
                usart_send_string(": 0x");
                usart_send_string(str_a);
                int_to_bin_string(result, str_a);
                usart_send_string(" - 0b");
                usart_send_string(str_a);
            }
            else
            {
                // send data
                unsigned int register_data;
                if(compare_string(&buffer[a+2], "b") == 0)
                {
                    register_data = register_bin_to_int(&buffer[a+3]);
                    usart_send_string("\r\nBin - working ...");
                    char str_a[17];
                    int_to_bin_string(register_data, str_a);
                    usart_send_string(" Decoded: 0b");
                    usart_send_string(str_a);
                    int_to_hex_string(register_num, str_a);
                    usart_send_string(" R");
                    usart_send_string(str_a);
                }
                else if(compare_string(&buffer[a+2], "x") == 0)
                {
                    register_data = register_hex_to_int(&buffer[a+3]);
                    usart_send_string("\r\nHex - working ...");
                }
                else
                {
                    register_data = string_to_int(&buffer[a+1], 5);
                    usart_send_string("\r\nDec - working ...");
                }
                if(register_data > 0xFF)
                {
                    usart_send_string("\r\nErr - Input number too large!");
                }
                else
                {
                    //send_LMX2491(register_num,register_data);
                    usart_send_string("\r\nSending - OK");
                }
            }
        }
        else if(compare_string(buffer, "F") == 0)
        {
            // get frequency - max 800000 kHz min 400000 kHz : Format F xxxxxx
            int a = 1;
            while(buffer[a] != ' ')
            {
                str[a-1] = buffer[a];
                a++;
            }
            //LMX2491_set_frequency(string_to_int(&buffer[a+1], 8));
        }
        else
        {
            usart_send_string("\r\nUnrecognized command. Try help.");
        }
        buff_index = 0;
        usart_send_string("\r\nARM > ");
    }
    return 0;
}
