#ifndef LMX2491_H_INCLUDED
#define LMX2491_H_INCLUDED

void send_LMX2491(unsigned char addr, unsigned int data);
unsigned int read_LMX2491(unsigned int addr);
void init_LMX2491(void);
void change_F_LMX2491(int F);
unsigned int set_CW_frequency_LMX2491(unsigned int frequency);
char read_lock_status_LMX2491(char print_result);

void enable_CW_mode(void);
void enable_sweep_mode(void);
void setup_ramp_0(unsigned int F_start, unsigned int F_step, unsigned int ramp_length);

#endif /* LMX2491_H_INCLUDED */
