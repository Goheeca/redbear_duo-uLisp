#include "lookup.h"
#include <string.h>

#include "../special_forms.h"
#include "../tail_call.h"
#include "../pprint.h"
#include "../bus/i2c.h"

// Insert your own function definitions here

// Built-in procedure names - stored in PROGMEM

#include "../list.h"
#include "../arithmetics.h"
#include "../char_string.h"
#include "../logic.h"
#include "../gc.h"
#include "../hw/machine.h"
#include "../editor.h"
#include "../lisp_library.h"

FREEZE(core_, nil_, "nil_");
FREEZE(core_, t, "t");
FREEZE(core_, nothing, "nothing");
FREEZE(core_, ampoptional, "&optional");
FREEZE(core_, amprest, "&rest");
FREEZE(core_, lambda, "lambda");
FREEZE(core_, let, "let");
FREEZE(core_, letstar, "let*");
FREEZE(core_, closure, "closure");

FREEZE(core_, special_forms, "special_forms");

static const char string10[] PROGMEM = "quote";
static const char string11[] PROGMEM = "defun";
static const char string12[] PROGMEM = "defvar";
static const char string13[] PROGMEM = "setq";
static const char string14[] PROGMEM = "loop";
static const char string15[] PROGMEM = "push_";
static const char string16[] PROGMEM = "pop_";
static const char string17[] PROGMEM = "incf";
static const char string18[] PROGMEM = "decf";
static const char string19[] PROGMEM = "setf";
static const char string20[] PROGMEM = "dolist";
static const char string21[] PROGMEM = "dotimes";
static const char string22[] PROGMEM = "trace";
static const char string23[] PROGMEM = "untrace";
static const char string24[] PROGMEM = "for-millis";
static const char string25[] PROGMEM = "with-serial";
static const char string26[] PROGMEM = "with-i2c";
static const char string27[] PROGMEM = "with-spi";
static const char string28[] PROGMEM = "with-sd-card";

FREEZE(core_, tail_forms, "tail_forms");

static const char string30[] PROGMEM = "progn";
static const char string31[] PROGMEM = "return";
static const char string32[] PROGMEM = "if";
static const char string33[] PROGMEM = "cond";
static const char string34[] PROGMEM = "when";
static const char string35[] PROGMEM = "unless";
static const char string36[] PROGMEM = "case";
static const char string37[] PROGMEM = "and";
static const char string38[] PROGMEM = "or";

FREEZE(core_, functions, "functions");

static const char string40[] PROGMEM = "not";
static const char string41[] PROGMEM = "null";
static const char string42[] PROGMEM = "cons";
static const char string43[] PROGMEM = "atom";
static const char string44[] PROGMEM = "listp";
static const char string45[] PROGMEM = "consp";
static const char string46[] PROGMEM = "symbolp_";
static const char string47[] PROGMEM = "streamp_";
static const char string48[] PROGMEM = "eq";

FREEZE_LIST

FREEZE_ARITHMETICS

FREEZE_CHAR_STRING

FREEZE_LOGIC

static const char string143[] PROGMEM = "eval";
static const char string144[] PROGMEM = "globals";
static const char string145[] PROGMEM = "locals";
static const char string146[] PROGMEM = "makunbound";
static const char string147[] PROGMEM = "break";
static const char string148[] PROGMEM = "read";
static const char string149[] PROGMEM = "prin1";
static const char string150[] PROGMEM = "print";
static const char string151[] PROGMEM = "princ";
static const char string152[] PROGMEM = "terpri";
static const char string153[] PROGMEM = "read-byte";
static const char string154[] PROGMEM = "read-line";
static const char string155[] PROGMEM = "write-byte";
static const char string156[] PROGMEM = "write-string";
static const char string157[] PROGMEM = "write-line";
static const char string158[] PROGMEM = "restart-i2c";
static const char string159[] PROGMEM = "gc";
static const char string160[] PROGMEM = "room";
static const char string161[] PROGMEM = "save-image";
static const char string162[] PROGMEM = "load-image";
static const char string163[] PROGMEM = "cls";

FREEZE_MACHINE

static const char string173[] PROGMEM = "edit";
static const char string174[] PROGMEM = "pprint";
static const char string175[] PROGMEM = "pprintall";
#if defined(lisplibrary)
static const char string176[] PROGMEM = "require";
static const char string177[] PROGMEM = "list-library";
#endif

