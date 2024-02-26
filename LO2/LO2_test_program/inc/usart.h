#ifndef USART_H_INCLUDED
#define USART_H_INCLUDED

#define usart_buffer 40


void init_usart(void);

void usart_send_string(char *str);
void usart_send_byte(unsigned char byte);

int command_decode(char *str);
int compare_string(char *first, char *second);
int string_to_int(char *str, int length);


#endif /* USART_H_INCLUDED */
