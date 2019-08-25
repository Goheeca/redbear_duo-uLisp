#ifndef __I2C_H
#define __I2C_H

#include <stdint.h>
#include "../core.h"
//#include <Wire.h>
#include "../hw/platform.h"

// I2C interface

extern unsigned int I2CCount;

void I2Cinit (bool enablePullup);
inline uint8_t I2Cread () {
  return Wire.read();
}
inline bool I2Cwrite (uint8_t data) {
  return Wire.write(data);
}
bool I2Cstart (uint8_t address, uint8_t read);
bool I2Crestart (uint8_t address, uint8_t read);
void I2Cstop (uint8_t read);

object *fn_restarti2c (object *args, object *env);

#endif
