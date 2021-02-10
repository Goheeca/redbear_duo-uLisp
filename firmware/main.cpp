#include <Particle.h>
#include "ulisp/ulisp.h"

SYSTEM_MODE(AUTOMATIC)
SYSTEM_THREAD(ENABLED)

int cloud(String data);
bool init = false;

void setup () {
  Serial.begin(9600);
  ulisp_setup();
  Particle.function("lisp", cloud);
}

void loop () {
  ULISP_SETUP_EXCEPTION_HANDLING
  if (!init) {
    ulisp_load_library();
    while (!Serial.isConnected()) process_system();
    ulisp_banner();
    init = true;
  }
  ulisp_reset();
  repl(NULL);
}
