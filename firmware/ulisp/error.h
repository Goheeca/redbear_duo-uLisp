#ifndef __ERROR_H
#define __ERROR_H

#include "hw/platform.h"
#include "core.h"

// Error handling

void errorsub (symbol_t fname, PGM_P string);
void error (symbol_t fname, PGM_P string, object *symbol);
void error2 (symbol_t fname, PGM_P string);

/*#define errorsub(fname, string) _errorsub(fname, PSTR(string))
#define error(fname, string, symbol) _error(fname, PSTR(string), symbol)
#define error2(fname, string) error2(fname, PSTR(string))*/

// Save space as these are used multiple times
extern const char notanumber[] PROGMEM;
extern const char notastring[] PROGMEM;
extern const char notalist[] PROGMEM;
extern const char notproper[] PROGMEM;
extern const char notproper2[] PROGMEM;
extern const char notproper3[] PROGMEM;
extern const char noargument[] PROGMEM;
extern const char nostream[] PROGMEM;
extern const char overflow[] PROGMEM;

#endif
