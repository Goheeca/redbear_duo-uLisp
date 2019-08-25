#ifndef __ULISP_H
#define __ULISP_H

//Modules

#include "core.h"
#include "arithmetics.h"
#include "logic.h"
#include "gc.h"
#include "image.h"
#include "special_forms.h"
#include "list.h"
#include "print.h"
#include "read.h"
#include "tail_call.h"
#include "eval.h"
#include "string.h"
#include "pprint.h"
#include "repl.h"
#include "hw/machine.h"

#define END 0xA5 // Canary to check stack
#if defined(resetautorun)
#define AUTORUN 12 // Fudge to keep code size the same
#else
#define AUTORUN 13
#endif

#define ULISP_SETUP_EXCEPTION_HANDLING \
  End = END; \
  if (!setjmp(exception)) { \
    volatile int autorun = AUTORUN; \
    if (autorun == 12) autorunimage(); \
  }

void ulisp_setup ();
void ulisp_reset ();
void ulisp_repl ();

#endif
