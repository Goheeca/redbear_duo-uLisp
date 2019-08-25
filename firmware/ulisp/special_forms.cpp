#include "special_forms.h"
#include "stream.h"
#include "eval.h"
#include "tail_call.h"
#include "bus/serial.h"
#include "bus/i2c.h"
#include "bus/spi.h"
#include "hw/machine.h"
#include "hw/platform.h"

// Special forms

object *sp_quote (object *args, object *env) {
  (void) env;
  checkargs(QUOTE, args);
  return first_(args);
}

object *sp_defun (object *args, object *env) {
  (void) env;
  checkargs(DEFUN, args);
  object *var = first_(args);
  if (var->type != SYMBOL_TAG) error(DEFUN, PSTR("not a symbol"), var);
  object *val = cons(symbol(LAMBDA), cdr_(args));
  object *pair = value(var->name,GlobalEnv);
  if (pair != NULL) { cdr_(pair) = val; return var; }
  push_(cons(var, val), GlobalEnv);
  return var;
}

object *sp_defvar (object *args, object *env) {
  checkargs(DEFVAR, args);
  object *var = first_(args);
  if (var->type != SYMBOL_TAG) error(DEFVAR, PSTR("not a symbol"), var);
  object *val = NULL;
  val = eval(second_(args), env);
  object *pair = value(var->name, GlobalEnv);
  if (pair != NULL) { cdr_(pair) = val; return var; }
  push_(cons(var, val), GlobalEnv);
  return var;
}

object *sp_setq (object *args, object *env) {
  checkargs(SETQ, args);
  object *arg = eval(second_(args), env);
  object *pair = findvalue(first_(args), env);
  cdr_(pair) = arg;
  return arg;
}

object *sp_loop (object *args, object *env) {
  clrflag_(RETURNFLAG);
  object *start = args;
  for (;;) {
    args = start;
    while (args != NULL) {
      object *result = eval(car_(args),env);
      if (tstflag_(RETURNFLAG)) {
        clrflag_(RETURNFLAG);
        return result;
      }
      args = cdr_(args);
    }
  }
}

object *sp_push_ (object *args, object *env) {
  checkargs(PUSH, args);
  object *item = eval(first_(args), env);
  object **loc = place(PUSH, second_(args), env);
  push_(item, *loc);
  return *loc;
}

object *sp_pop_ (object *args, object *env) {
  checkargs(POP, args);
  object **loc = place(POP, first_(args), env);
  object *result = car_(*loc);
  pop_(*loc);
  return result;
}

// Special forms incf/decf

object *sp_incf (object *args, object *env) {
  checkargs(INCF, args);
  object **loc = place(INCF, first_(args), env);
  args = cdr_(args);

  object *x = *loc;
  object *inc = (args != NULL) ? eval(first_(args), env) : NULL;

  if (floatp_(x) || floatp_(inc)) {
    float increment;
    float value = checkintfloat(INCF, x);

    if (inc == NULL) increment = 1.0;
    else increment = checkintfloat(INCF, inc);

    *loc = makefloat(value + increment);
  } else if (integerp_(x) && (integerp_(inc) || inc == NULL)) {
    int increment;
    int value = x->integer;

    if (inc == NULL) increment = 1;
    else increment = inc->integer;

    if (increment < 1) {
      if (INT_MIN - increment > value) *loc = makefloat((float)value + (float)increment);
      else *loc = number(value + increment);
    } else {
      if (INT_MAX - increment < value) *loc = makefloat((float)value + (float)increment);
      else *loc = number(value + increment);
    }
  } else error2(INCF, notanumber);
  return *loc;
}

object *sp_decf (object *args, object *env) {
  checkargs(DECF, args);
  object **loc = place(DECF, first_(args), env);
  args = cdr_(args);

  object *x = *loc;
  object *dec = (args != NULL) ? eval(first_(args), env) : NULL;

