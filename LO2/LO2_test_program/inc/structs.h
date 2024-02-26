#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED

struct frequencyControlStruct {
    unsigned int frequency_start; // in kHz
    unsigned int frequency_stop; // in kHz
    unsigned int frequency_resolution; // in kHz
    unsigned int N_start;
    unsigned int N_start_div;
    unsigned int N_stop;
    unsigned int N_current;
    unsigned int N_next;
    unsigned int N_next_div;
    unsigned int F_start;
    unsigned int F_stop;
    unsigned int F_current_start;
    unsigned int F_current_step;
    unsigned int F_current_step_count;
    unsigned int F_next;
};

struct shift_reg_srtuct {
    unsigned char PLL_SA_CLK;
    unsigned char PLL_SA_DATA;
    unsigned char PLL_SA_LE;
    unsigned char PLL_SA_RF_EN;

    unsigned char PLL_TG_CLK;
    unsigned char PLL_TG_DATA;
    unsigned char PLL_TG_LE;
    unsigned char PLL_TG_RF_EN;

    unsigned char Xtal_ON;
    unsigned char Xtal_10k;
    unsigned char Xtal_3k;
    unsigned char Xtal_1k;

    unsigned char LC_3M;
    unsigned char LC_1M;
    unsigned char LC_300k;
    unsigned char LC_100k;
    unsigned char LC_30k;

    unsigned char Att_SA_CLK;
    unsigned char Att_SA_DATA;
    unsigned char Att_SA_LE;

    unsigned char Att_TG_CLK;
    unsigned char Att_TG_DATA;
    unsigned char Att_TG_LE;

    unsigned char Register1;
    unsigned char Register2;
    unsigned char Register3;
    unsigned char Register4;

    unsigned char LED_reg2;
    unsigned char LED_reg3;
    unsigned char LED_reg4;

    unsigned char LE1;
    unsigned char LE2;
};

#endif /* STRUCTS_H_INCLUDED */
