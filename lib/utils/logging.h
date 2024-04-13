#ifndef LOGGING
#define LOGGING

#ifndef PRINTFN
#define PRINTFN printf
#endif
#ifndef FLUSHFN
#define FLUSHFN() fflush(stdout)
#endif

#include <stdlib.h>
#include <stdio.h>
#include <utils/misc.h>

struct print_format
{
    int argument;
};

typedef int (*printfn_t)(const char* format, ...);

struct printer
{
    int count;
    printfn_t print_function;
    print_format format;
};

#define default_print_format {1}
printer __printer__ = {0, PRINTFN, default_print_format};

printer operator,(printer p, print_format pf)
{
    p.format = pf;
    return p;
}

//for printing using fprintf or other format functions
//TODO: there's probably a better way to do this
printer operator,(printer p, printfn_t f)
{
    p.print_function = f;
    return p;
}

//TODO: make this automatically generated for each type
struct int_list
{
    int* list;
    int length;
};

printer operator,(printer p, int_list a)
{
    for(int __i = 0; __i < p.format.argument; __i++)
    {
        for(int j = 0; j < a.length; j++)
            p.count += p.print_function("%i, ", a.list[j]);
    }
    p.format = default_print_format;
    return p;
}

struct float_list
{
    float* list;
    int length;
};

printer operator,(printer p, float_list a)
{
    for(int __i = 0; __i < p.format.argument; __i++)
    {
        for(int j = 0; j < a.length; j++)
            p.count += p.print_function("%f, ", a.list[j]);
    }
    p.format = default_print_format;
    return p;
}


#define define_printer(arg, print_code)                         \
    printer operator,(printer p, arg)                           \
    {                                                           \
        for(int __i = 0; __i < p.format.argument; __i++)        \
        {                                                       \
            p.count += p.print_function print_code;             \
        }                                                       \
        p.format = default_print_format;                        \
        return p;                                               \
    }


define_printer(const char* s, ("%s", s));

define_printer(char* s, ("%s", s));

define_printer(char c, ("%.1s", &c));

define_printer(int a, ("%d", a));
define_printer(int64 a, ("%lld", a));
define_printer(uint a, ("%u", a));
define_printer(uint64 a, ("%llu", a));
#if __EMSCRIPTEN__
define_printer(size_t a, ("%lu", a));
#endif

define_printer(void* a, ("0x%08X", a));

struct hex_uint {uint value;};
define_printer(hex_uint a, ("0x%08X", (uint) a.value));

define_printer(float a, ("%f", a));
define_printer(double a, ("%f", a));

#ifdef MATHS
define_printer(int_2 a, ("(%i, %i)", a.x, a.y));
define_printer(int_3 a, ("(%i, %i, %i)", a.x, a.y, a.z));
define_printer(real_2 a, ("(%f, %f)", a.x, a.y));
define_printer(real_3 a, ("(%f, %f, %f)", a.x, a.y, a.z));
define_printer(real_3x3 a, ("\n(%f, %f, %f\n %f, %f, %f\n %f, %f, %f)", a[0][0], a[1][0], a[2][0], a[0][1], a[1][1], a[2][1], a[0][2], a[1][2], a[2][2]));
define_printer(quaternion a, ("(%f, %f, %f, %f)", a.r, a.i, a.j, a.k));
define_printer(real_4 a, ("(%f, %f, %f, %f)", a.x, a.y, a.z, a.w));
#endif //MATHS

/* define_printer(bool a, printf("%s", a ? "true" : "false")); */

#define log_warning(...) {(__printer__, "warning: ", __VA_ARGS__, "\n"); FLUSHFN();}

#define log_error(...) {(__printer__, "error: ", __VA_ARGS__, "\n"); FLUSHFN(); exit(EXIT_FAILURE);}

#define log_output(...) {(__printer__, __VA_ARGS__); FLUSHFN();}

#ifndef NO_ASSERTS
#define assert(this_is_true, ...) {if(!(this_is_true)) {log_error("assert(", #this_is_true, ") in ", __FILE__, " (", __LINE__, "): ", ##__VA_ARGS__);}}
#else
#define assert(...)
#endif

#endif //LOGGING
