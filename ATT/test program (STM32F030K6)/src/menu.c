#include "stm32f0xx.h"
#include "system_stm32f0xx.h"

#include "pinout.h"
#include "delay.h"
#include "usart.h"
#include "display.h"
#include "calibration.h"
#include "modulation.h"
#include "flash.h"

#include "max2871.h"
#include "attenuator.h"

extern int memory[10][3];

extern int cursor;
extern  int program;
extern  char status;
extern  int frequency; // kHz
extern  int frequency_output;
extern int frequency_flash;
extern int power; // dBm
extern  int modulation; // Hz

void menuLoadDataFromMemoryToWorkingRAM(int program){
    frequency = memory[program][0]; // 0 - frekvenca
    power = memory[program][1]; // 1 - moc
    modulation = memory[program][2]; // 2 - modulacija
}

void menuSaveDataFromWorkingRAMToMemory(int program){
    memory[program][0] = frequency; // 0 - frekvenca
    memory[program][1] = power; // 1 - moc
    memory[program][2] = modulation; // 2 - modulacija
}

void menuShow(void){
    lcdPositionXY(0,0);
    lcdWriteChar((unsigned char)(0x30 + program));
    lcdWriteChar(' ');
    lcdWriteChar(' ');
    // preskocimo polje za status znak - pisemo v drugi zanki
    lcdPositionXY(4,0);
    lcdWriteChar(' ');
    // frekvenca
    int temp = frequency;
    char digit = 0x30;
    while((temp-1000000)>=0){
        digit++;
        temp = temp-1000000;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-100000)>=0){
        digit++;
        temp = temp-100000;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-10000)>=0){
        digit++;
        temp = temp-10000;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-1000)>=0){
        digit++;
        temp = temp-1000;
    }
    lcdWriteChar(digit);
    lcdWriteChar('.');
    digit = 0x30;
    while((temp-100)>=0){
        digit++;
        temp = temp-100;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-10)>=0){
        digit++;
        temp = temp-10;
    }
    lcdWriteChar(digit);
    digit = 0x30 + temp;
    lcdWriteChar(digit);
    lcdWriteString("MHz\n");

    // moc
    if(!(power/10)){
       // ce ni desetic premaknemo za eno mesto v desno
        lcdWriteChar(' ');
    }
    if(power>=0){
        lcdWriteChar('+');
        temp = power;
    }
    else{
        lcdWriteChar('-');
        temp = power + (-2*power);
    }
    digit = 0x30;
    while((temp-10)>=0){
        digit++;
        temp = temp-10;
    }
    if(digit != 0x30) lcdWriteChar(digit); // pisemo samo ce so desetice
    digit = 0x30 + temp;
    lcdWriteChar(digit);
    lcdWriteString("dBm ");

    // modulacija
    temp = modulation;
    if(!modulation){
        lcdWriteString("SW.OFF");
    }
    else{
        digit = 0x30;
        while((temp-10000)>=0){
            digit++;
            temp = temp-10000;
        }
        lcdWriteChar(digit);
        digit = 0x30;
        while((temp-1000)>=0){
            digit++;
            temp = temp-1000;
        }
        lcdWriteChar(digit);
        lcdWriteChar('.');
        digit = 0x30;
        while((temp-100)>=0){
            digit++;
            temp = temp-100;
        }
        lcdWriteChar(digit);
        digit = 0x30;
        while((temp-10)>=0){
            digit++;
            temp = temp-10;
        }
        lcdWriteChar(digit);
        digit = 0x30 + temp;
        lcdWriteChar(digit);
    }
    if(modulation){
         lcdWriteString("kHz");
    }
    else{
         lcdWriteString("   ");
    }
    lcdPositionXY(cursor%16,cursor/16);

    // Posodobi Cipe
    frequency_output = MAX2871_set_frequency(frequency);
    attenuator_send(calibrationReturnAttenuation(frequency,power));
    modulationSet(modulation);

    menuSaveDataFromWorkingRAMToMemory(program); // ko smo vse uspesno izvedli, shranimo se v RAM spomin

    frequency_flash = -200;  // 2s kazemo nastavljeno vrednost
}

void menuShowStatus(char status){
    lcdPositionXY(3,0);
    lcdWriteChar(status);
    lcdPositionXY(cursor%16,cursor/16);
}

void menuShowFrequency(int new_frequency){
    // zamenjamo samo frekvenco
    lcdPositionXY(5,0);
    // frekvenca
    int temp = new_frequency;
    char digit = 0x30;
    while((temp-1000000)>=0){
        digit++;
        temp = temp-1000000;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-100000)>=0){
        digit++;
        temp = temp-100000;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-10000)>=0){
        digit++;
        temp = temp-10000;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-1000)>=0){
        digit++;
        temp = temp-1000;
    }
    lcdWriteChar(digit);
    lcdWriteChar('.');
    digit = 0x30;
    while((temp-100)>=0){
        digit++;
        temp = temp-100;
    }
    lcdWriteChar(digit);
    digit = 0x30;
    while((temp-10)>=0){
        digit++;
        temp = temp-10;
    }
    lcdWriteChar(digit);
    digit = 0x30 + temp;
    lcdWriteChar(digit);
    // vrnemo nazaj cursor
    lcdPositionXY(cursor%16,cursor/16);
}

