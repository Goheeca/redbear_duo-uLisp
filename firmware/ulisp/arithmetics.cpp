#include "core.h"
#include "arithmetics.h"
#include <math.h>

// Arithmetic functions

object *add_floats (object *args, float fresult) {
  while (args != NULL) {
    object *arg = car_(args);
    fresult = fresult + checkintfloat(ADD, arg);
    args = cdr_(args);
  }
  return makefloat(fresult);
}

object *fn_add (object *args, object *env) {
  (void) env;
  int result = 0;
  while (args != NULL) {
    object *arg = car_(args);
    if (floatp_(arg)) return add_floats(args, (float)result);
    else if (integerp_(arg)) {
      int val = arg->integer;
      if (val < 1) { if (INT_MIN - val > result) return add_floats(args, (float)result); }
      else { if (INT_MAX - val < result) return add_floats(args, (float)result); }
      result = result + val;
    } else error(ADD, notanumber, arg);
    args = cdr_(args);
  }
  return number(result);
}

object *subtract_floats (object *args, float fresult) {
  while (args != NULL) {
    object *arg = car_(args);
    fresult = fresult - checkintfloat(SUBTRACT, arg);
    args = cdr_(args);
  }
  return makefloat(fresult);
}

object *negate (object *arg) {
  if (integerp_(arg)) {
    int result = arg->integer;
    if (result == INT_MIN) return makefloat(-result);
    else return number(-result);
  } else if (floatp_(arg)) return makefloat(-(arg->single_float));
  else error(SUBTRACT, notanumber, arg);
}

object *fn_subtract (object *args, object *env) {
  (void) env;
  object *arg = car_(args);
  args = cdr_(args);
  if (args == NULL) return negate(arg);
  else if (floatp_(arg)) return subtract_floats(args, arg->single_float);
  else if (integerp_(arg)) {
    int result = arg->integer;
    while (args != NULL) {
      arg = car_(args);
      if (floatp_(arg)) return subtract_floats(args, result);
      else if (integerp_(arg)) {
        int val = (car_(args))->integer;
        if (val < 1) { if (INT_MAX + val < result) return subtract_floats(args, result); }
        else { if (INT_MIN + val > result) return subtract_floats(args, result); }
        result = result - val;
      } else error(SUBTRACT, notanumber, arg);
      args = cdr_(args);
    }
    return number(result);
  } else error(SUBTRACT, notanumber, arg);
}

object *multiply_floats (object *args, float fresult) {
  while (args != NULL) {
   object *arg = car_(args);
    fresult = fresult * checkintfloat(MULTIPLY, arg);
    args = cdr_(args);
  }
  return makefloat(fresult);
}

object *fn_multiply (object *args, object *env) {
  (void) env;
  int result = 1;
  while (args != NULL){
    object *arg = car_(args);
    if (floatp_(arg)) return multiply_floats(args, result);
    else if (integerp_(arg)) {
      int64_t val = result * (int64_t)(arg->integer);
      if ((val > INT_MAX) || (val < INT_MIN)) return multiply_floats(args, result);
      result = val;
    } else error(MULTIPLY, notanumber, arg);
    args = cdr_(args);
  }
  return number(result);
}

object *divide_floats (object *args, float fresult) {
  while (args != NULL) {
    object *arg = car_(args);
    float f = checkintfloat(DIVIDE, arg);
    if (f == 0.0) error2(DIVIDE, PSTR("division by zero"));
    fresult = fresult / f;
    args = cdr_(args);
  }
  return makefloat(fresult);
}

object *fn_divide (object *args, object *env) {
  (void) env;
  object* arg = first_(args);
  args = cdr_(args);
  // One argument
  if (args == NULL) {
    if (floatp_(arg)) {
      float f = arg->single_float;
      if (f == 0.0) error2(DIVIDE, PSTR("division by zero"));
      return makefloat(1.0 / f);
    } else if (integerp_(arg)) {
      int i = arg->integer;
      if (i == 0) error2(DIVIDE, PSTR("division by zero"));
      else if (i == 1) return number(1);
      else return makefloat(1.0 / i);
    } else error(DIVIDE, notanumber, arg);
  }
  // Multiple arguments
  if (floatp_(arg)) return divide_floats(args, arg->single_float);
  else if (integerp_(arg)) {
    int result = arg->integer;
    while (args != NULL) {
      arg = car_(args);
      if (floatp_(arg)) {
        return divide_floats(args, result);
      } else if (integerp_(arg)) {
        int i = arg->integer;
        if (i == 0) error2(DIVIDE, PSTR("division by zero"));
        if ((result % i) != 0) return divide_floats(args, result);
        if ((result == INT_MIN) && (i == -1)) return divide_floats(args, result);
        result = result / i;
        args = cdr_(args);
      } else error(DIVIDE, notanumber, arg);
    }
    return number(result);
  } else error(DIVIDE, notanumber, arg);
}

