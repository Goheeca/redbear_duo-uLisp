#ifndef __ULISP_H
#define __ULISP_H

#include <Particle.h>
#include <avr/pgmspace.h>

typedef int BitOrder;

// Compile options

// #define resetautorun
#define printfreespace
// #define printgcs
// #define sdcardsupport
// #define gfxsupport
#define lisplibrary
#define assemblerlist
#define lineeditor
#define vt100

#include <stdint.h>
#include <setjmp.h>
typedef uint16_t uint16;

extern uint8_t End;
extern jmp_buf toplevel_handler;

void autorunimage ();

#define END 0xA5 // Canary to check stack
#if defined(resetautorun)
#define AUTORUN 12 // Fudge to keep code size the same
#else
#define AUTORUN 13
#endif


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
#define arrayp(x)          ((x) != NULL && (x)->type == ARRAY)
#define streamp(x)         ((x) != NULL && (x)->type == STREAM)

#define mark(x)            (car(x) = (object *)(((uintptr_t)(car(x))) | MARKBIT))
#define unmark(x)          (car(x) = (object *)(((uintptr_t)(car(x))) & ~MARKBIT))
#define marked(x)          ((((uintptr_t)(car(x))) & MARKBIT) != 0)
#define MARKBIT            1

#define setflag(x)         (Flags_ = Flags_ | 1<<(x))
#define clrflag(x)         (Flags_ = Flags_ & ~(1<<(x)))
#define tstflag(x)         (Flags_ & 1<<(x))

#define issp(x)            (x == ' ' || x == '\n' || x == '\r' || x == '\t')

// Code marker stores start and end of code block
#define startblock(x)      ((x->integer) & 0xFFFF)
#define endblock(x)        ((x->integer) >> 16 & 0xFFFF)

// Constants

const int TRACEMAX = 3; // Number of traced functions
enum type { ZZERO=0, SYMBOL=2, CODE=4, NUMBER=6, STREAM=8, CHARACTER=10, FLOAT=12, ARRAY=14, STRING_=16, PAIR=18 };  // ARRAY STRING and PAIR must be last
enum token { UNUSED, BRA, KET, QUO, DOT };
enum stream { SERIALSTREAM, I2CSTREAM, SPISTREAM, SDSTREAM, STRINGSTREAM, GFXSTREAM };

// Stream names used by printobject
const char serialstream[] PROGMEM = "serial";
const char i2cstream[] PROGMEM = "i2c";
const char spistream[] PROGMEM = "spi";
const char sdstream[] PROGMEM = "sd";
const char stringstream[] PROGMEM = "string";
const char gfxstream[] PROGMEM = "gfx";
const char *const streamname[] PROGMEM = {serialstream, i2cstream, spistream, sdstream, stringstream, gfxstream};

