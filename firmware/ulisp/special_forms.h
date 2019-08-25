#ifndef __SPECIAL_FORMS_H
#define __SPECIAL_FORMS_H

#include "core.h"

// Special forms

object *sp_quote (object *args, object *env);
object *sp_defun (object *args, object *env);
object *sp_defvar (object *args, object *env);
object *sp_setq (object *args, object *env);
object *sp_loop (object *args, object *env);
object *sp_push_ (object *args, object *env);
object *sp_pop_ (object *args, object *env);

// Special forms incf/decf

object *sp_incf (object *args, object *env);
object *sp_decf (object *args, object *env);
object *sp_setf (object *args, object *env);
object *sp_dolist (object *args, object *env);
object *sp_dotimes (object *args, object *env);
object *sp_trace (object *args, object *env);
object *sp_untrace (object *args, object *env);
object *sp_formillis (object *args, object *env);
object *sp_withserial (object *args, object *env);
object *sp_withi2c (object *args, object *env);
object *sp_withspi (object *args, object *env);
object *sp_withsdcard (object *args, object *env);

#endif
