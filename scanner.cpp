#include "scanner.h"
#include <cstring>
#include <cctype>
#include <cstdio>
#include <cstdlib>
bool flagOperator=false;

int flagMinus=0;
int keys = 32;
bool invalid_number=false;
char *keyword[] = {
    "and", "begin", "bool", "by", "constant",
    "do", "else", "end", "false", "fi", "float", "for", "from",
    "function", "if", "integer", "not", "od", "or", "procedure",
    "program", "read", "return", "string", "then", "to", "true",
    "var", "while", "write"
};

LEXEME_TYPE key_type[] = {
    kw_and, kw_begin, kw_bool, kw_by, kw_constant,
    kw_do, kw_else, kw_end, kw_false, kw_fi, kw_float, kw_for, kw_from,
    kw_function, kw_if, kw_integer, kw_not, kw_od, kw_or, kw_procedure,
    kw_program, kw_read, kw_return, kw_string, kw_then, kw_to, kw_true,
    kw_var, kw_while, kw_write
};

SCANNER::SCANNER() : Fd(nullptr), ch(' ') {}

SCANNER::SCANNER(FileDescriptor *fd) : Fd(fd) {
    ch =' ';
    flagOperator=false;
    read_char();
}

void SCANNER::read_char() {
    ch = Fd->GetChar();

}

void SCANNER::skip_comments() {
    while (ch != EOF) {
        // short comment
        if (ch == '#') {
            read_char();
            if (ch == '#') {
                read_char();
                return;
            }
        // long comment
        } else {
            read_char();
            if(ch == '\n') return;
        }
    }
}

bool SCANNER::check_keyword(TOKEN *token) {
    int flag =0;
    for (int i = 0; i < 30; ++i) {
        if (strcmp(lexeme, keyword[i]) == 0) {
            token->type = key_type[i];
            return true;
        }
    }
    return false;
}
// return Identifiers
TOKEN *SCANNER::get_id() {
    
    int i = 0;
    // if character or digit 
    while (isalnum(ch) || ch == '_') {
        lexeme[i++] = ch;
        read_char();
    }
    lexeme[i] = '\0';

    TOKEN *token = new TOKEN;
   // chaeck if got value is keyword
    if (!check_keyword(token)) {
        token->type = lx_identifier;
        token->str_ptr = strdup(lexeme);
    }
    
    return token;
}

TOKEN *SCANNER::get_string() {
    int i = 0;
    read_char(); // Skip the opening quote
    while (ch != '"' && ch != EOF) {
        lexeme[i++] = ch;
        read_char();
    }
    lexeme[i] = '\0';
    read_char(); 

    TOKEN *token = new TOKEN;
    token->type = lx_string;
    token->str_ptr = strdup(lexeme);
    return token;
}
bool negative_flage=false;
bool test= false;
    bool sign = false;
TOKEN* SCANNER::get_number() {
    if(ch == '-') test =true;
    TOKEN* token = new TOKEN;
    int i = 0;
    bool is_float = false;
    bool invalid_float = false;
    bool digits_after_dot = false;

    int error_line = Fd->GetLineNum();

    // Check for a leading dot or minus sign
    if (ch == '.') {
        lexeme[i++] = ch;
        read_char();
        is_float = true;
    }

    if (ch == '-') {
        
        sign = true;
        lexeme[i++] = ch;
        read_char();
    }

            int error_column = Fd->GetCharNum(); 
    while (isdigit(ch) || ch == '.') {
             error_column = Fd->GetCharNum(); 

        if (ch == '.') {
            // Check if more than one dot
            if (is_float) {
                invalid_float = true;
                break;
            } else {
                is_float = true;
                lexeme[i++] = ch;
            }
        } else {
            if (is_float) {
                digits_after_dot = true;
            }
            lexeme[i++] = ch;
        }
        read_char();
    }

    // Null-terminate the lexeme
    lexeme[i] = '\0';

    // If invalid floating point number (more than one dot or dot without digits after)
    if (invalid_float || (is_float && !digits_after_dot)) {
        // Fd->SetLineNumber(error_line);
        Fd->ReportError("Bad floating point number", error_line, error_column);

        // Skip to the next whitespace or newline to continue scanning
        while (ch != EOF && !isspace(ch)) {
            read_char();
        }
        if (isspace(ch)) {
            read_char(); // Move past the space/newline
        }

        delete token;
        return Scan();
    }

    if (isalpha(ch)) {
        Fd->ReportError("Invalid identifier, cannot start as number");

        // Skip to the next whitespace or newline to continue scanning
        while (ch != EOF && !isspace(ch)) {
            read_char();
        }
        if (isspace(ch)) {
            read_char(); // Move past the space/newline
        }

        delete token;
        return Scan();
    }

    // Set token type and value
    if (is_float) {
        if(test == true && atof(lexeme) ==0 )
        token->type = lx_minus;
        else{
        token->type = lx_float;
        token->float_value = atof(lexeme);
        }

    } else {
        if(test == true && atoi(lexeme) ==0 )
        token->type = lx_minus;
        else {
        token->type = lx_integer;
        token->value = atoi(lexeme);
        }
    }

    return token;
}

