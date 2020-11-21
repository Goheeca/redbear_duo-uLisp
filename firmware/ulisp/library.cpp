#ifdef __ULISP_C_H

// Insert your own function definitions here

enum function_ { PEEK = _ENDFUNCTIONS, POKE, ENDFUNCTIONS};

object *fn_peek (object *args, object *env);
object *fn_poke (object *args, object *env);

extern const char string_fn_peek[] PROGMEM;
extern const char string_fn_poke[] PROGMEM;

#ifdef LOOKUP_TABLE_ENTRIES
#undef LOOKUP_TABLE_ENTRIES
#endif
#define LOOKUP_TABLE_ENTRIES \
    { string_fn_peek, fn_peek, 0x11 }, \
    { string_fn_poke, fn_poke, 0x22 }, \

#else // __ULISP_C_H

#include "core/ulisp-c-library.h"

object *fn_peek (object *args, object *env) {
  (void) env;
  int addr = checkinteger(PEEK, first(args));
  return number(*(int *)addr);
}

object *fn_poke (object *args, object *env) {
  (void) env;
  int addr = checkinteger(POKE, first(args));
  object *val = second(args);
  *(int *)addr = checkinteger(POKE, val);
  return val;
}

const char string_fn_peek[] PROGMEM = "peek";
const char string_fn_poke[] PROGMEM = "poke";

#endif