enum function { NIL, TEE, NOTHING, OPTIONAL, INITIALELEMENT, ELEMENTTYPE, BIT, AMPREST, LAMBDA, LET,
LETSTAR, CLOSURE, SPECIAL_FORMS, QUOTE, DEFUN, DEFVAR, SETQ, LOOP, RETURN, PUSH, POP, INCF, DECF, SETF,
DOLIST, DOTIMES, TRACE, UNTRACE, FORMILLIS, WITHOUTPUTTOSTRING, WITHSERIAL, WITHI2C, WITHSPI, WITHSDCARD,
WITHGFX, DEFCODE, UNWINDPROTECT, IGNOREERRORS, SP_ERROR, TAIL_FORMS, PROGN, IF, COND, WHEN, UNLESS, CASE, AND, OR, FUNCTIONS, NOT, NULLFN, CONS,
ATOM, LISTP, CONSP, SYMBOLP, ARRAYP, BOUNDP, SETFN, STREAMP, EQ, CAR, FIRST, CDR, REST, CAAR, CADR,
SECOND, CDAR, CDDR, CAAAR, CAADR, CADAR, CADDR, THIRD, CDAAR, CDADR, CDDAR, CDDDR, LENGTH,
ARRAYDIMENSIONS, LIST, MAKEARRAY, REVERSE, NTH, AREF, ASSOC, MEMBER, APPLY, FUNCALL, APPEND, MAPC, MAPCAR,
MAPCAN, ADD, SUBTRACT, MULTIPLY, DIVIDE, MOD, ONEPLUS, ONEMINUS, ABS, RANDOM, MAXFN, MINFN, NOTEQ, NUMEQ,
LESS, LESSEQ, GREATER, GREATEREQ, PLUSP, MINUSP, ZEROP, ODDP, EVENP, INTEGERP, NUMBERP, FLOATFN, FLOATP,
SIN, COS, TAN, ASIN, ACOS, ATAN, SINH, COSH, TANH, EXP, SQRT, LOG, EXPT, CEILING, FLOOR, TRUNCATE, ROUND,
CHAR, CHARCODE, CODECHAR, CHARACTERP, STRINGP, STRINGEQ, STRINGLESS, STRINGGREATER, SORT, STRINGFN,
CONCATENATE, SUBSEQ, READFROMSTRING, PRINCTOSTRING, PRIN1TOSTRING, LOGAND, LOGIOR, LOGXOR, LOGNOT, ASH,
LOGBITP, EVAL, GLOBALS, LOCALS, MAKUNBOUND, BREAK, READ, PRIN1, PRINT, PRINC, TERPRI, READBYTE, READLINE,
WRITEBYTE, WRITESTRING, WRITELINE, RESTARTI2C, GC, ROOM, SAVEIMAGE, LOADIMAGE, CLS, PINMODE, DIGITALREAD,
DIGITALWRITE, ANALOGREAD, ANALOGWRITE, DELAY, MILLIS, SLEEP, NOTE, EDIT, PPRINT, PPRINTALL, FORMAT,
REQUIRE, LISTLIBRARY, DRAWPIXEL, DRAWLINE, DRAWRECT, FILLRECT, DRAWCIRCLE, FILLCIRCLE, DRAWROUNDRECT,
FILLROUNDRECT, DRAWTRIANGLE, FILLTRIANGLE, DRAWCHAR, SETCURSOR, SETTEXTCOLOR, SETTEXTSIZE, SETTEXTWRAP,
FILLSCREEN, SETROTATION, INVERTDISPLAY, _ENDFUNCTIONS };

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
        int chars; // For strings
        float single_float;
      };
    };
  };
} object;

typedef object *(*fn_ptr_type)(object *, object *);
typedef void (*mapfun_t)(object *, object **);
typedef int (*intfn_ptr_type)(int w, int x, int y, int z);

typedef struct {
  const char *string;
  fn_ptr_type fptr;
  uint8_t minmax;
} tbl_entry_t;

typedef int (*gfun_t)();
typedef void (*pfun_t)(char);
typedef int PinMode_;

// API

#define ULISP_SETUP_EXCEPTION_HANDLING \
  End = END; \
  if (!setjmp(toplevel_handler)) { \
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
void checkminmax (symbol_t name, int nargs);
void pinthex (uint32_t i, pfun_t pfun);
void printhex4 (int i, pfun_t pfun);
void prin1object (object *form, pfun_t pfun);

// Forward references

extern object *tee;
object *tf_progn (object *form, object *env);
object *eval (object *form, object *env);
object *read (gfun_t gfun);
void repl(object *env);
void prin1object (object *form, pfun_t pfun);
void printobject (object *form, pfun_t pfun);
char *lookupbuiltin (symbol_t name);
intptr_t lookupfn (symbol_t name);
int builtin (char* n);
void error (symbol_t fname, PGM_P string, object *symbol);
void error2 (symbol_t fname, PGM_P string);
object *newsymbol (symbol_t name);
int longsymbol (char *buffer);
int pack40 (const char *buffer);

object *number (int n);
object *makefloat (float f);
object *character (char c);

#define LOOKUP_TABLE_ENTRIES

#endif
