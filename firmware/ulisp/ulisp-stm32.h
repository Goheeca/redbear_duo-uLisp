#ifndef __ULISP_H
#define __ULISP_H

#include <Particle.h>

#define PROGMEM
typedef int BitOrder;
typedef PinMode WiringPinMode;

// Compile options

// #define resetautorun
#define printfreespace
#define serialmonitor
// #define printgcs
// #define sdcardsupport
// #define lisplibrary


#include <stdint.h>
#include <setjmp.h>
typedef uint16_t uint16;

extern uint8_t End;
extern jmp_buf exception;

void autorunimage ();

#define END 0xA5 // Canary to check stack
#if defined(resetautorun)
#define AUTORUN 12 // Fudge to keep code size the same
#else
#define AUTORUN 13
#endif

// Typedefs

typedef unsigned int symbol_t;

typedef struct sobject {
  union {
    struct {
      sobject *car;
      sobject *cdr;
    };
    struct {
      unsigned int type;
      union {
        symbol_t name;
        int integer;
        float single_float;
      };
    };
  };
} object;

typedef object *(*fn_ptr_type)(object *, object *);

typedef struct {
  const char *string;
  fn_ptr_type fptr;
  uint8_t min;
  uint8_t max;
} tbl_entry_t;

typedef int (*gfun_t)();
typedef void (*pfun_t)(char);
typedef int PinMode_;

// API

#define ULISP_SETUP_EXCEPTION_HANDLING \
  End = END; \
  if (!setjmp(exception)) { \
    volatile int autorun = AUTORUN; \
    if (autorun == 12) autorunimage(); \
  }

void ulisp_setup ();
void ulisp_reset ();
void repl (object *env);

// Forward declarations

inline int maxbuffer (char *buffer);
char nthchar (object *string, int n);
void pserial (char c);
void pfstring (const char *s, pfun_t pfun);
void pint (int i, pfun_t pfun);
inline void pln (pfun_t pfun);
object *apply (symbol_t name, object *function, object *args, object *env);
void pfl (pfun_t pfun);
char *symbolname (symbol_t x);
void pstring (char *s, pfun_t pfun);
char *lookupsymbol (symbol_t name);
int listlength (symbol_t name, object *list);
uint8_t lookupmin (symbol_t name);
uint8_t lookupmax (symbol_t name);
int gserial ();
void testescape ();
inline int serial1read () { while (!Serial1.available()) testescape(); return Serial1.read(); }
void printstring (object *form, pfun_t pfun);
object *edit (object *fun);
void superprint (object *form, int lm, pfun_t pfun);
int subwidthlist (object *form, int w);
void supersub (object *form, int lm, int super, pfun_t pfun);
int glibrary ();
object *read (gfun_t gfun);

#endif
