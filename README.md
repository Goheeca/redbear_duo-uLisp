[![μλ](http://www.ulisp.com/pictures/3J/logo.gif)](http://www.ulisp.com/)

# [uLisp 3.3](http://www.ulisp.com/) for RedBear Duo

uLisp (Copyright (c) 2016-2020 David Johnson-Davies) is a remarkable implementation of lisp, this is a port to RedBear Duo and possibly other boards from the [particle.io](https://www.particle.io/) family.
This version is based on `ulisp-arm.ino`, because `ulisp-stm32.ino` is behind.

## Customization

* Lisp -- Go to `firmware/ulisp/library.lisp` and amend the lisp code, note the include directive at the end is essential.

* FFI -- Go to `firmware/ulisp/library.cpp`, this file consists of two sections the header and the implementation:
  * Header -- add the following: an enumeration constant into `function_` before `ENDFUNCTIONS`, a forward declaration of your custom function, a forward declaration of the string holding the symbolic name of your function, a new lookup entry (the last columns are argument count restrictions),
  * Body -- implement your custom functions and their symbolic names.

## The REPL of μλ

* Entering: `po serial monitor`
* Leaving: `Ctrl+a d` (it's `screen`)

### TODO

- [ ] Check peripherals and make them available in μλ
- [ ] Adding [FatFs](http://elm-chan.org/fsw/ff/00index_e.html)
- [ ] Image loading & saving using FAT
- [x] Expose the Particle API to μλ

---
[![Built with po-util](https://rawgit.com/nrobinson2000/po-util/master/images/built-with-po-util.svg)](https://po-util.com)

# This repository is a [po-util](https://po-util.com) project

Po makes it easy to locally develop firmware for Particle devices, and supports the Particle Photon, P1, Electron, Core, Raspberry Pi, and Redbear Duo.

Your project’s C++ files go in the `firmware/` directory, and the binary will appear in the `bin/` directory.

To compile code, run `po DEVICE_TYPE build`, substituting `DEVICE_TYPE` with `photon`, `P1`, `electron`, `core`, `pi`, or `duo`.

To compile and flash code, run `po DEVICE_TYPE flash`. Code is compiled and then flashed to your device over USB.

To clean the project, run `po DEVICE_TYPE clean`.

To flash a project over USB without rebuilding, run `po DEVICE_TYPE dfu`.

To upload a compiled project over the air run `po DEVICE ota DEVICE_NAME`, where `DEVICE_NAME` is the name of your device in the Particle cloud. **Note: You must be logged into particle-cli to use this feature. You can log into particle-cli with:**

```
particle login
```

For more help, run `man po`, or visit <https://docs.po-util.com/>

*By the way, po has tab completion. Try pressing [TAB] at any time to have arguments completed.*

Feel free to edit this README.md to make it more suitable for your project. **(I do ask that you please include the badge at the top though.)**
