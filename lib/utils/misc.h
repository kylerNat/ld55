#ifndef MISC
#define MISC

#include <stdint.h>
#include <limits.h>

#define NaN (0.0/0)

#define sq(a) ((a)*(a))

// #define STR_(a) #a
// #define STR(a) STR_(a)
#define STR_(...) #__VA_ARGS__
#define STR(...) STR_(__VA_ARGS__)

#define CODE(source) #source

#define CONCAT_(a, b) a##b
#define CONCAT(a, b) CONCAT_(a, b)

#define NOPAREN(...) __VA_ARGS__

#define NARGS(...) NARGS_(__VA_ARGS__, BACK_SEQ)
#define NARGS_(...) NARGS__(__VA_ARGS__)
#define NARGS__(                                \
    _1, _2, _3, _4, _5, _6, _7, _8, _9,_10,     \
    _11,_12,_13,_14,_15,_16,_17,_18,_19,_20,    \
    _21,_22,_23,_24,_25,_26,_27,_28,_29,_30,    \
    _31,_32,_33,_34,_35,_36,_37,_38,_39,_40,    \
    _41,_42,_43,_44,_45,_46,_47,_48,_49,_50,    \
    _51,_52,_53,_54,_55,_56,_57,_58,_59,_60,    \
    _61,_62,_63,N,...) N
#define BACK_SEQ          63,62,61,60,          \
        59,58,57,56,55,54,53,52,51,50,          \
        49,48,47,46,45,44,43,42,41,40,          \
        39,38,37,36,35,34,33,32,31,30,          \
        29,28,27,26,25,24,23,22,21,20,          \
        19,18,17,16,15,14,13,12,11,10,          \
        9,8,7,6,5,4,3,2,1,0

typedef unsigned int uint;

typedef uint8_t byte;
typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef uint8 bool8;

typedef uint32 bool32;

typedef float float32;
typedef double float64;

typedef float real;

#undef min
#undef max
//not just macros to avoid side effects of evaluating arguments
#define def_minmax(type)                                \
    type min(type a, type b) {return (a < b) ? a : b;} \
    type max(type a, type b) {return (a > b) ? a : b;} \
    type clamp(type t, type a, type b) {return max((a), min((b), (t)));}\
    type clamp_##type(type t, type a, type b) {return max((a), min((b), (t)));}

def_minmax(int)
def_minmax(uint)
def_minmax(uint64)
def_minmax(real)

#define local static

#define ever (;;)

#define len(array) sizeof(array)/sizeof((array)[0])

#define swap(a, b) {auto temp = a; a = b; b = temp;}

#ifdef __clang__
#define crash __builtin_trap()
#else
#ifdef _WIN32
#include <intrin.h>
#define crash __debugbreak()
#endif
#endif

#define kilobyte 1024
#define megabyte 1024*1024
#define gigabyte 1024*1024*1024

void quicksort(uint* values, int start, int end)
{
    if(start >= end-1) return;

    int middle = (start+end-1)/2;
    int pivot = values[middle];

    int unsorted_end = end;
    int lower_end = 0;
    int upper_start = end;
    for(int i = start; i < unsorted_end;)
    {
        if(values[i] > pivot)
        {
            swap(values[i], values[--unsorted_end]);
        }
        else
        {
            i++;
            if(values[i] < pivot) lower_end++;
        }
    }

    quicksort(values, start, lower_end);
    quicksort(values, unsorted_end, end);
}


void sort(uint* values, int n_values)
{
    quicksort(values, 0, n_values);
}

#endif
