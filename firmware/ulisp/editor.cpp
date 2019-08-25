#include "editor.h"
#include "bus/serial.h"
#include "read.h"
#include "eval.h"
#include "print.h"
#include "pprint.h"

// Tree Editor

object *fn_edit (object *args, object *env) {
  object *fun = first_(args);
  object *pair = findvalue(fun, env);
  clrflag_(EXITEDITOR);
  object *arg = edit(eval(fun, env));
  cdr_(pair) = arg;
  return arg;
}

object *edit (object *fun) {
  while (1) {
    if (tstflag_(EXITEDITOR)) return fun;
    char c = gserial();
    if (c == 'q') setflag_(EXITEDITOR);
    else if (c == 'b') return fun;
    else if (c == 'r') fun = read(gserial);
    else if (c == '\n') { pfl(pserial); superprint(fun, 0, pserial); pln(pserial); }
    else if (c == 'c') fun = cons(read(gserial), fun);
    else if (atom(fun)) pserial('!');
    else if (c == 'd') fun = cons(car_(fun), edit(cdr_(fun)));
    else if (c == 'a') fun = cons(edit(car_(fun)), cdr_(fun));
    else if (c == 'x') fun = cdr_(fun);
    else pserial('?');
  }
}
