#ifndef __ULISP_H
#define __ULISP_H

#include <Particle.h>
#include <avr/pgmspace.h>

//#define PROGMEM
typedef int BitOrder;
typedef PinMode WiringPinMode;

// Compile options

// #define resetautorun
#define printfreespace
#define serialmonitor
// #define printgcs
// #define sdcardsupport
#define lisplibrary

// Lisp Library
extern const char LispLibrary[] PROGMEM;


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

// C Macros

#define nil                NULL
#define car(x)             (((object *) (x))->car)
#define cdr(x)             (((object *) (x))->cdr)

#define first(x)           (((object *) (x))->car)
#define second(x)          (car(cdr(x)))
#define cddr(x)            (cdr(cdr(x)))
#define third(x)           (car(cdr(cdr(x))))

#define push(x, y)         ((y) = cons((x),(y)))
#define pop(y)             ((y) = cdr(y))

#define integerp(x)        ((x) != NULL && (x)->type == NUMBER)
#define floatp(x)          ((x) != NULL && (x)->type == FLOAT)
#define symbolp(x)         ((x) != NULL && (x)->type == SYMBOL)
#define stringp(x)         ((x) != NULL && (x)->type == STRING_)
#define characterp(x)      ((x) != NULL && (x)->type == CHARACTER)
#define streamp(x)         ((x) != NULL && (x)->type == STREAM)

#define mark(x)            (car(x) = (object *)(((uintptr_t)(car(x))) | MARKBIT))
#define unmark(x)          (car(x) = (object *)(((uintptr_t)(car(x))) & ~MARKBIT))
#define marked(x)          ((((uintptr_t)(car(x))) & MARKBIT) != 0)
#define MARKBIT            1

#define setflag(x)         (Flags_ = Flags_ | 1<<(x))
#define clrflag(x)         (Flags_ = Flags_ & ~(1<<(x)))
#define tstflag(x)         (Flags_ & 1<<(x))

// Constants

const int TRACEMAX = 3; // Number of traced functions
enum type { ZERO=0, SYMBOL=2, NUMBER=4, STREAM=6, CHARACTER=8, FLOAT=10, STRING_=12, PAIR=14 };  // STRING_ and PAIR must be last
enum token { UNUSED, BRA, KET, QUO, DOT };
enum stream { SERIALSTREAM, I2CSTREAM, SPISTREAM, SDSTREAM };

enum function { NIL, TEE, NOTHING, OPTIONAL, AMPREST, LAMBDA, LET, LETSTAR, CLOSURE, SPECIAL_FORMS, QUOTE,
DEFUN, DEFVAR, SETQ, LOOP, RETURN, PUSH, POP, INCF, DECF, SETF, DOLIST, DOTIMES, TRACE, UNTRACE,
FORMILLIS, WITHSERIAL, WITHI2C, WITHSPI, WITHSDCARD, TAIL_FORMS, PROGN, IF, COND, WHEN, UNLESS, CASE, AND,
OR, FUNCTIONS, NOT, NULLFN, CONS, ATOM, LISTP, CONSP, SYMBOLP, STREAMP, EQ, CAR, FIRST, CDR, REST, CAAR,
CADR, SECOND, CDAR, CDDR, CAAAR, CAADR, CADAR, CADDR, THIRD, CDAAR, CDADR, CDDAR, CDDDR, LENGTH, LIST,
REVERSE, NTH, ASSOC, MEMBER, APPLY, FUNCALL, APPEND, MAPC, MAPCAR, MAPCAN, ADD, SUBTRACT, MULTIPLY,
DIVIDE, MOD, ONEPLUS, ONEMINUS, ABS, RANDOM, MAXFN, MINFN, NOTEQ, NUMEQ, LESS, LESSEQ, GREATER, GREATEREQ,
PLUSP, MINUSP, ZEROP, ODDP, EVENP, INTEGERP, NUMBERP, FLOATFN, FLOATP, SIN, COS, TAN, ASIN, ACOS, ATAN,
SINH, COSH, TANH, EXP, SQRT, LOG, EXPT, CEILING, FLOOR, TRUNCATE, ROUND, CHAR, CHARCODE, CODECHAR,
CHARACTERP, STRINGP, STRINGEQ, STRINGLESS, STRINGGREATER, SORT, STRINGFN, CONCATENATE, SUBSEQ,
READFROMSTRING, PRINCTOSTRING, PRIN1TOSTRING, LOGAND, LOGIOR, LOGXOR, LOGNOT, ASH, LOGBITP, EVAL, GLOBALS,
LOCALS, MAKUNBOUND, BREAK, READ, PRIN1, PRINT, PRINC, TERPRI, READBYTE, READLINE, WRITEBYTE, WRITESTRING,
WRITELINE, RESTARTI2C, GC, ROOM, SAVEIMAGE, LOADIMAGE, CLS, PINMODE, DIGITALREAD, DIGITALWRITE,
ANALOGREAD, ANALOGWRITE, DELAY, MILLIS, SLEEP, NOTE, EDIT, PPRINT, PPRINTALL, REQUIRE, LISTLIBRARY, _ENDFUNCTIONS };

// Forward references

object *tf_progn (object *form, object *env);
object *eval (object *form, object *env);
object *read ();
void repl(object *env);
void printobject (object *form, pfun_t pfun);
char *lookupbuiltin (symbol_t name);
intptr_t lookupfn (symbol_t name);
int builtin (char* n);
void error (symbol_t fname, PGM_P string, object *symbol);
void error2 (symbol_t fname, PGM_P string);

#define LOOKUP_TABLE_ENTRIES

#endif
