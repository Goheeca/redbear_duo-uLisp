#ifndef __LOOKUP_H
#define __LOOKUP_H

#include "../core.h"

#define CALL_ARGUMENTS_LIMIT 127

#define FREEZE(prefix, no, str) \
    static const char prefix##no[] PROGMEM = str

#define REIFY(prefix, no, fn, min_arg, max_arg) \
    { prefix##no, fn, min_arg, max_arg }

#define REIFY_SPECIAL(name) \
    { name, NULL, NIL, NIL}

// Table lookup functions

int builtin (char* n);
intptr_t lookupfn (symbol_t name);
uint8_t lookupmin (symbol_t name);
uint8_t lookupmax (symbol_t name);
char *lookupbuiltin (symbol_t name);

#endif
