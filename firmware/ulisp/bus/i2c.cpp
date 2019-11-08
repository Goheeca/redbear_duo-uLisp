#include "i2c.h"
#include "../stream.h"

// I2C interface

unsigned int I2CCount;

void I2Cinit (bool enablePullup) {
  (void) enablePullup;
  Wire.begin();
}

bool I2Cstart (uint8_t address, uint8_t read) {
 int ok = true;
 if (read == 0) {
   Wire.beginTransmission(address);
   ok = (Wire.endTransmission(true) == 0);
   Wire.beginTransmission(address);
 }
 else Wire.requestFrom(address, I2CCount);
 return ok;
}

bool I2Crestart (uint8_t address, uint8_t read) {
  int error = (Wire.endTransmission(false) != 0);
  if (read == 0) Wire.beginTransmission(address);
  else Wire.requestFrom(address, I2CCount);
  return error ? false : true;
}

void I2Cstop (uint8_t read) {
  if (read == 0) Wire.endTransmission(); // Check for error?
}

object *fn_restarti2c (object *args, object *env) {
  (void) env;
  int stream = first_(args)->integer;
  args = cdr_(args);
  int read = 0; // Write
  I2CCount = 0;
  if (args != NULL) {
    object *rw = first_(args);
    if (integerp_(rw)) I2CCount = checkinteger(RESTARTI2C, rw);
    read = (rw != NULL);
  }
  int address = stream & 0xFF;
  if (stream>>8 != I2CSTREAM) error2(RESTARTI2C, PSTR("not an i2c stream"));
  return I2Crestart(address, read) ? tee : nil_;
}
