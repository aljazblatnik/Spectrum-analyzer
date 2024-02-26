#ifndef SHIFT_REG_H_INCLUDED
#define SHIFT_REG_H_INCLUDED

void init_shift_reg(void);
void LEDShiftRegister(unsigned char led_data);
void sendDataToShiftRegisters(void);

#endif /* SHIFT_REG_H_INCLUDED */
