#include "Particle.h"
#include "ulisp/ulisp.h"

SYSTEM_THREAD(ENABLED)

void setup () {
  Serial.begin(9600);
  while (!Serial);
  ulisp_setup();
}

void loop () {
  ULISP_SETUP_EXCEPTION_HANDLING
  delay(100); while (Serial.available()) Serial.read();
  ulisp_reset();
  ulisp_repl();
}
