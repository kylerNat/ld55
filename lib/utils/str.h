#ifndef __STR__
#define __STR__

#include <string.h> //might remove later
#include "misc.h"
#include "logging.h"
#include "memory.h"

struct strand
{
    char* str;
    int len;
};

#define init_strand(string) ((strand) {string, sizeof(string)-1})

bool operator==(strand str, int a)
{
    if(a == 0) return str.str==0;
    log_error("cannot compare strand and int");
    return 0;
}

define_printer(strand s, ("%.*s", s.len, s.str));

#define is_space(a) ((a) == ' ' || (a) == '\t')

#define is_newline(a) ((a) == '\n' || (a) == '\r')
#define is_lowercase(a) (('a' <= (a) && (a) <= 'z'))
#define is_uppercase(a) (('A' <= (a) && (a) <= 'Z'))
#define is_lowercasehex(a) (('a' <= (a) && (a) <= 'f'))
#define is_uppercasehex(a) (('A' <= (a) && (a) <= 'F'))
#define is_wordstart(a) (is_lowercase(a) || is_uppercase(a) || (a) == '_')
#define is_wordchar(a) (is_wordstart(a) || ('0' <= (a) && (a) <= '9'))
#define is_digit(a) (('0' <= (a) && (a) <= '9'))
#define is_hexdigit(a) (is_digit(a) || ('a' <= (a) && (a) <= 'f') || ('A' <= (a) && (a) <= 'F'))
#define is_bit(a) (('0' <= (a) && (a) <= '9'))

int get_next_word(char* code)
{
    if(!is_wordstart(*code)) return 0;
    int size = 1;
    while(is_wordchar(code[size])) size++;
    return size;
}

strand scan_next_word(char*& code)
{
    char* word = code;
    int word_len = get_next_word(code);
    code += word_len;
    return {word, word_len};
}

int get_line(char* code)
{
    int size = 0;
    while(!is_newline(code[size])) size++;
    return size;
}

//returns the start of the current line
char* get_line_start(char* code)
{
    while(!is_newline(*code)) code--;
    return code+1;
}

bool equal(char* a, char* b)
{
    return a && b && strcmp(a, b) == 0;
}

bool equal(char* a, const char* b)
{
    return a && b && strcmp(a, b) == 0;
}

bool equal(char* a, char* b, int size)
{
    return a && b && strncmp(a, b, size) == 0;
}

bool equal(strand a, strand b)
{
    return a.len==b.len && strncmp(a.str, b.str, a.len) == 0;
}

int find(char* code, char c)
{
    int i = 0;
    while(code[i] != c) i++;
    return i;
}

int find_oneof(char* code, char* cs)
{
    int i = 0;
    while(code[i])
    {
        for(char* c = cs; *c != 0; c++)
            if(code[i] == *c)
                return i;
        i++;
    }
    return i;
}

int find(char* code, char* s)
{
    int i = 0;
    while(!equal(code+i, s)) i++;
    return i;
}

// struct concat_helper
// {
//     char* s;
//     int len;
//     int mem_id;
// };

// concat_helper init_concat_helper()
// {
//     concat_helper helper;
//     helper.mem_id = reserve_stack();
//     helper.s = (char*) memory_stack_memory[helper.mem_id];
//     *helper.s = 0;
//     helper.len = 0;
//     return helper;
// }

// concat_helper operator,(concat_helper helper, char* s)
// {
//     strcat_s(helper.s, available_free_memory(helper.mem_id), s);
//     helper.len += strlen(s);
//     return helper;
// }

// concat_helper operator,(concat_helper helper, const char* s)
// {
//     strcat_s(helper.s, available_free_memory(helper.mem_id), s);
//     helper.len += strlen(s);
//     return helper;
// }

// concat_helper operator,(concat_helper helper, char c)
// {
//     helper.s[helper.len++] = c;
//     helper.s[helper.len] = 0;
//     return helper;
// }

// concat_helper operator,(concat_helper helper, real x)
// {

//     helper.len += sprintf(helper.s+helper.len, "%g", x);
//     return helper;
// }

// concat_helper operator,(concat_helper helper, double x)
// {

//     helper.len += sprintf(helper.s+helper.len, "%g", x);
//     return helper;
// }

// concat_helper operator,(concat_helper helper, int x)
// {

//     helper.len += sprintf(helper.s+helper.len, "%d", x);
//     return helper;
// }

// char* finalize_concat(concat_helper helper)
// {
//     memory_stack_memory[helper.mem_id] += helper.len+1;
//     unreserve_stack(helper.mem_id);
//     return helper.s;
// }

// #define concat(...) (finalize_concat((init_concat_helper(), __VA_ARGS__)))

#endif
