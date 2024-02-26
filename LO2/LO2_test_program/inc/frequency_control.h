#ifndef FREQUENCY_CONTROL_H_INCLUDED
#define FREQUENCY_CONTROL_H_INCLUDED

#include "structs.h"

unsigned int set_CW_frequency(unsigned int frequency);
void calculate_F_step(struct frequencyControlStruct *sweepData);
void calculate_start_stop_F_N(struct frequencyControlStruct *sweepData);
void calculate_frequency_step(struct frequencyControlStruct *sweepData, struct shift_reg_srtuct shiftData);
void calculate_start_and_stop_frequency(struct frequencyControlStruct *sweepData, int centerFrequency, int span);
void sweep_init(int centerFrequency, int span, struct shift_reg_srtuct shiftData);
void sweep_single(void);

#endif /* FREQUENCY_CONTROL_H_INCLUDED */
