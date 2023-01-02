#include "stm32f0xx.h"
#include "system_stm32f0xx.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "delay.h"
#include "usart.h"
#include "attenuator.h"

extern int buff_index;


void usart_init(void){
    RCC->APB2ENR |= (RCC_APB2ENR_USART1EN); // clock

    //USART1->BRR = 0x340; // 115.25 KBps
    USART1->BRR = 0x2710; // 9600 bits/s
    USART1->CR1 |= USART_CR1_OVER8;
    USART1->CR1 |= USART_CR1_RE; // receive enable
    USART1->CR1 |= USART_CR1_TE; // transmit enable
    //USART1->CR1 |= USART_CR1_RTOIE; // timeout interrupt enable
    //USART1->CR2 |= USART_CR2_RTOEN; // Receiver timeout enable
    USART1->RTOR = 0xFFFFFF; // timeout value
    USART1->CR1 |= USART_CR1_UE; // usart enable

    // receive interrupt setup
    USART1->CR1 |= USART_CR1_RXNEIE;

    NVIC_EnableIRQ(USART1_IRQn);
    NVIC_SetPriority(USART1_IRQn,2);   //enable interrupt

    /* kako uporabljati prekinitvene funkcije:
    (USART1->ISR & USART_ISR_RXNE) // data ready to be read
    USART1->RDR; // receive register
    USART1->TDR; // transmit register
    */
}

void usart_send_string(char *str){
    int i = 0;
    while(str[i] != '\0'){
        USART1->TDR = str[i] ;
        i++;
        while(!(USART1->ISR & USART_ISR_TC));
    }
}

void usart_send_byte(unsigned char byte){
    USART1->TDR = byte;
    while(!(USART1->ISR & USART_ISR_TC));
}

char* int_to_dec_string(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    // negative number
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}


int string_to_int(char *str, int length){
    // dolzina je stevilo znakov, ki jih vzame v obdelavo, vkljucno s predznakom!
    unsigned int result = 0;
    char const digit[] = "0123456789";
    int i = 0;
    int sign = 1;
    if(str[0] == '-'){
        sign = -1;
        length--;
        i = 1;
    }
    else if(str[0] == '+'){
        sign = 1;
        length--;
        i = 1;
    }
    while(length){
        int n;
        for(n=0; digit[n]; n++){
            if(digit[n] == str[i]){
                break;
            }
        }
        result *= 10;
        result += n;
        i++;
        length--;
    }
    result *= sign;
    return result;
}

int compare_string(char *first, char *second){ // substring
    while (*first == *second) {
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

int command_decode(char *str){
    if(buff_index == 0){
        usart_send_string("ARM> ");
        return 0;
    }
    if(compare_string(str, "help") == 0){
        // lahko posljemo seznam ukazov
        usart_send_string("A xxx - amplitude [+/-dBm]\r\nF xxxxxxx - frequency [kHz]\r\nM xxxxx - modulation [Hz]\r\nP 123456789 - save settings\r\nR - reset, S x - memory\r\n\r\n");
    }
    else if(compare_string(str, "Att ") == 0){
        int result = string_to_int(&str[4],3);
        attenuator_send(result*4); // posljemo samo nastavitev slabilcu
        usart_send_string("OK\r\n");
        buff_index = 0;
        return 0; // ne posodabljamo menuja
    }
    else{
        usart_send_string("Unrecognized command. Try help.\r\n");
        buff_index = 0;
        return 0;
    }
    buff_index = 0;
    return 1;
}
