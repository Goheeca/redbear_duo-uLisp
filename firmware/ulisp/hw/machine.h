#ifndef __MACHINE_H
#define __MACHINE_H

#include "../core.h"

// Sleep

void initsleep ();
void sleep (int secs);

// Arduino procedures

object *fn_pinmode (object *args, object *env);
object *fn_digitalread (object *args, object *env);
object *fn_digitalwrite (object *args, object *env);
object *fn_analogread (object *args, object *env);
object *fn_analogwrite (object *args, object *env);
object *fn_delay (object *args, object *env);
object *fn_millis (object *args, object *env);
object *fn_sleep (object *args, object *env);
object *fn_note (object *args, object *env);

#include "../frozen/lookup.h"
#define FREEZE_MA(no, str) \
    FREEZE(machine_, no, str)
#define REIFY_MA(no, fn, min_arg, max_arg) \
    REIFY(machine_, no, fn, min_arg, max_arg)

#define FREEZE_MACHINE \
    FREEZE_MA(pinmode, "pinmode"); \
    FREEZE_MA(digitalread, "digitalread"); \
    FREEZE_MA(digitalwrite, "digitalwrite"); \
    FREEZE_MA(analogread, "analogread"); \
    FREEZE_MA(analogwrite, "analogwrite"); \
    FREEZE_MA(delay, "delay"); \
    FREEZE_MA(millis, "millis"); \
    FREEZE_MA(sleep, "sleep"); \
    FREEZE_MA(note, "note");

#define REIFY_MACHINE \
    REIFY_MA(pinmode, fn_pinmode, 2, 2), \
    REIFY_MA(digitalread, fn_digitalread, 1, 1), \
    REIFY_MA(digitalwrite, fn_digitalwrite, 2, 2), \
    REIFY_MA(analogread, fn_analogread, 1, 1), \
    REIFY_MA(analogwrite, fn_analogwrite, 2, 2), \
    REIFY_MA(delay, fn_delay, 1, 1), \
    REIFY_MA(millis, fn_millis, 0, 0), \
    REIFY_MA(sleep, fn_sleep, 1, 1), \
    REIFY_MA(note, fn_note, 0, 3),

#endif
