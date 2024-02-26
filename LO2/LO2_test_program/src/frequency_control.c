#include "stm32g4xx.h"
#include "stm32g441xx.h" // Suggestions
#include "lmx2491.h"
#include "stuW81300.h"
#include "pinout.h"
#include "delay.h"
#include "frequency_control.h"

struct frequencyControlStruct frequencySweep; // global data

// INPUT: frequency in kHZ
// OUTPUT: actual frequency in kHZ
unsigned int set_CW_frequency(unsigned int frequency) {
    unsigned int N, F, freq_out;
    float Fout = (double)frequency/1000;

    if(frequency<6000000) {
        N = Fout/(80+(float)5/16); // get the N value
        F = (Fout-(80*N))*((float)0x1FFFFFF/(5*N));
        freq_out = ((80000*N)+(((long long)5000*N*F)/0x1FFFFFF));
        set_N_division_int_mode(0);
    } else {
        N = Fout/(160+(float)5/8); // get the N value
        F = (Fout-(160*N))*((float)0xFFFFFF/(5*N));
        freq_out = ((160000*N)+(((long long)5000*N*F)/0xFFFFFF));
        set_N_division_int_mode(1);
    }
    change_F_LMX2491(F);
    change_N_STUW81300(N);
    return freq_out;
}

void calculate_F_step(struct frequencyControlStruct *sweepData) {
    unsigned int F_current_max = 0x20000000/sweepData->N_current; // Koliko je F za naslednji N
    if(sweepData->N_current < 76) {
        // < 6.08 GHz
        sweepData->F_current_step = (sweepData->frequency_resolution/((float)625*sweepData->N_current))*(float)0x400000;
    } else {
        // >= 6.08 GHz
        sweepData->F_current_step = (sweepData->frequency_resolution/((float)625*sweepData->N_current))*(float)0x200000;
    }
    // calculate number of steps
    sweepData->F_current_step_count = (F_current_max - sweepData->F_current_start)/sweepData->F_current_step;
    // calculate next F value
    unsigned int F_current_end_left = (F_current_max - sweepData->F_current_start)%sweepData->F_current_step;
    if(sweepData->N_current == 75) {
        sweepData->F_next = ((sweepData->frequency_resolution*0x100000/(float)125)-(375*F_current_end_left/(float)4))/(float)95;
        sweepData->N_next = 38;
        sweepData->N_next_div = 1;
    }
    else if (sweepData->N_current < 52){
        // above 6.08 GHz
        sweepData->F_next = ((sweepData->frequency_resolution/(double)625)*0x200000-(sweepData->N_current*F_current_end_left))/(sweepData->N_current+1);
        sweepData->N_next = sweepData->N_current + 1;
        sweepData->N_next_div = 1;
    }
    else if (sweepData->N_current < 75) {
        // under 6 GHz
        sweepData->F_next = ((sweepData->frequency_resolution/(double)625)*0x400000-(sweepData->N_current*F_current_end_left))/(sweepData->N_current+1);
        sweepData->N_next = sweepData->N_current + 1;
        sweepData->N_next_div = 0;
    }
}

void calculate_start_stop_F_N(struct frequencyControlStruct *sweepData) {
    // start frequency
    float Fout = (double)sweepData->frequency_start/1000;
    if(sweepData->frequency_start < 6000000) {
        sweepData->N_start = Fout/(80+(float)5/16); // get the N value
        sweepData->F_start = (Fout-(80*sweepData->N_start))*((float)0x1FFFFFF/(5*sweepData->N_start));
        sweepData->N_start_div = 0;
    } else {
        sweepData->N_start = Fout/(160+(float)5/8); // get the N value
        sweepData->F_start = (Fout-(160*sweepData->N_start))*((float)0xFFFFFF/(5*sweepData->N_start));
        sweepData->N_start_div = 1;
    }
    // stop frequency
    Fout = (double)sweepData->frequency_stop/1000;
    if(sweepData->frequency_stop < 6000000) {
        sweepData->N_stop = Fout/(80+(float)5/16); // get the N value
        sweepData->F_stop = (Fout-(80*sweepData->N_stop))*((float)0x1FFFFFF/(5*sweepData->N_stop));
    } else {
        sweepData->N_stop = Fout/(160+(float)5/8); // get the N value
        sweepData->F_stop = (Fout-(160*sweepData->N_stop))*((float)0xFFFFFF/(5*sweepData->N_stop));
    }
}

