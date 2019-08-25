#include "serial.h"
#include <Particle.h>

#if defined(SERIAL1)

inline int serial1read () { while (!Serial1.available()) testescape(); return Serial1.read(); }

inline void serial1write (char c) { Serial1.write(c); }

#endif

void serialbegin (int address, int baud) {
  #if defined(SERIAL1)
  if (address == 1) Serial1.begin((long)baud*100);
  else error(WITHSERIAL, PSTR("port not supported"), number(address));
  #endif
}

void serialend (int address) {
  #if defined(SERIAL1)
  if (address == 1) {Serial1.flush(); Serial1.end(); }
  #endif
}

void testescape () {
  if (Serial.read() == '~') error2(0, PSTR("escape!"));
}

int gserial () {
  if (LastChar) {
    char temp = LastChar;
    LastChar = 0;
    return temp;
  }
  while (!Serial.available());
  char temp = Serial.read();
  if (temp != '\n') pserial(temp);
  return temp;
}
