#ifndef __CORE_H
#define __CORE_H

/* uLisp STM32 Version 2.8a - www.ulisp.com
   David Johnson-Davies - www.technoblogy.com - 6th August 2019

   Licensed under the MIT license: https://opensource.org/licenses/MIT
*/

#include "config.h"

// Includes

#include <stddef.h>
#include <setjmp.h>
#include <limits.h>

// Core Modules

#include "definitions.h"
#include "frozen/lookup.h"
#include "frozen/function_symbols.h"
#include "error.h"

// Declarations

#if defined(sdcardsupport)
#define SDSIZE 172
#else
#define SDSIZE 0
#endif

#if defined(_BOARD_MAPLE_MINI_H_) || defined(_VARIANT_ARDUINO_STM32_)
#define SERIAL1
#endif

// Workspace
//#define PERSIST __attribute__((section(".text")))
#define WORDALIGNED __attribute__((aligned (4)))
#define BUFFERSIZE 34  // Number of bits+2

#if defined(REDBEAR_DUO)
  //#include "pgmspace_dummy.h"

  //#define WORKSPACESIZE 1130-SDSIZE       /* Cells (8*bytes) */
  #define WORKSPACESIZE 1130              /* Cells (8*bytes) */
  #define SYMBOLTABLESIZE 512             /* Bytes - must be even*/
  #define EEPROMSIZE (184*4096)
  extern uint8_t _end;

#endif

extern object Workspace[WORKSPACESIZE] WORDALIGNED;
extern char SymbolTable[SYMBOLTABLESIZE];

// Global variables

extern jmp_buf exception;
extern unsigned int Freespace;
extern object *Freelist;
extern char *SymbolTop;
extern unsigned int TraceFn[TRACEMAX];
extern unsigned int TraceDepth[TRACEMAX];

extern object *GlobalEnv;
extern object *GCStack;
extern object *GlobalString;
extern int GlobalStringIndex;
extern char BreakLevel;
extern char LastChar;
extern char LastPrint;
extern char PrintReadably;

// Flags
enum flag { RETURNFLAG, ESCAPE, EXITEDITOR, LIBRARYLOADED };
extern volatile char Flags_;

// Forward references
extern object *tee;
/*object *tf_progn (object *form, object *env);
object *eval (object *form, object *env);
object *read ();
void repl(object *env);
void printobject (object *form, pfun_t pfun);
char *lookupbuiltin (symbol_t name);
intptr_t lookupfn (symbol_t name);
int builtin (char* n);*/
void error (symbol_t fname, PGM_P string, object *symbol);
void error2 (symbol_t fname, PGM_P string);

inline int maxbuffer (char *buffer) {
  return SYMBOLTABLESIZE-(buffer-SymbolTable)-1;
}

// Lookup

int longsymbol (char *buffer);
char *lookupsymbol (symbol_t name);
void deletesymbol (symbol_t name);

// Set up workspace

void initworkspace ();
object *myalloc ();

inline void myfree (object *obj) {
  car_(obj) = NULL;
  cdr_(obj) = Freelist;
  Freelist = obj;
  Freespace++;
}

// Make each type of object

object *number (int n);
object *makefloat (float f);
object *character (char c);
object *cons (object *arg1, object *arg2);
object *symbol (symbol_t name);
object *newsymbol (symbol_t name);
object *stream (unsigned char streamtype, unsigned char address);

// Tracing

bool tracing (symbol_t name);
void trace (symbol_t name);
void untrace (symbol_t name);

// Helper functions

bool consp (object *x);
bool atom (object *x);
bool listp (object *x);
bool improperp (object *x);
int toradix40 (char ch);
int fromradix40 (int n);
int pack40 (char *buffer);
bool valid40 (char *buffer);
int digitvalue (char d);
char *symbolname (symbol_t x);
int checkinteger (symbol_t name, object *obj);
float checkintfloat (symbol_t name, object *obj);
int checkchar (symbol_t name, object *obj);
int isstream (object *obj);
int issymbol (object *obj, symbol_t n);
void checkargs (symbol_t name, object *args);
int eq (object *arg1, object *arg2);
int listlength (symbol_t name, object *list);

// Association lists

object *assoc (object *key, object *list);
object *delassoc (object *key, object **alist);

// String utilities

void indent (int spaces, pfun_t pfun);
void buildstring (char ch, int *chars, object **head);
object *readstring (char delim, gfun_t gfun);
int stringlength (object *form);
char nthchar (object *string, int n);

// Lookup variable in environment

object *value (symbol_t n, object *env);
object *findvalue (object *var, object *env);

// In-place operations

object **place (symbol_t name, object *args, object *env);

// Checked car_ and cdr_

inline object *carx (object *arg) {
  if (!listp(arg)) error(0, PSTR("Can't take car"), arg);
  if (arg == nil_) return nil_;
  return car_(arg);
}

inline object *cdrx (object *arg) {
  if (!listp(arg)) error(0, PSTR("Can't take cdr"), arg);
  if (arg == nil_) return nil_;
  return cdr_(arg);
}

void checkanalogread (int pin);
void checkanalogwrite (int pin);

// Core functions

object *fn_not (object *args, object *env);
object *fn_cons (object *args, object *env);
object *fn_atom (object *args, object *env);
object *fn_listp (object *args, object *env);
object *fn_consp (object *args, object *env);
object *fn_symbolp (object *args, object *env);
object *fn_streamp (object *args, object *env);
object *fn_eq (object *args, object *env);

// System functions

object *fn_eval (object *args, object *env);
object *fn_globals (object *args, object *env);
object *fn_locals (object *args, object *env);
object *fn_makunbound (object *args, object *env);
object *fn_break (object *args, object *env);
object *fn_read (object *args, object *env);
object *fn_prin1 (object *args, object *env);
object *fn_print (object *args, object *env);
object *fn_princ (object *args, object *env);
object *fn_terpri (object *args, object *env);
object *fn_readbyte (object *args, object *env);
object *fn_readline (object *args, object *env);
object *fn_writebyte (object *args, object *env);
object *fn_writestring (object *args, object *env);
object *fn_writeline (object *args, object *env);

object *fn_saveimage (object *args, object *env);
object *fn_loadimage (object *args, object *env);
object *fn_cls (object *args, object *env);

// Setup

void initenv ();

#endif
