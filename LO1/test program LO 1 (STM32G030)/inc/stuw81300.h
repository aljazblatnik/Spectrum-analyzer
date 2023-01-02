#ifndef STUW81300_H_INCLUDED
#define STUW81300_H_INCLUDED

void send_STUW81300(unsigned char addr, unsigned int data);
unsigned int read_STUW81300(unsigned int addr);
void init_STUW81300(void);
void startup_STUW81300(void);
void test_STUW81300(void);
void change_freq_STUW81300(unsigned char N);

#endif /* STUW81300_H_INCLUDED */