object *fn_mod (object *args, object *env) {
  (void) env;
  object *arg1 = first_(args);
  object *arg2 = second_(args);
  if (integerp_(arg1) && integerp_(arg2)) {
    int divisor = arg2->integer;
    if (divisor == 0) error2(MOD, PSTR("division by zero"));
    int dividend = arg1->integer;
    int remainder = dividend % divisor;
    if ((dividend<0) != (divisor<0)) remainder = remainder + divisor;
    return number(remainder);
  } else {
    float fdivisor = checkintfloat(MOD, arg2);
    if (fdivisor == 0.0) error2(MOD, PSTR("division by zero"));
    float fdividend = checkintfloat(MOD, arg1);
    float fremainder = fmod(fdividend , fdivisor);
    if ((fdividend<0) != (fdivisor<0)) fremainder = fremainder + fdivisor;
    return makefloat(fremainder);
  }
}

object *fn_oneplus (object *args, object *env) {
  (void) env;
  object* arg = first_(args);
  if (floatp_(arg)) return makefloat((arg->single_float) + 1.0);
  else if (integerp_(arg)) {
    int result = arg->integer;
    if (result == INT_MAX) return makefloat((arg->integer) + 1.0);
    else return number(result + 1);
  } else error(ONEPLUS, notanumber, arg);
}

object *fn_oneminus (object *args, object *env) {
  (void) env;
  object* arg = first_(args);
  if (floatp_(arg)) return makefloat((arg->single_float) - 1.0);
  else if (integerp_(arg)) {
    int result = arg->integer;
    if (result == INT_MIN) return makefloat((arg->integer) - 1.0);
    else return number(result - 1);
  } else error(ONEMINUS, notanumber, arg);
}

object *fn_abs (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  if (floatp_(arg)) return makefloat(abs(arg->single_float));
  else if (integerp_(arg)) {
    int result = arg->integer;
    if (result == INT_MIN) return makefloat(abs((float)result));
    else return number(abs(result));
  } else error(ABS, notanumber, arg);
}

object *fn_random (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  if (integerp_(arg)) return number(random(arg->integer));
  else if (floatp_(arg)) return makefloat((float)rand()/(float)(RAND_MAX/(arg->single_float)));
  else error(RANDOM, notanumber, arg);
}

object *fn_maxfn (object *args, object *env) {
  (void) env;
  object* result = first_(args);
  args = cdr_(args);
  while (args != NULL) {
    object *arg = car_(args);
    if (integerp_(result) && integerp_(arg)) {
      if ((arg->integer) > (result->integer)) result = arg;
    } else if ((checkintfloat(MAXFN, arg) > checkintfloat(MAXFN, result))) result = arg;
    args = cdr_(args);
  }
  return result;
}

object *fn_minfn (object *args, object *env) {
  (void) env;
  object* result = first_(args);
  args = cdr_(args);
  while (args != NULL) {
    object *arg = car_(args);
    if (integerp_(result) && integerp_(arg)) {
      if ((arg->integer) < (result->integer)) result = arg;
    } else if ((checkintfloat(MINFN, arg) < checkintfloat(MINFN, result))) result = arg;
    args = cdr_(args);
  }
  return result;
}

// Arithmetic comparisons

object *fn_noteq (object *args, object *env) {
  (void) env;
  while (args != NULL) {
    object *nargs = args;
    object *arg1 = first_(nargs);
    nargs = cdr_(nargs);
    while (nargs != NULL) {
      object *arg2 = first_(nargs);
      if (integerp_(arg1) && integerp_(arg2)) {
        if ((arg1->integer) == (arg2->integer)) return nil_;
      } else if ((checkintfloat(NOTEQ, arg1) == checkintfloat(NOTEQ, arg2))) return nil_;
      nargs = cdr_(nargs);
    }
    args = cdr_(args);
  }
  return tee;
}

