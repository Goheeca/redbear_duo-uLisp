(pinmode 7 1)
(defun toggle () (digitalwrite 7 (not (digitalread 7))))
(defun cloud (data) (publish "lisp" data))
(defun alarm (h m s) #|(print (now))|# (wake (list h m s)))
(defun wake (time)
    (when (eq (caddr time) 0)
      (print time)))
#include "core/ulisp-lisp-library.h"
