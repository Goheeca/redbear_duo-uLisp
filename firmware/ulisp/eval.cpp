#include "eval.h"
#include "print.h"
#include "tail_call.h"
#include "gc.h"

#if defined (serialmonitor)
#include "bus/serial.h"
#endif
// Handling closures

object *closure (int tc, symbol_t name, object *state, object *function, object *args, object **env) {
  int trace = 0;
  if (name) trace = tracing(name);
  if (trace) {
    indent(TraceDepth[trace-1]<<1, pserial);
    pint(TraceDepth[trace-1]++, pserial);
    pserial(':'); pserial(' '); pserial('('); pstring(lookupbuiltin(name), pserial);
  }
  object *params = first_(function);
  function = cdr_(function);
  // Dropframe
  if (tc) {
    if (*env != NULL && car_(*env) == NULL) {
      pop_(*env);
      while (*env != NULL && car_(*env) != NULL) pop_(*env);
    } else push_(nil_, *env);
  }
  // Push state
  while (state != NULL) {
    object *pair = first_(state);
    push_(pair, *env);
    state = cdr_(state);
  }
  // Add arguments to environment
  boolean optional = false;
  while (params != NULL) {
    object *value;
    object *var = first_(params);
    if (symbolp_(var) && var->name == OPTIONAL) optional = true;
    else {
      if (consp(var)) {
        if (!optional) error(name, PSTR("invalid default value"), var);
        if (args == NULL) value = eval(second_(var), *env);
        else { value = first_(args); args = cdr_(args); }
        var = first_(var);
        if (!symbolp_(var)) error(name, PSTR("illegal optional parameter"), var);
      } else if (!symbolp_(var)) {
        error2(name, PSTR("illegal parameter"));
      } else if (var->name == AMPREST) {
        params = cdr_(params);
        var = first_(params);
        value = args;
        args = NULL;
      } else {
        if (args == NULL) {
          if (optional) value = nil_;
          else {
            if (name) error2(name, PSTR("has too few arguments"));
            else error2(0, PSTR("function has too few arguments"));
          }
        } else { value = first_(args); args = cdr_(args); }
      }
      push_(cons(var,value), *env);
      if (trace) { pserial(' '); printobject(value, pserial); }
    }
    params = cdr_(params);
  }
  if (args != NULL) {
    if (name) error2(name, PSTR("has too many arguments"));
    else error2(0, PSTR("function has too many arguments"));
  }
  if (trace) { pserial(')'); pln(pserial); }
  // Do an implicit progn
  if (tc) push_(nil_, *env);
  return tf_progn(function, *env);
}

object *apply (symbol_t name, object *function, object *args, object *env) {
  if (symbolp_(function)) {
    symbol_t fname = function->name;
    checkargs(fname, args);
    return ((fn_ptr_type)lookupfn(fname))(args, env);
  }
  if (consp(function) && issymbol(car_(function), LAMBDA)) {
    function = cdr_(function);
    object *result = closure(0, 0, NULL, function, args, &env);
    return eval(result, env);
  }
  if (consp(function) && issymbol(car_(function), CLOSURE)) {
    function = cdr_(function);
    object *result = closure(0, 0, car_(function), cdr_(function), args, &env);
    return eval(result, env);
  }
  error(name, PSTR("illegal function"), function);
  return NULL;
}

// Main evaluator

uint8_t End;

object *eval (object *form, object *env) {
  int TC=0;
  EVAL:
  yield(); // Needed on ESP8266 to avoid Soft WDT Reset
  // Enough space?
  if (End != 0xA5) error2(0, PSTR("Stack overflow"));
  if (Freespace <= WORKSPACESIZE>>4) gc(form, env);
  // Escape
  if (tstflag_(ESCAPE)) { clrflag_(ESCAPE); error2(0, PSTR("Escape!"));}
  #if defined (serialmonitor)
  testescape();
  #endif

  if (form == NULL) return nil_;

  if (integerp_(form) || floatp_(form) || characterp_(form) || stringp_(form)) return form;

