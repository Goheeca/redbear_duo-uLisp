#ifndef __CHAR_STRING_H
#define __CHAR_STRING_H

#include "core.h"

// Characters

object *fn_char (object *args, object *env);
object *fn_charcode (object *args, object *env);
object *fn_codechar (object *args, object *env);
object *fn_characterp (object *args, object *env);

// Strings

object *fn_stringp (object *args, object *env);
bool stringcompare (symbol_t name, object *args, bool lt, bool gt, bool eq);
object *fn_stringeq (object *args, object *env);
object *fn_stringless (object *args, object *env);
object *fn_stringgreater (object *args, object *env);
object *fn_sort (object *args, object *env);
object *fn_stringfn (object *args, object *env);
object *fn_concatenate (object *args, object *env);
object *fn_subseq (object *args, object *env);
int gstr ();
object *fn_readfromstring (object *args, object *env);
void pstr (char c);
object *fn_princtostring (object *args, object *env);
object *fn_prin1tostring (object *args, object *env);

#include "frozen/lookup.h"
#define FREEZE_CS(no, str) \
    FREEZE(char_string_, no, str)
#define REIFY_CS(no, fn, min_arg, max_arg) \
    REIFY(char_string_, no, fn, min_arg, max_arg)

#define FREEZE_CHAR_STRING \
    FREEZE_CS(char, "char"); \
    FREEZE_CS(charcode, "char-code"); \
    FREEZE_CS(codechar, "code-char"); \
    FREEZE_CS(characterp, "characterp"); \
    FREEZE_CS(stringp, "stringp"); \
    FREEZE_CS(stringeq, "string="); \
    FREEZE_CS(stringless, "string<"); \
    FREEZE_CS(stringgreater, "string>"); \
    FREEZE_CS(sort, "sort"); \
    FREEZE_CS(stringfn, "string"); \
    FREEZE_CS(concatenate, "concatenate"); \
    FREEZE_CS(subseq, "subseq"); \
    FREEZE_CS(readfromstring, "read-from-string"); \
    FREEZE_CS(princtostring, "princ-to-string"); \
    FREEZE_CS(prin1tostring, "prin1-to-string");

#define REIFY_CHAR_STING \
    REIFY_CS(char, fn_char, 2, 2), \
    REIFY_CS(charcode, fn_charcode, 1, 1), \
    REIFY_CS(codechar, fn_codechar, 1, 1), \
    REIFY_CS(stringp, fn_stringp, 1, 1), \
    REIFY_CS(stringeq, fn_stringeq, 2, 2), \
    REIFY_CS(stringless, fn_stringless, 2, 2), \
    REIFY_CS(stringgreater, fn_stringgreater, 2, 2), \
    REIFY_CS(sort, fn_sort, 2, 2), \
    REIFY_CS(stringfn, fn_stringfn, 1, 1), \
    REIFY_CS(concatenate, fn_concatenate, 1, CALL_ARGUMENTS_LIMIT), \
    REIFY_CS(subseq, fn_subseq, 2, 3), \
    REIFY_CS(readfromstring, fn_readfromstring, 1, 1), \
    REIFY_CS(princtostring, fn_princtostring, 1, 1), \
    REIFY_CS(prin1tostring, fn_prin1tostring, 1, 1),

#endif