TOKEN* SCANNER::Scan() {
    while (isspace(ch)) {
        read_char();
    }
    
 
    TOKEN *token = nullptr;
    if (ch == EOF) {
        token = new TOKEN;
        token->type = lx_eof;
        return token;
    }

    switch (ch) {
        case '+': token = new TOKEN; token->type = lx_plus; read_char(); flagOperator=true; break;
        
      
        case '-':{
   
        if(flagOperator){
            TOKEN *t =get_number();
            return t;
        }
        else
            token = new TOKEN; token->type = lx_minus; read_char(); 
         
         break;
}

        case '*': token = new TOKEN; token->type = lx_star; read_char();flagOperator=true;  break;
        case '#':
            read_char();
            if (ch == '#') {
                skip_comments();
                return Scan();
            } else {
                Fd->ReportError("Illegal input");
                // read next tken
                while (ch != EOF) {
                    read_char();
                if (ch == ' ' || ch== '\n') {
                    return Scan();
                    
                }
                }
                return Scan();


                return Scan();
            }
            break;
       case '/': token = new TOKEN; token->type = lx_slash; read_char();flagOperator=true;  break;
        case '(': token = new TOKEN; token->type = lx_Iparen; read_char(); break;
        case ')': token = new TOKEN; token->type = lx_rparen; read_char(); break;
        case '{': token = new TOKEN; token->type = lx_rbracket; read_char(); break;
        case '}': token = new TOKEN; token->type = lx_lbracket; read_char(); break;
        case '[': token = new TOKEN; token->type = lx_lbracket; read_char(); break;
        case ']': token = new TOKEN; token->type = lx_rbracket; read_char(); break;
        case ':':
            read_char();
            if (ch == '=') {
                token = new TOKEN;
                token->type = lx_colon_eq;
                read_char();
            } else {
                token = new TOKEN;
                token->type = lx_colon;
            }
            break;
        case '.': token = new TOKEN; token->type = lx_dot; read_char(); break;
        case ';': token = new TOKEN; token->type = lx_semicolon; read_char(); break;
        case ',': token = new TOKEN; token->type = lx_comma; read_char(); break;
        case '=': token = new TOKEN; token->type = lx_eq; read_char(); break;
        case '<':
            read_char();
            if (ch == '=') {
                token = new TOKEN;
                token->type = lx_le;
                read_char();
            }  else {
                token = new TOKEN;
                token->type = lx_lt;
            }
            break;
        case '>':
            read_char();
            if (ch == '=') {
                token = new TOKEN;
                token->type = lx_ge;
                read_char();
            } else {
                token = new TOKEN;
                token->type = lx_gt;
            }
            break;
         case '!':
            read_char();
            if (ch == '=') {
                token = new TOKEN;
                token->type = lx_neq;
                read_char();
            } else {
                token = new TOKEN;
                token->type = lx_neq;
            }
            break;
            
        case '"': return get_string();

        default:
            if (isalpha(ch) || ch == '_') {
                return get_id();
            } else if (isdigit(ch)) {
                return get_number();
            } else {
                Fd->ReportError("Illegal Input"); // Skip unrecognized character
                read_char();
                return Scan();
            }
    }
    return token;
}

