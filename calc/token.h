#ifndef CALC_TOKEN_H
#define CALC_TOKEN_H

#include <stdint.h>

/* Look at me, I know how to use the preprocessor */
#define DEFAULT_TOKEN {TOK_EOF, {NULL}}
#define IS_OPERATOR(type) (type >= TOK_ADD && type <= TOK_EXP)
#define STACK_TOP(s) (s->base[s->size - 1])
#define IS_NUMBER(t) (t->type == TOK_LONG || t->type == TOK_DOUBLE)

typedef enum {
    ASS_LEFT,
    ASS_RIGHT,
    ASS_UNKNOWN
} Associativity;

typedef enum {
    TOK_EOF,

    /* Operators */
    TOK_ADD,
    TOK_SUB,
    TOK_MUL,
    TOK_DIV,
    TOK_MOD,
    TOK_EXP,

    /* "Punctuation" */
    TOK_LPAR,
    TOK_RPAR,
    TOK_COMMA,

    /* NB: Simple to print type must go above 'TOK_STRING' */
    /* Value types */
    TOK_STRING,
    TOK_LONG,
    TOK_DOUBLE,

    /* Might be useful... */
    TOK_COUNT
} TokenType;

typedef struct {
    TokenType type;

    union {
        char* string;
        int64_t i64;
        double f64;
    } as;
} Token;

typedef struct {
    uint64_t size;
    uint64_t capacity;

    Token* base;
} TokenStack;

void print_token(Token* tok);
int get_precedence(Token* tok);
int get_associativity(Token* tok);
void scrub_token(Token* tok);

TokenStack* alloc_token_stack();
void free_token_stack(TokenStack* target);
void push_token_stack(TokenStack* target, Token* item);
Token pop_token_stack(TokenStack* target);
void print_token_stack(TokenStack* target);

Token add_tokens(Token* t1, Token* t2);
Token sub_tokens(Token* t1, Token* t2);
Token mul_tokens(Token* t1, Token* t2);
Token div_tokens(Token* t1, Token* t2);
Token mod_tokens(Token* t1, Token* t2);
Token exp_tokens(Token* t1, Token* t2);

#endif