// RBW in kHz
void calculate_frequency_step(struct frequencyControlStruct *sweepData, struct shift_reg_srtuct shiftData) {
    // fixed at this point. Based on RBW setting
    if(shiftData.LC_3M) {
        sweepData->frequency_resolution=1000;
    }
    if(shiftData.LC_1M) {
        sweepData->frequency_resolution=333;
    }
    if(shiftData.LC_300k) {
        sweepData->frequency_resolution=100;
    }
    if(shiftData.LC_100k) {
        sweepData->frequency_resolution=33;
    }
    if(shiftData.LC_30k) {
        sweepData->frequency_resolution=10;
    }
    if(shiftData.Xtal_10k) {
        sweepData->frequency_resolution=3;
    }
    if(shiftData.Xtal_3k) {
        sweepData->frequency_resolution=1;
    }
    if(shiftData.Xtal_1k) {
        sweepData->frequency_resolution=1; // know limitation in current software
    }
}

// input in kHz
void calculate_start_and_stop_frequency(struct frequencyControlStruct *sweepData, int centerFrequency, int span) {
    sweepData->frequency_start = centerFrequency - (span/2);
    sweepData->frequency_stop = sweepData->frequency_start + span;
}

void sweep_init(int centerFrequency, int span, struct shift_reg_srtuct shiftData) {
    calculate_start_and_stop_frequency(&frequencySweep, centerFrequency, span);
    calculate_frequency_step(&frequencySweep, shiftData);
    calculate_start_stop_F_N(&frequencySweep);
    frequencySweep.N_current = frequencySweep.N_start;
    frequencySweep.F_current_start = frequencySweep.F_start;
    calculate_F_step(&frequencySweep);

    // init triggers
    R_MOD_LOW;
    R_TRIG1_LOW;

    setup_ramp_0(frequencySweep.F_current_start,frequencySweep.F_current_step,frequencySweep.F_current_step_count);
    //change_F_LMX2491(frequencySweep.F_current_start);
    while(!R_MUX); // wait for lock
    set_N_division_int_mode(frequencySweep.N_start_div);
    change_N_STUW81300(frequencySweep.N_current);
    while(!MUX);
    enable_sweep_mode();
}

void sweep_single(void) {
    while(frequencySweep.N_current != frequencySweep.N_stop){
        // do blank steps for state machine
        for(int i=0; i<5; i++){
            R_MOD_HIGH;
            delay_us(2); // hitreje kot 1us? Kaj je narobe s kodo?
            R_MOD_LOW;
            delay_us(2);
        }
        while(frequencySweep.F_current_step_count) {
            R_MOD_HIGH;
            delay_us(2);
            R_MOD_LOW;
            while(!R_MUX); // Ref. locked
            while(!MUX); // N locked
            // sample data
            delay_us(10); // test only
            frequencySweep.F_current_step_count--;
        }

        frequencySweep.N_current = frequencySweep.N_next;
        frequencySweep.F_current_start = frequencySweep.F_next;
        calculate_F_step(&frequencySweep);
        setup_ramp_0(frequencySweep.F_current_start,frequencySweep.F_current_step,frequencySweep.F_current_step_count);
        set_N_division_int_mode(frequencySweep.N_next_div);
        change_N_STUW81300(frequencySweep.N_current);
    }
    // do final loop
    calculate_F_step(&frequencySweep);
    setup_ramp_0(frequencySweep.F_current_start,frequencySweep.F_current_step,frequencySweep.F_current_step_count);
    // do blank steps for state machine
    for(int i=0; i<5; i++){
        R_MOD_HIGH;
        delay_us(2); // hitreje kot 1us? Kaj je narobe s kodo?
        R_MOD_LOW;
        delay_us(2);
    }
    change_N_STUW81300(frequencySweep.N_current);
    unsigned int F_temp = frequencySweep.F_current_start;
    while(F_temp <= frequencySweep.F_stop){
        R_MOD_HIGH;
        delay_us(2);
        R_MOD_LOW;
        while(!R_MUX); // Ref. locked
        while(!MUX); // N locked
        // sample data
        delay_us(10); // test only
        F_temp += frequencySweep.F_current_step;
    }
}
