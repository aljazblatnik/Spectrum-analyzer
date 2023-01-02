#include "stm32f0xx.h"
#include "system_stm32f0xx.h"

#include <stdio.h>
#include <stdlib.h>

extern const int program_flash_memory[10][3];
extern int memory[10][3];
extern char status;

void flashUnlock(void){
    // najprej nastavimo latency
    FLASH->ACR |= FLASH_ACR_LATENCY; // vec kot 24 MHz
    while((FLASH->SR & FLASH_SR_BSY) != 0); // cakamo da se vse operacije zakljucijo
    if ((FLASH->CR & FLASH_CR_LOCK) != 0){ // preverimo ce ni odklenjen in ga odklenimo
        FLASH->KEYR = FLASH_FKEY1;
        FLASH->KEYR = FLASH_FKEY2;
    }
}

void flashLock(void){
    FLASH->CR |= FLASH_CR_LOCK; // zaklenemo flash
}

void flashPageErase(int page_addr){
    while((FLASH->SR & FLASH_SR_BSY) != 0); // Pocakajmo da je BSY bit sproscen, operacija ne poteka
    FLASH->CR |= FLASH_CR_PER; // Omogocimo page erase
    FLASH->AR = page_addr; // Vstavimo naslov
    FLASH->CR |= FLASH_CR_STRT; // zacnemo z brisanjem
    __NOP(); // naredimo zakasnitev enega urinega cikla
    while ((FLASH->SR & FLASH_SR_BSY) != 0);
    if(FLASH->SR & FLASH_SR_EOP){
        FLASH->SR = FLASH_SR_EOP; //  Clear EOP flag pisemo 1
    }
    FLASH->CR &= ~FLASH_CR_PER; // ponastavimo nacin za brisanje
}

void flashWrite(uint32_t flash_address, uint32_t flash_data){
    while((FLASH->SR & FLASH_SR_BSY) != 0); // Pocakajmo da je BSY bit sproscen, operacija ne poteka
    FLASH->CR |= FLASH_CR_PG; // Omogocimo programiranje
    // 1/2
    *(__IO uint16_t*)(flash_address) = (uint16_t)flash_data; // pisemo half-word na zeljen naslov
    __NOP(); // naredimo zakasnitev enega urinega cikla
    while((FLASH->SR & FLASH_SR_BSY) != 0); // Pocakajmo da je BSY bit sproscen
    if(FLASH->SR & FLASH_SR_EOP){
        FLASH->SR = FLASH_SR_EOP; //  Clear EOP flag pisemo 1
    }
    // 2/2
    int temp = flash_address + 2;
    while((FLASH->SR & FLASH_SR_BSY) != 0); // Pocakajmo da je BSY bit sproscen, operacija ne pot
    *(__IO uint16_t*)(temp) = (uint16_t)(flash_data >> 16); // pisemo half-word na zeljen naslov
    __NOP(); // naredimo zakasnitev enega urinega cikla
    while((FLASH->SR & FLASH_SR_BSY) != 0); // Pocakajmo da je BSY bit sproscen
    while ((FLASH->SR & FLASH_SR_EOP) == 0); // cakamo da se operacija zakljuci
    if(FLASH->SR & FLASH_SR_EOP){
        FLASH->SR = FLASH_SR_EOP; //  Clear EOP flag pisemo 1
    }
    FLASH->CR &= ~FLASH_CR_PG; // onemogocimo programiranje
}

char flashWriteChanges(void){
    // Spravimo podatke v flash vendar samo, ce se podatki razlikujejo
    // S tem preprecimo nepotrebno pisanje po flashu, ki degradira zivljensko dobo
    int i, j;
    int changes = 0;
    for(i = 0; i < 10; i++){
        for(j = 0; j < 3; j++){
            if(memory[i][j] != program_flash_memory[i][j]) changes++;
        }
    }
    if(changes == 0){
        return 0; // ni sprememb
    }

    // zapisemo spremembe
    flashUnlock(); //odklenemo flash
    flashPageErase(0x08007C00); // izbrisemo celoten page

    //const int *first_flash_address = &(program_flash_memory[0][0]); // pointer se lahko premika
    int first_flash_address = 0x08007C00;
    int *data = &(memory[0][0]); // ustvarimo kazalec na spomin

    for(i = 0; i < (sizeof(memory)/sizeof(memory[0][0])); i++, data++){
        flashWrite(first_flash_address, *data);
        first_flash_address += 4;
    }

    flashLock();
    return 1;
}

void flashCopyFromFlashToRAM(void){
    // funkcija kopira vrednosti iz flasha v spomin
    int i,j;
    for(i = 0; i < 10; i++){
        for(j = 0; j < 3; j++){
            memory[i][j] = program_flash_memory[i][j];
        }
    }
}
