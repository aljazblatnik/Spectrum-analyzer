#ifndef STUW81300_H_INCLUDED
#define STUW81300_H_INCLUDED

void send_STUW81300(unsigned char addr, unsigned int data);
unsigned int read_STUW81300(unsigned int addr);
void init_STUW81300(void);
void startup_STUW81300(void);
void change_N_STUW81300(unsigned char N);
void change_F_STUW81300(int F);
void set_N_division_int_mode(char division);
unsigned int set_CW_frequency_STUW81300(unsigned int frequency);
char read_status_STUW81300(char print_result);
char read_lock_status_STUW81300(char print_result);

#endif /* STUW81300_H_INCLUDED */
