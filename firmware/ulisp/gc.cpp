#include "gc.h"
#include "print.h"
#include "hw/platform.h"

// Garbage collection

void markobject (object *obj) {
  MARK:
  if (obj == NULL) return;
  if (marked_(obj)) return;

  object* arg = car_(obj);
  unsigned int type = obj->type;
  mark_(obj);

  if (type >= PAIR_TAG || type == ZERO_TAG) { // cons
    markobject(arg);
    obj = cdr_(obj);
    goto MARK;
  }

  if (type == STRING_TAG) {
    obj = cdr_(obj);
    while (obj != NULL) {
      arg = car_(obj);
      mark_(obj);
      obj = arg;
    }
  }
}

void sweep () {
  Freelist = NULL;
  Freespace = 0;
  for (int i=WORKSPACESIZE-1; i>=0; i--) {
    object *obj = &Workspace[i];
    if (!marked_(obj)) myfree(obj); else unmark_(obj);
  }
}

void gc (object *form, object *env) {
  #if defined(printgcs)
  int start = Freespace;
  #endif
  markobject(tee);
  markobject(GlobalEnv);
  markobject(GCStack);
  markobject(form);
  markobject(env);
  sweep();
  #if defined(printgcs)
  pfl(pserial); pserial('{'); pint(Freespace - start, pserial); pserial('}');
  #endif
}

object *fn_gc (object *obj, object *env) {
  int initial = Freespace;
  unsigned long start = micros();
  gc(obj, env);
  unsigned long elapsed = micros() - start;
  pfstring(PSTR("Space: "), pserial);
  pint(Freespace - initial, pserial);
  pfstring(PSTR(" bytes, Time: "), pserial);
  pint(elapsed, pserial);
  pfstring(PSTR(" us\r"), pserial);
  return nil_;
}

object *fn_room (object *args, object *env) {
  (void) args, (void) env;
  return number(Freespace);
}