  if (floatp_(x) || floatp_(dec)) {
    float decrement;
    float value = checkintfloat(DECF, x);

    if (dec == NULL) decrement = 1.0;
    else decrement = checkintfloat(DECF, dec);

    *loc = makefloat(value - decrement);
  } if (integerp_(x) && (integerp_(dec) || dec == NULL)) {
    int decrement;
    int value = x->integer;

    if (dec == NULL) decrement = 1;
    else decrement = dec->integer;

    if (decrement < 1) {
      if (INT_MAX + decrement < value) *loc = makefloat((float)value - (float)decrement);
      else *loc = number(value - decrement);
    } else {
      if (INT_MIN + decrement > value) *loc = makefloat((float)value - (float)decrement);
      else *loc = number(value - decrement);
    }
  } else error2(DECF, notanumber);
  return *loc;
}

object *sp_setf (object *args, object *env) {
  checkargs(SETF, args);
  object **loc = place(SETF, first_(args), env);
  object *result = eval(second_(args), env);
  *loc = result;
  return result;
}

object *sp_dolist (object *args, object *env) {
  if (args == NULL) error2(DOLIST, noargument);
  object *params = first_(args);
  object *var = first_(params);
  object *result;
  object *list = eval(second_(params), env);
  push_(list, GCStack); // Don't GC the list
  object *pair = cons(var,nil_);
  push_(pair,env);
  params = cdr_(cdr_(params));
  object *forms = cdr_(args);
  while (list != NULL) {
    if (improperp(list)) error(DOLIST, notproper, list);
    cdr_(pair) = first_(list);
    list = cdr_(list);
    result = eval(tf_progn(forms,env), env);
    if (tstflag_(RETURNFLAG)) {
      clrflag_(RETURNFLAG);
      return result;
    }
  }
  cdr_(pair) = nil_;
  pop_(GCStack);
  if (params == NULL) return nil_;
  return eval(car_(params), env);
}

object *sp_dotimes (object *args, object *env) {
  if (args == NULL) error2(DOTIMES, noargument);
  object *params = first_(args);
  object *var = first_(params);
  object *result;
  int count = checkinteger(DOTIMES, eval(second_(params), env));
  int index = 0;
  params = cdr_(cdr_(params));
  object *pair = cons(var,number(0));
  push_(pair,env);
  object *forms = cdr_(args);
  while (index < count) {
    cdr_(pair) = number(index);
    index++;
    result = eval(tf_progn(forms,env), env);
    if (tstflag_(RETURNFLAG)) {
      clrflag_(RETURNFLAG);
      return result;
    }
  }
  cdr_(pair) = number(index);
  if (params == NULL) return nil_;
  return eval(car_(params), env);
}

object *sp_trace (object *args, object *env) {
  (void) env;
  while (args != NULL) {
      trace(first_(args)->name);
      args = cdr_(args);
  }
  int i = 0;
  while (i < TRACEMAX) {
    if (TraceFn[i] != 0) args = cons(symbol(TraceFn[i]), args);
    i++;
  }
  return args;
}

object *sp_untrace (object *args, object *env) {
  (void) env;
  if (args == NULL) {
    int i = 0;
    while (i < TRACEMAX) {
      if (TraceFn[i] != 0) args = cons(symbol(TraceFn[i]), args);
      TraceFn[i] = 0;
      i++;
    }
  } else {
    while (args != NULL) {
      untrace(first_(args)->name);
      args = cdr_(args);
    }
  }
  return args;
}

object *sp_formillis (object *args, object *env) {
  object *param = first_(args);
  unsigned long start = millis();
  unsigned long now, total = 0;
  if (param != NULL) total = checkinteger(FORMILLIS, eval(first_(param), env));
  eval(tf_progn(cdr_(args),env), env);
  do {
    now = millis() - start;
    testescape();
  } while (now < total);
  if (now <= INT_MAX) return number(now);
  return nil_;
}

object *sp_withserial (object *args, object *env) {
  object *params = first_(args);
  if (params == NULL) error2(WITHSERIAL, nostream);
  object *var = first_(params);
  int address = checkinteger(WITHSERIAL, eval(second_(params), env));
  params = cddr_(params);
  int baud = 96;
  if (params != NULL) baud = checkinteger(WITHSERIAL, eval(first_(params), env));
  object *pair = cons(var, stream(SERIALSTREAM, address));
  push_(pair,env);
  serialbegin(address, baud);
  object *forms = cdr_(args);
  object *result = eval(tf_progn(forms,env), env);
  serialend(address);
  return result;
}

