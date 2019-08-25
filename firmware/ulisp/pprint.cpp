#include "pprint.h"
#include "print.h"
#include "stream.h"

// Pretty printer

const int PPINDENT = 2;
const int PPWIDTH = 80;

void pcount (char c) {
  LastPrint = c;
  if (c == '\n') GlobalStringIndex++;
  GlobalStringIndex++;
}

int atomwidth (object *obj) {
  GlobalStringIndex = 0;
  printobject(obj, pcount);
  return GlobalStringIndex;
}

bool quoted (object *obj) {
  return (consp(obj) && car_(obj) != NULL && car_(obj)->name == QUOTE && consp(cdr_(obj)) && cddr_(obj) == NULL);
}

int subwidth (object *obj, int w) {
  if (atom(obj)) return w - atomwidth(obj);
  if (quoted(obj)) return subwidthlist(car_(cdr_(obj)), w - 1);
  return subwidthlist(obj, w - 1);
}

int subwidthlist (object *form, int w) {
  while (form != NULL && w >= 0) {
    if (atom(form)) return w - (2 + atomwidth(form));
    w = subwidth(car_(form), w - 1);
    form = cdr_(form);
  }
  return w;
}

void superprint (object *form, int lm, pfun_t pfun) {
  if (atom(form)) {
    if (symbolp_(form) && form->name == NOTHING) pstring(symbolname(form->name), pfun);
    else printobject(form, pfun);
  }
  else if (quoted(form)) { pfun('\''); superprint(car_(cdr_(form)), lm + 1, pfun); }
  else if (subwidth(form, PPWIDTH - lm) >= 0) supersub(form, lm + PPINDENT, 0, pfun);
  else supersub(form, lm + PPINDENT, 1, pfun);
}

static const int ppspecials = 15;
static const char ppspecial[ppspecials] PROGMEM =
  { DOTIMES, DOLIST, IF, SETQ, TEE, LET, LETSTAR, LAMBDA, WHEN, UNLESS, WITHI2C, WITHSERIAL, WITHSPI, WITHSDCARD, FORMILLIS };

void supersub (object *form, int lm, int super, pfun_t pfun) {
  int special = 0, separate = 1;
  object *arg = car_(form);
  if (symbolp_(arg)) {
    int name = arg->name;
    if (name == DEFUN) special = 2;
    else for (int i=0; i<ppspecials; i++) {
      if (name == ppspecial[i]) { special = 1; break; }
    }
  }
  while (form != NULL) {
    if (atom(form)) { pfstring(PSTR(" . "), pfun); printobject(form, pfun); pfun(')'); return; }
    else if (separate) { pfun('('); separate = 0; }
    else if (special) { pfun(' '); special--; }
    else if (!super) pfun(' ');
    else { pln(pfun); indent(lm, pfun); }
    superprint(car_(form), lm, pfun);
    form = cdr_(form);
  }
  pfun(')'); return;
}

object *fn_pprint (object *args, object *env) {
  (void) env;
  object *obj = first_(args);
  pfun_t pfun = pstreamfun(cdr_(args));
  pln(pfun);
  superprint(obj, 0, pfun);
  return symbol(NOTHING);
}

object *fn_pprintall (object *args, object *env) {
  (void) args, (void) env;
  object *globals = GlobalEnv;
  while (globals != NULL) {
    object *pair = first_(globals);
    object *var = car_(pair);
    object *val = cdr_(pair);
    pln(pserial);
    if (consp(val) && symbolp_(car_(val)) && car_(val)->name == LAMBDA) {
      superprint(cons(symbol(DEFUN), cons(var, cdr_(val))), 0, pserial);
    } else {
      superprint(cons(symbol(DEFVAR),cons(var,cons(cons(symbol(QUOTE),cons(val,NULL))
      ,NULL))), 0, pserial);
    }
    pln(pserial);
    globals = cdr_(globals);
  }
  return symbol(NOTHING);
}