void SCANNER::printToken(TOKEN *token) {
    switch (token->type) {
        case lx_identifier:
            printf("Token: IDENTIFIER, Name: %s\n", token->str_ptr);
            break;
        case lx_integer:
        if(negative_flage)
            printf("Token: INTEGER, Value: -%d\n", token->value);
          else  printf("Token: INTEGER, Value: %d\n", token->value);
            break;
        case lx_float:
            printf("Token: FLOAT, Value: %.2f\n", token->float_value);
            break;
        case lx_string:
            printf("Token: STRING, Value: %s\n", token->str_ptr);
            break;
        case lx_plus:
            printf("Token: PLUS\n");
            break;
        case lx_minus:
            printf("Token: MINUS\n");
            break;
        case lx_star:
            printf("Token: STAR\n");
            break;
        case lx_slash:
            printf("Token: SLASH\n");
            break;
        case lx_Iparen:
            printf("Token: LPAREN\n");
            break;
        case lx_rparen:
            printf("Token: RPAREN\n");
            break;
        case lx_lbracket:
            printf("Token: LBRACKET\n");
            break;
        case lx_rbracket:
            printf("Token: RBRACKET\n");
            break;
        case lx_colon:
            printf("Token: COLON\n");
            break;
        case lx_dot:
            printf("Token: DOT\n");
            break;
        case lx_semicolon:
            printf("Token: SEMICOLON\n");
            break;
        case lx_comma:
            printf("Token: COMMA\n");
            break;
        case lx_colon_eq:
            printf("Token: COLON_EQ\n");
            break;
        case lx_eq:
            printf("Token: EQ\n");
            break;
        case lx_neq:
            printf("Token: NEQ\n");
            break;
        case lx_lt:
            printf("Token: LT\n");
            break;
        case lx_le:
            printf("Token: LE\n");
            break;
        case lx_gt:
            printf("Token: GT\n");
            break;
        case lx_ge:
            printf("Token: GE\n");
            break;
        case lx_eof:
            printf("Token: EOF\n");
            break;
        case kw_if:
            printf("Token: if KEYWORD\n");
            break;
        case kw_program:
            printf("Token: program KEYWORD\n");
            break;
        case kw_return:
            printf(" Token: return KEYWORD\n");
            break;
        case kw_var:
            printf(" Token: var KEYWORD\n");
            break;
        case kw_and:
            printf(" Token: and KEYWORD\n");
            break;
        case kw_begin:
            printf("Token: begin KEYWORD\n");
            break;
        case kw_bool:
            printf("Token: bool KEYWORD\n");
            break;
        case kw_by:
            printf("Token: by KEYWORD\n");
            break;
        case kw_constant:
            printf("Token: constant KEYWORD\n");
            break;
        case kw_do:
            printf("Token: do KEYWORD\n");
            break;
        case kw_else:
            printf("Token: else KEYWORD\n");
            break;
        case kw_end:
            printf("Token: end KEYWORD\n");
            break;
        case kw_false:
            printf("Token: false KEYWORD\n");
            break;
        case kw_fi:
            printf("Token: fi KEYWORD\n");
            break;
        case kw_float:
            printf("Token: float KEYWORD\n");
            break;
        case kw_for:
            printf("Token: for KEYWORD\n");
            break;
        case kw_from:
            printf("Token: from KEYWORD\n");
            break;
        case kw_function:
            printf("Token: function KEYWORD\n");
            break;
        
        case kw_integer:
            printf("Token: integer KEYWORD\n");
            break;
        case kw_not:
            printf("Token: not KEYWORD\n");
            break;
        case kw_od:
            printf("Token: od KEYWORD\n");
            break;
        case kw_or:
            printf("Token: or KEYWORD\n");
            break;
        case kw_procedure:
            printf("Token: procedure KEYWORD\n");
            break;
        
        case kw_read:
            printf("Token: read KEYWORD\n");
            break;
        
        case kw_string:
            printf("Token: string KEYWORD\n");
            break;
        case kw_then:
            printf("Token: then KEYWORD\n");
            break;
        case kw_to:
            printf("Token: to KEYWORD\n");
            break;
        case kw_true:
            printf("Token: true KEYWORD\n");
            break;
        case kw_while:
            printf("Token: while KEYWORD\n");
            break;
        case kw_write:
            printf("Token: write KEYWORD\n");
            break;
        default:
            printf("Token: Illegal Input\n");
            break;
    }
}



int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    FileDescriptor fd(argv[1]);
    if (!fd.IsOpen()) {
        printf("Can't open %s\n", argv[1]);
        return 1;
    }

    SCANNER scanner(&fd);
    TOKEN *token;
    do {
        token = scanner.Scan();
        scanner.printToken(token);
    } while (token->type != lx_eof);

    return 0;
}