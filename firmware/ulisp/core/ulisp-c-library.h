#ifndef __ULISP_C_H
#define __ULISP_C_H

#define PARTICLE_USING_DEPRECATED_API 1

#include "../ulisp.h"

object *number (int n);
object *makefloat (float f);
object *character (char c);
object *cons (object *arg1, object *arg2);
object *symbol (symbol_t name);
object *stream (unsigned char streamtype, unsigned char address);

int checkinteger (symbol_t name, object *obj);
float checkintfloat (symbol_t name, object *obj);
int checkchar (symbol_t name, object *obj);
int issymbol (object *obj, symbol_t n);
void checkargs (symbol_t name, object *args);
int isstream (object *obj);

void errorsub (symbol_t fname, PGM_P string);
void error (symbol_t fname, PGM_P string, object *symbol);
void error2 (symbol_t fname, PGM_P string);

#include "../library.cpp"

#endif // __ULISP_C_H
