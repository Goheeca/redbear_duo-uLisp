#include "lisp_library.h"
#include "read.h"
#include "eval.h"
#include "print.h"

// LispLibrary

#if defined(lisplibrary)
#include "frozen/user/LispLibrary.h"

int glibrary () {
  if (LastChar) {
    char temp = LastChar;
    LastChar = 0;
    return temp;
  }
  char c = LispLibrary[GlobalStringIndex++];
  return (c != 0) ? c : -1; // -1?
}

void loadfromlibrary (object *env) {
  GlobalStringIndex = 0;
  object *line = read(glibrary);
  while (line != NULL) {
    eval(line, env);
    line = read(glibrary);
  }
}

object *fn_require (object *args, object *env) {
  object *arg = first_(args);
  object *globals = GlobalEnv;
  if (!symbolp_(arg)) error(REQUIRE, PSTR("argument is not a symbol"), arg);
  while (globals != NULL) {
    object *pair = first_(globals);
    object *var = car_(pair);
    if (symbolp_(var) && var == arg) return nil_;
    globals = cdr_(globals);
  }
  GlobalStringIndex = 0;
  object *line = read(glibrary);
  while (line != NULL) {
    // Is this the definition we want
    int fname = first_(line)->name;
    if ((fname == DEFUN || fname == DEFVAR) && symbolp_(second_(line)) && second_(line)->name == arg->name) {
      eval(line, env);
      return tee;
    }
    line = read(glibrary);
  }
  return nil_;
}

object *fn_listlibrary (object *args, object *env) {
  (void) args, (void) env;
  GlobalStringIndex = 0;
  object *line = read(glibrary);
  while (line != NULL) {
    int fname = first_(line)->name;
    if (fname == DEFUN || fname == DEFVAR) {
      pstring(symbolname(second_(line)->name), pserial); pserial(' ');
    }
    line = read(glibrary);
  }
  return symbol(NOTHING);
}
#endif
