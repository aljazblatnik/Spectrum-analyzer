#ifndef LMX2491_H_INCLUDED
#define LMX2491_H_INCLUDED

void send_LMX2491(unsigned char addr, unsigned int data);
unsigned int read_LMX2491(unsigned int addr);
void init_LMX2491(void);
void startup_LMX2491(void);
int LMX2491_set_frequency(int frequency);

#endif /* LMX2491_H_INCLUDED */
