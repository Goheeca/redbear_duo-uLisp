(defun sq (x) (* x x))
(defun cub (x) (* x x x))
(pinmode 7 1)
(defun toggle () (digitalwrite 7 (not (digitalread 7))))
(defun test (x) (print x))
#include "core/ulisp-lisp-library.h"
