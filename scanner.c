#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#include "scanner.h"
#include "token.h"

#define CUR_CHAR source[index]
#define INITIAL_STRING_SIZE 64
#define STRING_GROWTH_RATE 2
#define MAX_ID_LENGTH 32

void skip_whitespace();
void next_char();
void scan_number(Token* target);
void scan_string(Token* target);
void scan_identifier(Token* target);
TokenType check_reserved(char* lexeme);

/*
    NOTE: src should be appended with its own '\0' 
    to make sure we do not read too far...
*/

char* source = NULL;
uint32_t index = 0;

void init_scanner(char* src)
{
    source = src;
    index = 0;
}

void next_token(Token* target)
{
    skip_whitespace();

    if (CUR_CHAR == '\0') {
        target->type = TOK_EOF;
        return;
    }

    if (isdigit(CUR_CHAR)) {

        scan_number(target);

    } else if (CUR_CHAR == '"') {

        /* Since the current character is a '"'. */
        next_char();

        scan_string(target);

    } else if (isalpha(CUR_CHAR) || CUR_CHAR == '_') {

        scan_identifier(target);

    } else {

        switch (CUR_CHAR) {
            case '+': {
                target->type = TOK_ADD;
                break;
            }
            case '-': {
                target->type = TOK_SUB;
                break;
            }
            case '*': {
                target->type = TOK_MUL;
                break;
            }
            case '/': {
                target->type = TOK_DIV;
                break;
            }
            case '%': {
                target->type = TOK_MOD;
                break;
            }
            case '^': {
                target->type = TOK_EXP;
                break;
            }
            case '(': {
                target->type = TOK_LPAR;
                break;
            }
            case ')': {
                target->type = TOK_RPAR;
                break;
            }
            case ',': {
                target->type = TOK_COMMA;
                break;
            }
            default: {
                target->type = TOK_EOF;
                break;
            }
        } /* switch */

        next_char();
    } /* else */
}   /* next_token() */

void scan_number(Token* target)
{
    /*
        If we come across whitespace before a period, then it's a long.
        Otherwise, if we encounter a period before a whitespace, it's a double.
        All we currently know is that the first character of the literal is a digit.
    */

    /*
        i64 max is 19 digits, as for f64...
        https://stackoverflow.com/questions/1701055/what-is-the-maximum-length-in-chars-needed-to-represent-any-double-value
    */
    char buffer[32] = {0};
    
    char* idx = buffer;

    while (CUR_CHAR != '\0' && isdigit(CUR_CHAR)) {
        *idx = CUR_CHAR;
        idx++;
        next_char();
    }  

    /* If float-then-else */
    if (CUR_CHAR == '.') {
        *idx = CUR_CHAR;
        idx++;
        next_char();
        
        while (CUR_CHAR != '\0' && isdigit(CUR_CHAR)) {
            *idx = CUR_CHAR;
            idx++;
            next_char();
        }
        *idx = '\0';

        target->type = TOK_DOUBLE;
        target->as.f64 = atof(buffer);
    } else {
        *idx = '\0';

        target->type = TOK_LONG;
        target->as.i64 = atol(buffer);
    }
}

void scan_string(Token* target)
{
    /* 
        NOTE: next_token() calls next_char() prior to this function, 
        so that the first character we have to handle here is not the '"'.
    */
    char* str_val;
    char* cur_pos;

    if ((str_val = malloc(INITIAL_STRING_SIZE + 1)) == NULL) {
        fprintf(stderr, "Failed to alloc space for string.\n");
        exit(1);
    }

    /* TODO: strings currently don't resize if longer than 64 characters */
    cur_pos = str_val;

    while (CUR_CHAR != '\0' && CUR_CHAR != '"') {
        *cur_pos = CUR_CHAR;
        cur_pos++;
        next_char();
    }
    *cur_pos = '\0';
    next_char();

    target->type = TOK_STRING;
    target->as.string = str_val;
}

void scan_identifier(Token* target)
{
    char lexeme[MAX_ID_LENGTH + 1];
    uint8_t i;

    /* TODO: should probably throw an error if ID_LENGTH is exceeded */
    i = 0;
    while (CUR_CHAR != '\0' && i < MAX_ID_LENGTH) {
        if (isalnum(CUR_CHAR) || CUR_CHAR == '_') {
            lexeme[i] = CUR_CHAR;
        } else {
            break;
        }
        i += 1;

        /* You numbskull, don't forget to call next character at the end of the loop */
        next_char();
    }
    lexeme[i] = '\0';

    target->type = check_reserved(lexeme);
    if (target->type != TOK_IDENTIFIER) {
        return;
    }

    target->as.string = malloc(strlen(lexeme) + 1);
    if (target->as.string == NULL) {
        fprintf(stderr, "Identifier allocation failed...");
        exit(11);
    }
    strncpy(target->as.string, lexeme, strlen(lexeme));
}

void cleanup_scanner()
{
    /* TODO: not sure how to manage source memory yet... */
    /* free(source); */ 

    source = NULL;
    index = 0;
}

void skip_whitespace()
{
    while (CUR_CHAR != '\0' && isspace(CUR_CHAR)) {
        next_char();
    }
}

void next_char()
{
    if (CUR_CHAR != '\0') {
        index += 1;
    }
}

typedef struct {
    TokenType type;
    char* id;
} ReservedPair;

ReservedPair reserved[] = {
    {TOK_SIN, "sin"},
    {TOK_COS, "cos"},
    {TOK_TAN, "tan"},
};


/* Yes, this is linear search & no I'm not ashamed... */
TokenType check_reserved(char* lexeme) {
    uint32_t reserved_length;
    uint32_t idx;
    
    reserved_length = sizeof(reserved) / sizeof(reserved[0]);

    for (idx = 0; idx < reserved_length; idx++) {
        if (strcmp(reserved[idx].id, lexeme) == 0) {
            return reserved[idx].type;
        }
    }

    return TOK_IDENTIFIER;
}