static const tbl_entry_t lookup_table[] PROGMEM = {
  REIFY(core_, nil_, NULL, 0, 0),
  REIFY(core_, t, NULL, 0, 0),
  REIFY(core_, nothing, NULL, 0, 0),
  REIFY(core_, ampoptional, NULL, 0, 0),
  REIFY(core_, amprest, NULL, 0, 0),
  REIFY(core_, lambda, NULL, 0, CALL_ARGUMENTS_LIMIT),
  REIFY(core_, let, NULL, 0, CALL_ARGUMENTS_LIMIT),
  REIFY(core_, letstar, NULL, 0, CALL_ARGUMENTS_LIMIT),
  REIFY(core_, closure, NULL, 0, CALL_ARGUMENTS_LIMIT),

  REIFY_SPECIAL(core_special_forms),

  { string10, sp_quote, 1, 1 },
  { string11, sp_defun, 0, 127 },
  { string12, sp_defvar, 2, 2 },
  { string13, sp_setq, 2, 2 },
  { string14, sp_loop, 0, 127 },
  { string15, sp_push_, 2, 2 },
  { string16, sp_pop_, 1, 1 },
  { string17, sp_incf, 1, 2 },
  { string18, sp_decf, 1, 2 },
  { string19, sp_setf, 2, 2 },
  { string20, sp_dolist, 1, 127 },
  { string21, sp_dotimes, 1, 127 },
  { string22, sp_trace, 0, 1 },
  { string23, sp_untrace, 0, 1 },
  { string24, sp_formillis, 1, 127 },
  { string25, sp_withserial, 1, 127 },
  { string26, sp_withi2c, 1, 127 },
  { string27, sp_withspi, 1, 127 },
  { string28, sp_withsdcard, 2, 127 },

  REIFY_SPECIAL(core_tail_forms),

  { string30, tf_progn, 0, 127 },
  { string31, tf_return, 0, 127 },
  { string32, tf_if, 2, 3 },
  { string33, tf_cond, 0, 127 },
  { string34, tf_when, 1, 127 },
  { string35, tf_unless, 1, 127 },
  { string36, tf_case, 1, 127 },
  { string37, tf_and, 0, 127 },
  { string38, tf_or, 0, 127 },

  REIFY_SPECIAL(core_functions),

  { string40, fn_not, 1, 1 },
  { string41, fn_not, 1, 1 },
  { string42, fn_cons, 2, 2 },
  { string43, fn_atom, 1, 1 },
  { string44, fn_listp, 1, 1 },
  { string45, fn_consp, 1, 1 },
  { string46, fn_symbolp, 1, 1 },
  { string47, fn_streamp, 1, 1 },
  { string48, fn_eq, 2, 2 },

  REIFY_LIST

  REIFY_ARITHMETICS

  REIFY_CHAR_STING

  REIFY_LOGIC

  { string143, fn_eval, 1, 1 },
  { string144, fn_globals, 0, 0 },
  { string145, fn_locals, 0, 0 },
  { string146, fn_makunbound, 1, 1 },
  { string147, fn_break, 0, 0 },
  { string148, fn_read, 0, 1 },
  { string149, fn_prin1, 1, 2 },
  { string150, fn_print, 1, 2 },
  { string151, fn_princ, 1, 2 },
  { string152, fn_terpri, 0, 1 },
  { string153, fn_readbyte, 0, 2 },
  { string154, fn_readline, 0, 1 },
  { string155, fn_writebyte, 1, 2 },
  { string156, fn_writestring, 1, 2 },
  { string157, fn_writeline, 1, 2 },
  { string158, fn_restarti2c, 1, 2 },
  { string159, fn_gc, 0, 0 },
  { string160, fn_room, 0, 0 },
  { string161, fn_saveimage, 0, 1 },
  { string162, fn_loadimage, 0, 1 },
  { string163, fn_cls, 0, 0 },

  REIFY_MACHINE

  { string173, fn_edit, 1, 1 },
  { string174, fn_pprint, 1, 2 },
  { string175, fn_pprintall, 0, 0 },
#if defined(lisplibrary)
  { string176, fn_require, 1, 1 },
  { string177, fn_listlibrary, 0, 0 },
#endif
};

// Table lookup functions

int builtin (char* n) {
  int entry = 0;
  while (entry < ENDFUNCTIONS) {
    if (strcasecmp(n, (char*)lookup_table[entry].string) == 0)
      return entry;
    entry++;
  }
  return ENDFUNCTIONS;
}

intptr_t lookupfn (symbol_t name) {
  return (intptr_t)lookup_table[name].fptr;
}

uint8_t lookupmin (symbol_t name) {
  return lookup_table[name].min;
}

uint8_t lookupmax (symbol_t name) {
  return lookup_table[name].max;
}

char *lookupbuiltin (symbol_t name) {
  char *buffer = SymbolTop;
  strcpy(buffer, (char *)lookup_table[name].string);
  return buffer;
}
