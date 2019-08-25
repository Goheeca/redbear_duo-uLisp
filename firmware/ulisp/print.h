#ifndef __PRINT_H
#define __PRINT_H

#include "core.h"
#include "hw/platform.h"

// Print functions

void pserial (char c);

extern const char ControlCodes[] PROGMEM;

void pcharacter (char c, pfun_t pfun);
void pstring (char *s, pfun_t pfun);
void printstring (object *form, pfun_t pfun);
void pfstring (const char *s, pfun_t pfun);
void pint (int i, pfun_t pfun);
void pmantissa (float f, pfun_t pfun);
void pfloat (float f, pfun_t pfun);
inline void pln (pfun_t pfun) {
  pfun('\n');
}
void pfl (pfun_t pfun);
void printobject (object *form, pfun_t pfun);

#endif