void moveCursorLR(){
    int cursor_temp = cursor; // spravimo si vrednost kurzorja, ce jo bomo potrebovali
    if(!bRIGHT){
        cursor++;
        if(cursor>0 && cursor<5){
            cursor = 5; //frekvenca
        }
        if(cursor == 9){
            cursor = 10;
        }
        if(cursor>12 && cursor<17){
            cursor = 17; // moc
        }
        if(cursor>18 && cursor<23){
            cursor = 23; // modulacija
        }
        if(cursor == 25){
            cursor = 26;
        }
        if(cursor>28){
            cursor = 28;
        }
        lcdPositionXY(cursor%16,cursor/16); // pisemo takoj, da obvestimo uporabnika
        int timer = 200;
        while(!bRIGHT && timer){
            delay_ms(1); // 1ms zakasnitve
            if(!bLEFT){
                // uporabnik drzi obe tipki
                cursor = cursor_temp; // damo kurzor nazaj na prvotno vrednost
                lcdPositionXY(0,0); // damo kurzor na zacetek
                lcdWriteString("** SHRANJUJEM **\n");
                if(flashWriteChanges()){ // shranimo spremembe v flash
                    lcdWriteString("Prosim pocakajte");
                }
                else{
                    lcdWriteString(" Brez sprememb  ");
                }
                delay_ms(3000);
                lcdDisplayClear(); // pocistimo zaslon
                timer = 0;
                menuShow(); // pokazemo nazaj menu
                while(!bLEFT || !bRIGHT); // cakamo da uporabnik spusti tipko
            }
            timer--;
        } // cakamo da uporabnik spusti tipko
        delay_ms(10); // de-bounce
    }
    if(!bLEFT){
        cursor--;
        if(cursor<0){
            cursor = 0;
        }
        if(cursor>0 && cursor<5){
            cursor = 0; // save
        }
        if(cursor == 9){
            cursor = 8;
        }
        if(cursor>12 && cursor<17){
            cursor = 12; // frekvenca
        }
        if(cursor>18 && cursor<23){
            cursor = 18; // modulacija
        }
        if(cursor == 25){
            cursor = 24;
        }
        lcdPositionXY(cursor%16,cursor/16);
        int timer = 200;
        while(!bLEFT && timer){
            delay_ms(1); // 1ms zakasnitve
            if(!bRIGHT){
                // uporabnik drzi obe tipki
                cursor = cursor_temp; // damo kurzor nazaj na prvotno vrednost
                lcdPositionXY(0,0); // damo kurzor na zacetek
                lcdWriteString("** SHRANJUJEM **\n");
                if(flashWriteChanges()){ // shranimo spremembe v flash
                    lcdWriteString("Prosim pocakajte");
                }
                else{
                    lcdWriteString(" Brez sprememb  ");
                }
                delay_ms(3000);
                lcdDisplayClear(); // pocistimo zaslon
                timer = 0;
                menuShow(); // pokazemo nazaj menu
                while(!bLEFT || !bRIGHT); // cakamo da uporabnik spusti tipko
            }
            timer--;
        } // cakamo da uporabnik spusti tipko
        delay_ms(10); // de-bounce
    }
}

void moveCursorUD(void){
    // gumb gor
    if(!bUP){
        switch(cursor){
        case 0:
            program++;
            if(program>9){
                program = 9;
            }
            menuLoadDataFromMemoryToWorkingRAM(program); // sprememba programa, nalozimo vrednosti iz RAM-a
            break;
        case 5:
            frequency += 1000000;
            break;
        case 6:
            frequency += 100000;
            break;
        case 7:
            frequency += 10000;
            break;
        case 8:
            frequency += 1000;
            break;
        case 10:
            frequency += 100;
            break;
        case 11:
            frequency += 10;
            break;
        case 12:
            frequency += 1;
            break;
        case 17:
            power += 10;
            break;
        case 18:
            power += 1;
            break;
        case 23:
            modulation +=10000;
            break;
        case 24:
            modulation +=1000;
            break;
        case 26:
            modulation +=100;
            break;
        case 27:
            modulation +=10;
            break;
        case 28:
            modulation +=1;
            break;
        }
        if(frequency>7000000){
            frequency = 7000000;
        }

        power = calibrationReturnPower(frequency,power);

        if(modulation>50000){
            modulation = 50000;
        }
        //pokazimo spremembe in pocakajmo na gumb
        menuShow();
        usart_send_report();
        while(!bUP);
        delay_ms(10);
    }
    // gumb dol
    if(!bDOWN){
        switch(cursor){
        case 0:
            program--;
            if(program<0){
                program = 0;
            }
            menuLoadDataFromMemoryToWorkingRAM(program); // sprememba programa, nalozimo vrednosti iz RAM-a
            break;
        case 5:
            frequency -= 1000000;
            break;
        case 6:
            frequency -= 100000;
            break;
        case 7:
            frequency -= 10000;
            break;
        case 8:
            frequency -= 1000;
            break;
        case 10:
            frequency -= 100;
            break;
        case 11:
            frequency -= 10;
            break;
        case 12:
            frequency -= 1;
            break;
        case 17:
            power -= 10;
            break;
        case 18:
            power -= 1;
            break;
        case 23:
            modulation -=10000;
            break;
        case 24:
            modulation -=1000;
            break;
        case 26:
            modulation -=100;
            break;
        case 27:
            modulation -=10;
            break;
        case 28:
            modulation -=1;
            break;
        }
        if(frequency<0){
            frequency = 0;
        }

        power = calibrationReturnPower(frequency,power);

        if(modulation<0){
            modulation = 0;
        }
        //pokazimo spremembe in pocakajmo na gumb
        menuShow();
        usart_send_report();
        while(!bDOWN);
        delay_ms(10);
    }
}
