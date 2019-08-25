#ifndef __SERIAL_H
#define __SERIAL_H

#include "../core.h"

#if defined(SERIAL1)
inline int serial1read ();
inline void serial1write (char c);
#endif
void serialbegin (int address, int baud);
void serialend (int address);
void testescape ();
int gserial ();

#endif
