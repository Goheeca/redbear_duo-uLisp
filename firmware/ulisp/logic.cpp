#include "logic.h"
#include "hw/platform.h"
//#include <stdlib.h>
// Bitwise operators

object *fn_logand (object *args, object *env) {
  (void) env;
  int result = -1;
  while (args != NULL) {
    result = result & checkinteger(LOGAND, first_(args));
    args = cdr_(args);
  }
  return number(result);
}

object *fn_logior (object *args, object *env) {
  (void) env;
  int result = 0;
  while (args != NULL) {
    result = result | checkinteger(LOGIOR, first_(args));
    args = cdr_(args);
  }
  return number(result);
}

object *fn_logxor (object *args, object *env) {
  (void) env;
  int result = 0;
  while (args != NULL) {
    result = result ^ checkinteger(LOGXOR, first_(args));
    args = cdr_(args);
  }
  return number(result);
}

object *fn_lognot (object *args, object *env) {
  (void) env;
  int result = checkinteger(LOGNOT, car_(args));
  return number(~result);
}

object *fn_ash (object *args, object *env) {
  (void) env;
  int value = checkinteger(ASH, first_(args));
  int count = checkinteger(ASH, second_(args));
  if (count >= 0)
    return number(value << count);
  else
    return number(value >> abs(count));
}

object *fn_logbitp (object *args, object *env) {
  (void) env;
  int index = checkinteger(LOGBITP, first_(args));
  int value = checkinteger(LOGBITP, second_(args));
  return (bitRead(value, index) == 1) ? tee : nil_;
}
