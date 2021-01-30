(pinmode 7 1)
(defun toggle () (digitalwrite 7 (not (digitalread 7))))
(defun cloud (data) (publish "lisp" data))
(defun alarm () #|(print (now))|#)
#include "core/ulisp-lisp-library.h"
