#include "machine.h"
#include "note.h"
#include "platform.h"

// Sleep

void initsleep () {

}

void sleep (int secs) {
  delay(1000*secs);
}

// Arduino procedures

object *fn_pinmode (object *args, object *env) {
  (void) env;
  int pin = checkinteger(PINMODE, first_(args));
  int pm = INPUT;
  object *mode = second_(args);
  if (integerp_(mode)) {
    int nmode = checkinteger(PINMODE, mode);
    if (nmode == 1) pm = OUTPUT; else if (nmode == 2) pm = INPUT_PULLUP;
    #if defined(INPUT_PULLDOWN)
    else if (nmode == 4) pm = INPUT_PULLDOWN;
    #endif
  } else if (mode != nil_) pm = OUTPUT;
  pinMode(pin, (WiringPinMode)pm);
  return nil_;
}

object *fn_digitalread (object *args, object *env) {
  (void) env;
  int pin = checkinteger(DIGITALREAD, first_(args));
  if (digitalRead(pin) != 0) return tee; else return nil_;
}

object *fn_digitalwrite (object *args, object *env) {
  (void) env;
  int pin = checkinteger(DIGITALWRITE, first_(args));
  object *mode = second_(args);
  if (integerp_(mode)) digitalWrite(pin, mode->integer);
  else digitalWrite(pin, (mode != nil_));
  return mode;
}

object *fn_analogread (object *args, object *env) {
  (void) env;
  int pin = checkinteger(ANALOGREAD, first_(args));
  checkanalogread(pin);
  return number(analogRead(pin));
}

object *fn_analogwrite (object *args, object *env) {
  (void) env;
  int pin = checkinteger(ANALOGWRITE, first_(args));
  checkanalogwrite(pin);
  object *value = second_(args);
  analogWrite(pin, checkinteger(ANALOGWRITE, value));
  return value;
}

object *fn_delay (object *args, object *env) {
  (void) env;
  object *arg1 = first_(args);
  delay(checkinteger(DELAY, arg1));
  return arg1;
}

object *fn_millis (object *args, object *env) {
  (void) args, (void) env;
  return number(millis());
}

object *fn_sleep (object *args, object *env) {
  (void) env;
  object *arg1 = first_(args);
  sleep(checkinteger(SLEEP, arg1));
  return arg1;
}

object *fn_note (object *args, object *env) {
  (void) env;
  static int pin = 255;
  if (args != NULL) {
    pin = checkinteger(NOTE, first_(args));
    int note = 0;
    if (cddr_(args) != NULL) note = checkinteger(NOTE, second_(args));
    int octave = 0;
    if (cddr_(args) != NULL) octave = checkinteger(NOTE, third_(args));
    playnote(pin, note, octave);
  } else nonote(pin);
  return nil_;
}
