#ifndef MAX2870_H_INCLUDED
#define MAX2870_H_INCLUDED

void MAX2870_init(void);
void send_MAX2870(unsigned char addr, unsigned int data);
unsigned int read_MAX2870(void);
int MAX2870_set_frequency(int frequency);

#endif /* MAX2870_H_INCLUDED */