  if (symbolp_(form)) {
    symbol_t name = form->name;
    if (name == NIL) return nil_;
    object *pair = value(name, env);
    if (pair != NULL) return cdr_(pair);
    pair = value(name, GlobalEnv);
    if (pair != NULL) return cdr_(pair);
    else if (name <= ENDFUNCTIONS) return form;
    error(0, PSTR("undefined"), form);
  }

  // It's a list
  object *function = car_(form);
  object *args = cdr_(form);

  if (function == NULL) error(0, PSTR("illegal function"), nil_);
  if (!listp(args)) error(0, PSTR("can't evaluate a dotted pair"), args);

  // List starts with a symbol?
  if (symbolp_(function)) {
    symbol_t name = function->name;

    if ((name == LET) || (name == LETSTAR)) {
      int TCstart = TC;
      object *assigns = first_(args);
      object *forms = cdr_(args);
      object *newenv = env;
      push_(newenv, GCStack);
      while (assigns != NULL) {
        object *assign = car_(assigns);
        if (!consp(assign)) push_(cons(assign,nil_), newenv);
        else if (cdr_(assign) == NULL) push_(cons(first_(assign),nil_), newenv);
        else push_(cons(first_(assign),eval(second_(assign),env)), newenv);
        car_(GCStack) = newenv;
        if (name == LETSTAR) env = newenv;
        assigns = cdr_(assigns);
      }
      env = newenv;
      pop_(GCStack);
      form = tf_progn(forms,env);
      TC = TCstart;
      goto EVAL;
    }

    if (name == LAMBDA) {
      if (env == NULL) return form;
      object *envcopy = NULL;
      while (env != NULL) {
        object *pair = first_(env);
        if (pair != NULL) push_(pair, envcopy);
        env = cdr_(env);
      }
      return cons(symbol(CLOSURE), cons(envcopy,args));
    }

    if (name < SPECIAL_FORMS) error2((int)function, PSTR("can't be used as a function"));

    if ((name > SPECIAL_FORMS) && (name < TAIL_FORMS)) {
      return ((fn_ptr_type)lookupfn(name))(args, env);
    }

    if ((name > TAIL_FORMS) && (name < FUNCTIONS)) {
      form = ((fn_ptr_type)lookupfn(name))(args, env);
      TC = 1;
      goto EVAL;
    }
  }

  // Evaluate the parameters - result in head
  object *fname = car_(form);
  int TCstart = TC;
  object *head = cons(eval(car_(form), env), NULL);
  push_(head, GCStack); // Don't GC the result list
  object *tail = head;
  form = cdr_(form);
  int nargs = 0;

  while (form != NULL){
    object *obj = cons(eval(car_(form),env),NULL);
    cdr_(tail) = obj;
    tail = obj;
    form = cdr_(form);
    nargs++;
  }

  function = car_(head);
  args = cdr_(head);

  if (symbolp_(function)) {
    symbol_t name = function->name;
    if (name >= ENDFUNCTIONS) error(0, PSTR("not valid here"), fname);
    if (nargs<lookupmin(name)) error2(name, PSTR("has too few arguments"));
    if (nargs>lookupmax(name)) error2(name, PSTR("has too many arguments"));
    object *result = ((fn_ptr_type)lookupfn(name))(args, env);
    pop_(GCStack);
    return result;
  }

  if (consp(function) && issymbol(car_(function), LAMBDA)) {
    form = closure(TCstart, fname->name, NULL, cdr_(function), args, &env);
    pop_(GCStack);
    int trace = tracing(fname->name);
    if (trace) {
      object *result = eval(form, env);
      indent((--(TraceDepth[trace-1]))<<1, pserial);
      pint(TraceDepth[trace-1], pserial);
      pserial(':'); pserial(' ');
      printobject(fname, pserial); pfstring(PSTR(" returned "), pserial);
      printobject(result, pserial); pln(pserial);
      return result;
    } else {
      TC = 1;
      goto EVAL;
    }
  }

  if (consp(function) && issymbol(car_(function), CLOSURE)) {
    function = cdr_(function);
    form = closure(TCstart, fname->name, car_(function), cdr_(function), args, &env);
    pop_(GCStack);
    TC = 1;
    goto EVAL;
  }

  error(0, PSTR("illegal function"), fname); return nil_;
}