object *fn_numeq (object *args, object *env) {
  (void) env;
  object *arg1 = first_(args);
  args = cdr_(args);
  while (args != NULL) {
    object *arg2 = first_(args);
    if (integerp_(arg1) && integerp_(arg2)) {
      if (!((arg1->integer) == (arg2->integer))) return nil_;
    } else if (!(checkintfloat(NUMEQ, arg1) == checkintfloat(NUMEQ, arg2))) return nil_;
    arg1 = arg2;
    args = cdr_(args);
  }
  return tee;
}

object *fn_less (object *args, object *env) {
  (void) env;
  object *arg1 = first_(args);
  args = cdr_(args);
  while (args != NULL) {
    object *arg2 = first_(args);
    if (integerp_(arg1) && integerp_(arg2)) {
      if (!((arg1->integer) < (arg2->integer))) return nil_;
    } else if (!(checkintfloat(LESS, arg1) < checkintfloat(LESS, arg2))) return nil_;
    arg1 = arg2;
    args = cdr_(args);
  }
  return tee;
}

object *fn_lesseq (object *args, object *env) {
  (void) env;
  object *arg1 = first_(args);
  args = cdr_(args);
  while (args != NULL) {
    object *arg2 = first_(args);
    if (integerp_(arg1) && integerp_(arg2)) {
      if (!((arg1->integer) <= (arg2->integer))) return nil_;
    } else if (!(checkintfloat(LESSEQ, arg1) <= checkintfloat(LESSEQ, arg2))) return nil_;
    arg1 = arg2;
    args = cdr_(args);
  }
  return tee;
}

object *fn_greater (object *args, object *env) {
  (void) env;
  object *arg1 = first_(args);
  args = cdr_(args);
  while (args != NULL) {
    object *arg2 = first_(args);
    if (integerp_(arg1) && integerp_(arg2)) {
      if (!((arg1->integer) > (arg2->integer))) return nil_;
    } else if (!(checkintfloat(GREATER, arg1) > checkintfloat(GREATER, arg2))) return nil_;
    arg1 = arg2;
    args = cdr_(args);
  }
  return tee;
}

object *fn_greatereq (object *args, object *env) {
  (void) env;
  object *arg1 = first_(args);
  args = cdr_(args);
  while (args != NULL) {
    object *arg2 = first_(args);
    if (integerp_(arg1) && integerp_(arg2)) {
      if (!((arg1->integer) >= (arg2->integer))) return nil_;
    } else if (!(checkintfloat(GREATEREQ, arg1) >= checkintfloat(GREATEREQ, arg2))) return nil_;
    arg1 = arg2;
    args = cdr_(args);
  }
  return tee;
}

object *fn_plusp (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  if (floatp_(arg)) return ((arg->single_float) > 0.0) ? tee : nil_;
  else if (integerp_(arg)) return ((arg->integer) > 0) ? tee : nil_;
  else error(PLUSP, notanumber, arg);
}

object *fn_minusp (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  if (floatp_(arg)) return ((arg->single_float) < 0.0) ? tee : nil_;
  else if (integerp_(arg)) return ((arg->integer) < 0) ? tee : nil_;
  else error(MINUSP, notanumber, arg);
}

object *fn_zerop (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  if (floatp_(arg)) return ((arg->single_float) == 0.0) ? tee : nil_;
  else if (integerp_(arg)) return ((arg->integer) == 0) ? tee : nil_;
  else error(ZEROP, notanumber, arg);
}

object *fn_oddp (object *args, object *env) {
  (void) env;
  int arg = checkinteger(ODDP, first_(args));
  return ((arg & 1) == 1) ? tee : nil_;
}

object *fn_evenp (object *args, object *env) {
  (void) env;
  int arg = checkinteger(EVENP, first_(args));
  return ((arg & 1) == 0) ? tee : nil_;
}

// Number functions

object *fn_integerp (object *args, object *env) {
  (void) env;
  return integerp_(first_(args)) ? tee : nil_;
}

