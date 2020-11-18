#include <Particle.h>
#include "ulisp/ulisp-stm32.h"

SYSTEM_MODE(MANUAL)

void setup () {
  Serial.begin(9600);
  waitUntil(Serial.isConnected);
  ulisp_setup();
}

void loop () {
  ULISP_SETUP_EXCEPTION_HANDLING
  delay(100); while (Serial.available()) Serial.read();
  ulisp_reset();
  repl(NULL);
}