object *sp_withi2c (object *args, object *env) {
  object *params = first_(args);
  if (params == NULL) error2(WITHI2C, nostream);
  object *var = first_(params);
  int address = checkinteger(WITHI2C, eval(second_(params), env));
  params = cddr_(params);
  int read = 0; // Write
  I2CCount = 0;
  if (params != NULL) {
    object *rw = eval(first_(params), env);
    if (integerp_(rw)) I2CCount = rw->integer;
    read = (rw != NULL);
  }
  I2Cinit(1); // Pullups
  object *pair = cons(var, (I2Cstart(address, read)) ? stream(I2CSTREAM, address) : nil_);
  push_(pair,env);
  object *forms = cdr_(args);
  object *result = eval(tf_progn(forms,env), env);
  I2Cstop(read);
  return result;
}

object *sp_withspi (object *args, object *env) {
  object *params = first_(args);
  if (params == NULL) error2(WITHSPI, nostream);
  object *var = first_(params);
  params = cdr_(params);
  if (params == NULL) error2(WITHSPI, nostream);
  int pin = checkinteger(WITHSPI, eval(car_(params), env));
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  params = cdr_(params);
  int clock = 4000, mode = SPI_MODE0; // Defaults
  BitOrder bitorder = MSBFIRST;
  if (params != NULL) {
    clock = checkinteger(WITHSPI, eval(car_(params), env));
    params = cdr_(params);
    if (params != NULL) {
      bitorder = (checkinteger(WITHSPI, eval(car_(params), env)) == 0) ? LSBFIRST : MSBFIRST;
      params = cdr_(params);
      if (params != NULL) {
        int modeval = checkinteger(WITHSPI, eval(car_(params), env));
        mode = (modeval == 3) ? SPI_MODE3 : (modeval == 2) ? SPI_MODE2 : (modeval == 1) ? SPI_MODE1 : SPI_MODE0;
      }
    }
  }
  object *pair = cons(var, stream(SPISTREAM, pin));
  push_(pair,env);
  SPI.begin();
  SPI.beginTransaction(SPISettings(((unsigned long)clock * 1000), bitorder, mode));
  digitalWrite(pin, LOW);
  object *forms = cdr_(args);
  object *result = eval(tf_progn(forms,env), env);
  digitalWrite(pin, HIGH);
  SPI.endTransaction();
  return result;
}

object *sp_withsdcard (object *args, object *env) {
#if defined(sdcardsupport)
  object *params = first_(args);
  if (params == NULL) error2(WITHSDCARD, nostream);
  object *var = first_(params);
  object *filename = eval(second_(params), env);
  params = cddr_(params);
  SD.begin(SDCARD_SS_PIN);
  int mode = 0;
  if (params != NULL && first_(params) != NULL) mode = checkinteger(WITHSDCARD, first_(params));
  int oflag = O_READ;
  if (mode == 1) oflag = O_RDWR | O_CREAT | O_APPEND; else if (mode == 2) oflag = O_RDWR | O_CREAT | O_TRUNC;
  if (mode >= 1) {
    SDpfile = SD.open(MakeFilename(filename), oflag);
    if (!SDpfile) error2(WITHSDCARD, PSTR("problem writing to SD card"));
  } else {
    SDgfile = SD.open(MakeFilename(filename), oflag);
    if (!SDgfile) error2(WITHSDCARD, PSTR("problem reading from SD card"));
  }
  object *pair = cons(var, stream(SDSTREAM, 1));
  push_(pair,env);
  object *forms = cdr_(args);
  object *result = eval(tf_progn(forms,env), env);
  if (mode >= 1) SDpfile.close(); else SDgfile.close();
  return result;
#else
  (void) args, (void) env;
  error2(WITHSDCARD, PSTR("not supported"));
  return nil_;
#endif
}
