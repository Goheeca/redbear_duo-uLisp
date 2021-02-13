#ifdef __ULISP_C_H

// Insert your own function definitions here

enum function_ { PEEK = _ENDFUNCTIONS, POKE, PUBLISH, NOW, ZONE, RGB_CONTROL, RGB_COLOR, RGB_BRIGHTNESS, ENDFUNCTIONS};

object *fn_peek (object *args, object *env);
object *fn_poke (object *args, object *env);
object *fn_publish (object *args, object *env);
object *fn_now (object *args, object *env);
object *fn_zone (object *args, object *env);
object *fn_rgb_control (object *args, object *env);
object *fn_rgb_color (object *args, object *env);
object *fn_rgb_brightness (object *args, object *env);
void process_system();

extern const char string_fn_peek[] PROGMEM;
extern const char string_fn_poke[] PROGMEM;
extern const char string_fn_publish[] PROGMEM;
extern const char string_fn_now[] PROGMEM;
extern const char string_fn_zone[] PROGMEM;

extern const char string_fn_rgb_control[] PROGMEM;
extern const char string_fn_rgb_color[] PROGMEM;
extern const char string_fn_rgb_brightness[] PROGMEM;

#ifdef LOOKUP_TABLE_ENTRIES
#undef LOOKUP_TABLE_ENTRIES
#endif
#define LOOKUP_TABLE_ENTRIES \
    { string_fn_peek, fn_peek, 0x11 }, \
    { string_fn_poke, fn_poke, 0x22 }, \
    { string_fn_publish, fn_publish, 0x22 }, \
    { string_fn_now, fn_now, 0x00 }, \
    { string_fn_zone, fn_zone, 0x11 }, \
    { string_fn_rgb_control, fn_rgb_control, 0x01 }, \
    { string_fn_rgb_color, fn_rgb_color, 0x33 }, \
    { string_fn_rgb_brightness, fn_rgb_brightness, 0x01 }, \

#else // __ULISP_C_H

#include "core/ulisp-c-library.h"
#include "Particle.h"

object *fn_peek (object *args, object *env) {
  (void) env;
  checkargs(PEEK, args);
  int addr = checkinteger(PEEK, first(args));
  return number(*(int *)addr);
}

object *fn_poke (object *args, object *env) {
  (void) env;
  checkargs(POKE, args);
  int addr = checkinteger(POKE, first(args));
  object *val = second(args);
  *(int *)addr = checkinteger(POKE, val);
  return val;
}

object *fn_now (object *args, object *env) {
  (void) env;
  checkargs(NOW, args);
  object *now = cons(number(Time.hour(Time.local())), cons(number(Time.minute(Time.local())), cons(number((Time.second)(Time.local())), NULL)));
  return now;
}

String STR_PARTICLE;
void STR_PARTICLE_APPEND(char c) {
    STR_PARTICLE.concat(String(c));
}

object *fn_publish (object *args, object *env) {
  (void) env;
  checkargs(PUBLISH, args);
  if (stringp(first(args))) {
    STR_PARTICLE = String("");
    prin1object(first(args), STR_PARTICLE_APPEND);
    String channel = STR_PARTICLE;
    STR_PARTICLE = String("");
    printobject(second(args), STR_PARTICLE_APPEND);
    return Particle.publish(channel, STR_PARTICLE) ? tee : nil;
  }
  return nil;
}

int STR_POSITION;
const char *STR_READER;
int string_reader () {
  char c = STR_READER[STR_POSITION++];
  return (c != 0) ? c : -1;
}

int cloud (String data) {
    STR_POSITION = 0;
    data.concat("\n");
    data.replace(")", " )");
    STR_READER = data.c_str();

    object *current_GCStack = GCStack;
    jmp_buf dynamic_handler;
    jmp_buf *previous_handler = handler;
    handler = &dynamic_handler;
    object *result = nil;

    bool muffled = tstflag(MUFFLEERRORS);
    setflag(MUFFLEERRORS);
    volatile int signaled_error = -2;
    if (!setjmp(dynamic_handler)) {
       object *lisp_data = read(string_reader);
       signaled_error = -1;
       object *form = cons(newsymbol(pack40("cloud\0")), cons(lisp_data, NULL));
       result = eval(form, NULL);
       signaled_error = 0;
    } else {
      GCStack = current_GCStack;
    }
    handler = previous_handler;
    if (!muffled) clrflag(MUFFLEERRORS);

    switch (signaled_error) {
        case 0: return result != nil ? 1 : 0;
        default: return signaled_error;
    }
}

object *fn_zone (object *args, object *env) {
  (void) env;
  checkargs(ZONE, args);
  float zone = checkintfloat(ZONE, first(args));
  Time.zone(zone);
  return nil;
}

int lastHour = -1;
int lastMinute = -1;
int lastSecond = -1;
void process_system() {
    Particle.process();
    int local = Time.local();
    int h = Time.hour(local);
    int min = Time.minute(local);
    int s = (Time.second)(local);
    if (Time.isValid() && (lastHour != h || lastMinute != min || lastSecond != s)) {
        lastHour = h;
        lastMinute = min;
        lastSecond = s;
        object *form = cons(symbol(IGNOREERRORS), cons(cons(newsymbol(pack40("alarm\0")), cons(number(h), cons(number(min), cons(number(s), NULL)))), NULL));
        eval(form, NULL);
    }
}

object *fn_rgb_control (object *args, object *env) {
  (void) env;
  checkargs(RGB_CONTROL, args);
  if (nil != args) {
    bool ctrl = first(args) != nil;
    RGB.control(ctrl);
  }
  return RGB.controlled() ? tee : nil;
}

object *fn_rgb_color (object *args, object *env) {
  (void) env;
  checkargs(RGB_COLOR, args);
  int red = checkinteger(RGB_COLOR, first(args));
  int green = checkinteger(RGB_COLOR, second(args));
  int blue = checkinteger(RGB_COLOR, third(args));
  RGB.color(red, green, blue);
  return tee;
}

object *fn_rgb_brightness (object *args, object *env) {
  (void) env;
  checkargs(RGB_BRIGHTNESS, args);
  if (nil != args) {
    int bright = checkinteger(RGB_BRIGHTNESS, first(args));
    RGB.brightness(bright);
  }
  return number(RGB.brightness());
}


const char string_fn_peek[] PROGMEM = "peek";
const char string_fn_poke[] PROGMEM = "poke";
const char string_fn_publish[] PROGMEM = "publish";
const char string_fn_now[] PROGMEM = "now";
const char string_fn_zone[] PROGMEM = "zone";
const char string_fn_rgb_control[] PROGMEM = "rgb-control";
const char string_fn_rgb_color[] PROGMEM = "rgb";
const char string_fn_rgb_brightness[] PROGMEM = "rgb-brightness";

#endif
