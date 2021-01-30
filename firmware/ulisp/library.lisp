(pinmode 7 1)
(defun toggle () (digitalwrite 7 (not (digitalread 7))))
(defun cloud (data) (publish "lisp" data))
(defun alarm () #|(print (now))|# (wake))
(defun wake ()
  (let ((n (now)))
    (when (eq (caddr n) 0)
      (print n))))
#include "core/ulisp-lisp-library.h"