object *fn_numberp (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  return (integerp_(arg) || floatp_(arg)) ? tee : nil_;
}

// Floating-point functions

object *fn_floatfn (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  return (floatp_(arg)) ? arg : makefloat((float)(arg->integer));
}

object *fn_floatp (object *args, object *env) {
  (void) env;
  return floatp_(first_(args)) ? tee : nil_;
}

object *fn_sin (object *args, object *env) {
  (void) env;
  return makefloat(sin(checkintfloat(SIN, first_(args))));
}

object *fn_cos (object *args, object *env) {
  (void) env;
  return makefloat(cos(checkintfloat(COS, first_(args))));
}

object *fn_tan (object *args, object *env) {
  (void) env;
  return makefloat(tan(checkintfloat(TAN, first_(args))));
}

object *fn_asin (object *args, object *env) {
  (void) env;
  return makefloat(asin(checkintfloat(ASIN, first_(args))));
}

object *fn_acos (object *args, object *env) {
  (void) env;
  return makefloat(acos(checkintfloat(ACOS, first_(args))));
}

object *fn_atan (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  float div = 1.0;
  args = cdr_(args);
  if (args != NULL) div = checkintfloat(ATAN, first_(args));
  return makefloat(atan2(checkintfloat(ATAN, arg), div));
}

object *fn_sinh (object *args, object *env) {
  (void) env;
  return makefloat(sinh(checkintfloat(SINH, first_(args))));
}

object *fn_cosh (object *args, object *env) {
  (void) env;
  return makefloat(cosh(checkintfloat(COSH, first_(args))));
}

object *fn_tanh (object *args, object *env) {
  (void) env;
  return makefloat(tanh(checkintfloat(TANH, first_(args))));
}

object *fn_exp (object *args, object *env) {
  (void) env;
  return makefloat(exp(checkintfloat(EXP, first_(args))));
}

object *fn_sqrt (object *args, object *env) {
  (void) env;
  return makefloat(sqrt(checkintfloat(SQRT, first_(args))));
}

object *fn_log (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  float fresult = log(checkintfloat(LOG, arg));
  args = cdr_(args);
  if (args == NULL) return makefloat(fresult);
  else return makefloat(fresult / log(checkintfloat(LOG, first_(args))));
}

int intpower (int base, int exp) {
  int result = 1;
  while (exp) {
    if (exp & 1) result = result * base;
    exp = exp / 2;
    base = base * base;
  }
  return result;
}

object *fn_expt (object *args, object *env) {
  (void) env;
  object *arg1 = first_(args); object *arg2 = second_(args);
  float float1 = checkintfloat(EXPT, arg1);
  float value = log(abs(float1)) * checkintfloat(EXPT, arg2);
  if (integerp_(arg1) && integerp_(arg2) && ((arg2->integer) > 0) && (abs(value) < 21.4875))
    return number(intpower(arg1->integer, arg2->integer));
  if (float1 < 0) error2(EXPT, PSTR("invalid result"));
  return makefloat(exp(value));
}

object *fn_ceiling (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  args = cdr_(args);
  if (args != NULL) return number(ceil(checkintfloat(CEILING, arg) / checkintfloat(CEILING, first_(args))));
  else return number(ceil(checkintfloat(CEILING, arg)));
}

object *fn_floor (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  args = cdr_(args);
  if (args != NULL) return number(floor(checkintfloat(FLOOR, arg) / checkintfloat(FLOOR, first_(args))));
  else return number(floor(checkintfloat(FLOOR, arg)));
}

object *fn_truncate (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  args = cdr_(args);
  if (args != NULL) return number((int)(checkintfloat(TRUNCATE, arg) / checkintfloat(TRUNCATE, first_(args))));
  else return number((int)(checkintfloat(TRUNCATE, arg)));
}

int myround (float number) {
  return (number >= 0) ? (int)(number + 0.5) : (int)(number - 0.5);
}

object *fn_round (object *args, object *env) {
  (void) env;
  object *arg = first_(args);
  args = cdr_(args);
  if (args != NULL) return number(myround(checkintfloat(ROUND, arg) / checkintfloat(ROUND, first_(args))));
  else return number(myround(checkintfloat(ROUND, arg)));
}
