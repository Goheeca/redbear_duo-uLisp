#ifndef __MEMORY_H
#define __MEMORY_H

#include "core.h"

// Make SD card filename

char *MakeFilename (object *arg);
extern const unsigned int Eeprom;

#if defined(sdcardsupport)

void SDWriteInt (File file, int data);
int SDReadInt (File file);

#else

void FlashSetup ();
void FlashWrite16 (unsigned int *addr, uint16_t data);
void FlashWriteInt (unsigned int *addr, int data);
uint16_t FlashRead16 (unsigned int *addr);
int FlashReadInt (unsigned int *addr);

#endif

#endif
