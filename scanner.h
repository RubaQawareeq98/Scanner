#ifndef __SCANNER__H
#define __SCANNER__H

#include "fd.h"
#include <cstring>
#include <cctype>
#include <cstdio>
#include <cstdlib>
enum { Ident_max = 16 };
typedef char Ident[Ident_max+1];
// Token types enumeration
typedef enum {
    /* Literals */
    lx_identifier, lx_integer, lx_string, lx_float,
    /* Keywords */
    kw_program, kw_var, kw_constant, kw_integer, kw_bool, kw_string, kw_float,
    kw_true, kw_false, kw_if, kw_fi, kw_then, kw_else,
    kw_while, kw_do, kw_od, kw_and, kw_or,
    kw_read, kw_write, kw_for, kw_from, kw_to, kw_by,
    kw_function, kw_procedure, kw_return, kw_not, kw_begin, kw_end,
    /* Operators */
    lx_Iparen, lx_rparen, lx_lbracket, lx_rbracket,
    lx_colon, lx_dot, lx_semicolon, lx_comma, lx_colon_eq,
    lx_plus, lx_minus, lx_star, lx_slash,
    lx_eq, lx_neq, lx_lt, lx_le, lx_gt, lx_ge, lx_eof
} LEXEME_TYPE;

// Definition of TOKEN
struct TOKEN {
    LEXEME_TYPE type;
    int value;
    float float_value;
    char *str_ptr;
};

class SCANNER {
private:
    FileDescriptor *Fd;
    int ch;
    char lexeme[256];
	Ident id;

    void read_char();
    void skip_comments();
    bool check_keyword(TOKEN *token);
    TOKEN *get_id();
    TOKEN *get_string();
    TOKEN *get_number();

public:
    SCANNER();
    SCANNER(FileDescriptor *fd);
    TOKEN* Scan();
    void printToken(TOKEN *token);
};

// Keyword list
extern int keys;
extern char *keyword[];
extern LEXEME_TYPE key_type[];

#endif
