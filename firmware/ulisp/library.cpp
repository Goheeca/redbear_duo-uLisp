#ifdef __ULISP_C_H

// Insert your own function definitions here

enum function_ { PEEK = _ENDFUNCTIONS, POKE, PUBLISH, CALL_TEST, ENDFUNCTIONS};

object *fn_peek (object *args, object *env);
object *fn_poke (object *args, object *env);
object *fn_publish (object *args, object *env);
object *fn_call_test (object *args, object *env);

extern const char string_fn_peek[] PROGMEM;
extern const char string_fn_poke[] PROGMEM;
extern const char string_fn_publish[] PROGMEM;
extern const char string_fn_call_test[] PROGMEM;

#ifdef LOOKUP_TABLE_ENTRIES
#undef LOOKUP_TABLE_ENTRIES
#endif
#define LOOKUP_TABLE_ENTRIES \
    { string_fn_peek, fn_peek, 0x11 }, \
    { string_fn_poke, fn_poke, 0x22 }, \
    { string_fn_publish, fn_publish, 0x22 }, \
    { string_fn_call_test, fn_call_test, 0x11 }, \

#else // __ULISP_C_H

#include "core/ulisp-c-library.h"
#include "Particle.h"

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

object *fn_call_test (object *args, object *env) {
  object *form = cons(newsymbol(pack40("test")), cons(first(args), NULL));
  return eval(form, env);
}

String STR_PARTICLE;
void STR_PARTICLE_APPEND(char c) {
    STR_PARTICLE.concat(String(c));
}

object *fn_publish (object *args, object *env) {
  (void) env;
  if (stringp(first(args))) {
    STR_PARTICLE = String("");
    prin1object(first(args), STR_PARTICLE_APPEND);
    String channel = STR_PARTICLE;
    STR_PARTICLE = String("");
    printobject(second(args), STR_PARTICLE_APPEND);
    return Particle.publish(channel, STR_PARTICLE) ? tee : nil;
  }
  return nil;
}

int STR_POSITION;
const char *STR_READER;
int string_reader () {
  char c = STR_READER[STR_POSITION++];
  return (c != 0) ? c : -1;
}

int fnc (String data) {
    STR_POSITION = 0;
    STR_READER = data.c_str();
    object *lisp_data = read(string_reader);
    object *form = cons(newsymbol(pack40("fnc")), cons(lisp_data, NULL));
    object *result = eval(form, NULL);
    return result == nil ? 0 : 1;
}

const char string_fn_peek[] PROGMEM = "peek";
const char string_fn_poke[] PROGMEM = "poke";
const char string_fn_publish[] PROGMEM = "publish";
const char string_fn_call_test[] PROGMEM = "call-test";

#endif
