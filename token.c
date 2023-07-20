#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>

#include "token.h"

#define INITIAL_STACK_CAPACITY 64
#define STACK_GROWTH_FACTOR 2

char* tok_to_string[] = {
    "EOF",

    "sin",
    "cos",
    "tan",

    "+",
    "-",
    "*",
    "/",
    "%",
    "^",

    "(",
    ")",
    ",",

    NULL,
    NULL,
    NULL,

    NULL,

    NULL,
};

void print_token(Token* tok)
{
    /* Can never be too safe... */
    assert(sizeof(tok_to_string) / sizeof(tok_to_string[0]) == TOK_COUNT + 1);

    /* Early exit for simple tokens */
    if (tok->type < TOK_STRING) {
        printf("%s\n", tok_to_string[tok->type]);
        return;
    }

    switch (tok->type) {
        case TOK_IDENTIFIER:
        case TOK_STRING: {
            printf("%s\n", tok->as.string);
            break;
        }
        case TOK_LONG: {
            printf("%ld\n", tok->as.i64);
            break;
        }
        case TOK_DOUBLE: {
            printf("%f\n", tok->as.f64);
            break;
        }
        default: {
            printf("Printing unimplemented.\n");
            break;
        }
    }
}

int get_precedence(Token* tok)
{
    switch (tok->type) {
        case TOK_ADD:
        case TOK_SUB: {
            return 2;
        }
        case TOK_MUL:
        case TOK_DIV: 
        case TOK_MOD: {
            return 3;
        }

        case TOK_EXP: {
            return 4;
        }

        default :{
            fprintf(stderr, "Precedence not set for token '%d'.\n", tok->type);
            exit(8);
        }
    }
}

int get_associativity(Token* tok)
{
    switch (tok->type) {
        case TOK_ADD:
        case TOK_SUB:
        case TOK_MUL:
        case TOK_DIV:
        case TOK_MOD: {
            return ASS_LEFT;
        }

        case TOK_EXP: {
            return ASS_RIGHT;
        }

        default: {
            fprintf(stderr, "ASS_UNKNOWN.\n");
            exit(1);
        }
    }
}

void scrub_token(Token* tok)
{
    /* In case other tokens require custom free logic */
    switch (tok->type) {
        case TOK_IDENTIFIER:
        case TOK_STRING: {
            free(tok->as.string);   
            break;
        }
        default: {
            break;
        }
    }
    
    /* Just to be safe */
    tok->as.string = NULL;
}

TokenStack* alloc_token_stack()
{
    TokenStack* output;
    Token* output_base;

    if ((output = malloc(sizeof(TokenStack))) == NULL) {
        fprintf(stderr, "Failed to allocate token stack.\n");
        exit(5);
    }

    if ((output_base = malloc(sizeof(Token) * INITIAL_STACK_CAPACITY)) == NULL) {
        fprintf(stderr, "Failed to allocate space for tokens.\n");
        free(output);
        exit(6);
    }

    output->size = 0;
    output->capacity = INITIAL_STACK_CAPACITY;
    output->base = output_base;

    return output;
}

void free_token_stack(TokenStack* target)
{
    uint64_t i;

    for (i = 0; i < target->size; i++) {
        scrub_token(&target->base[i]);
    }
    free(target->base);
    free(target);
}

void push_token_stack(TokenStack* target, Token* item)
{
    if (target->size >= target->capacity - 1) {
        /* TODO: resize */
        Token* new_stack;
        new_stack = realloc(target->base, target->capacity * STACK_GROWTH_FACTOR);
        target->base = new_stack;
        target->capacity *= STACK_GROWTH_FACTOR;
    }

    target->base[target->size] = *item;
    target->size += 1;
}

Token pop_token_stack(TokenStack* target)
{
    assert(target->size > 0);
    
    target->size -= 1;

    return target->base[target->size];
}

void print_token_stack(TokenStack* target)
{
    uint32_t i;

    for (i = 0; i < target->size; i++) {
        print_token(&target->base[i]);
    }
}

Token add_tokens(Token* t1, Token* t2)
{
    Token output;

    if (IS_NUMBER(t1) && IS_NUMBER(t2)) {

        if (t1->type == TOK_LONG && t2->type == TOK_LONG) {

            output.type = TOK_LONG;
            output.as.i64 = t1->as.i64 + t2->as.i64;

        } else {

            output.type = TOK_DOUBLE;
            output.as.f64 = 0.0;

            switch(t1->type) {
                case TOK_LONG: {
                    output.as.f64 += t1->as.i64;
                    break;
                }
                case TOK_DOUBLE: {
                    output.as.f64 += t1->as.f64;
                    break;
                }
                default: {
                    fprintf(stderr, "'Oh poop' - Vector\n");
                    exit(1);
                }
            }
            switch(t2->type) {
                case TOK_LONG: {
                    output.as.f64 += t2->as.i64;
                    break;
                }
                case TOK_DOUBLE: {
                    output.as.f64 += t2->as.f64;
                    break;
                }
                default: {
                    fprintf(stderr, "'Oh poop' - Vector\n");
                    exit(1);
                }
            }

        }

    } else {
        fprintf(stderr, "Addition unimplemented.\n");
        exit(9);
    }

    return output;
}

Token sub_tokens(Token* t1, Token* t2)
{
    Token output;

    if (IS_NUMBER(t1) && IS_NUMBER(t2)) {

        if (t1->type == TOK_LONG && t2->type == TOK_LONG) {

            output.type = TOK_LONG;
            output.as.i64 = t1->as.i64 - t2->as.i64;

        } else {

            output.type = TOK_DOUBLE;
            output.as.f64 = 0.0;

            switch(t1->type) {
                case TOK_LONG: {
                    output.as.f64 += t1->as.i64;
                    break;
                }
                case TOK_DOUBLE: {
                    output.as.f64 += t1->as.f64;
                    break;
                }
                default: {
                    fprintf(stderr, "'Oh poop' - Vector\n");
                    exit(1);
                }
            }
            switch(t2->type) {
                case TOK_LONG: {
                    output.as.f64 -= t2->as.i64;
                    break;
                }
                case TOK_DOUBLE: {
                    output.as.f64 -= t2->as.f64;
                    break;
                }
                default: {
                    fprintf(stderr, "'Oh poop' - Vector\n");
                    exit(1);
                }
            }

        }

    } else {
        fprintf(stderr, "Subtraction unimplemented.\n");
        exit(9);
    }

    return output;
}

Token mul_tokens(Token* t1, Token* t2)
{
    Token output;

    if (IS_NUMBER(t1) && IS_NUMBER(t2)) {

        if (t1->type == TOK_LONG && t2->type == TOK_LONG) {

            output.type = TOK_LONG;
            output.as.i64 = t1->as.i64 * t2->as.i64;

        } else {

            output.type = TOK_DOUBLE;
            output.as.f64 = 0.0;

            switch(t1->type) {
                case TOK_LONG: {
                    output.as.f64 += t1->as.i64;
                    break;
                }
                case TOK_DOUBLE: {
                    output.as.f64 += t1->as.f64;
                    break;
                }
                default: {
                    fprintf(stderr, "'Oh poop' - Vector\n");
                    exit(1);
                }
            }
            switch(t2->type) {
                case TOK_LONG: {
                    output.as.f64 *= t2->as.i64;
                    break;
                }
                case TOK_DOUBLE: {
                    output.as.f64 *= t2->as.f64;
                    break;
                }
                default: {
                    fprintf(stderr, "'Oh poop' - Vector\n");
                    exit(1);
                }
            }

        }

    } else {
        fprintf(stderr, "Multiplication unimplemented.\n");
        exit(9);
    }

    return output;
}

Token div_tokens(Token* t1, Token* t2)
{
    Token output;

    if (IS_NUMBER(t1) && IS_NUMBER(t2)) {

        if (t1->type == TOK_LONG && t2->type == TOK_LONG) {

            output.type = TOK_LONG;
            output.as.i64 = t1->as.i64 / t2->as.i64;

        } else {

            output.type = TOK_DOUBLE;
            output.as.f64 = 0.0;

            switch(t1->type) {
                case TOK_LONG: {
                    output.as.f64 += t1->as.i64;
                    break;
                }
                case TOK_DOUBLE: {
                    output.as.f64 += t1->as.f64;
                    break;
                }
                default: {
                    fprintf(stderr, "'Oh poop' - Vector\n");
                    exit(1);
                }
            }
            switch(t2->type) {
                case TOK_LONG: {
                    output.as.f64 /= t2->as.i64;
                    break;
                }
                case TOK_DOUBLE: {
                    output.as.f64 /= t2->as.f64;
                    break;
                }
                default: {
                    fprintf(stderr, "'Oh poop' - Vector\n");
                    exit(1);
                }
            }

        }

    } else {
        fprintf(stderr, "Division unimplemented.\n");
        exit(9);
    }

    return output;
}

Token mod_tokens(Token* t1, Token* t2)
{
    Token output;

    if (IS_NUMBER(t1) && IS_NUMBER(t2)) {

        if (t1->type == TOK_LONG && t2->type == TOK_LONG) {

            output.type = TOK_LONG;
            output.as.i64 = t1->as.i64 % t2->as.i64;

        } else {

            output.type = TOK_LONG;
            output.as.i64 = 0;

            switch(t1->type) {
                case TOK_LONG: {
                    output.as.i64 += t1->as.i64;
                    break;
                }
                case TOK_DOUBLE: {
                    output.as.i64 += t1->as.f64;
                    break;
                }
                default: {
                    fprintf(stderr, "'Oh poop' - Vector\n");
                    exit(1);
                }
            }
            switch(t2->type) {
                case TOK_LONG: {
                    output.as.i64 %= t2->as.i64;
                    break;
                }
                case TOK_DOUBLE: {
                    output.as.i64 %= (int64_t) t2->as.f64;
                    break;
                }
                default: {
                    fprintf(stderr, "'Oh poop' - Vector\n");
                    exit(1);
                }
            }

        }
    } else {
        fprintf(stderr, "Modulo unimplemented.\n");
        exit(9);
    }

    return output;
}

Token exp_tokens(Token* t1, Token* t2)
{
    Token output;

    if (IS_NUMBER(t1) && IS_NUMBER(t2)) {

        if (t1->type == TOK_LONG && t2->type == TOK_LONG) {

            output.type = TOK_LONG;
            output.as.i64 = pow(t1->as.i64, t2->as.i64);

        } else {
            output.type = TOK_DOUBLE;
            output.as.f64 = 0.0; 

            switch(t1->type) {
                case TOK_LONG: {
                    output.as.f64 += t1->as.i64;
                    break;
                }
                case TOK_DOUBLE: {
                    output.as.f64 += t1->as.f64;
                    break;
                }
                default: {
                    fprintf(stderr, "'Oh poop' - Vector\n");
                    exit(1);
                }
            }
            switch(t2->type) {
                case TOK_LONG: {
                    output.as.f64 = pow(output.as.f64, t2->as.i64);
                    break;
                }
                case TOK_DOUBLE: {
                    output.as.f64 = pow(output.as.f64, t2->as.f64);
                    break;
                }
                default: {
                    fprintf(stderr, "'Oh poop' - Vector\n");
                    exit(1);
                }
            }

        }
    } else {
        fprintf(stderr, "Exponent unimplemented.\n");
        exit(9);
    }

    return output;
}

Token sin_token(Token* t1)
{
    Token output;

    output.type = TOK_DOUBLE;

    switch (t1->type) {
        case TOK_LONG: {
            output.as.f64 = sin(t1->as.i64);
            break;
        }
        case TOK_DOUBLE: {
            output.as.f64 = sin(t1->as.f64);
            break;
        }

        default: {
            fprintf(stderr, "'sin' unimplemented.\n");
            exit(12);
        }
    }

    return output;
}

Token cos_token(Token* t1)
{
    Token output;

    output.type = TOK_DOUBLE;

    switch (t1->type) {
        case TOK_LONG: {
            output.as.f64 = cos(t1->as.i64);
            break;
        }
        case TOK_DOUBLE: {
            output.as.f64 = cos(t1->as.f64);
            break;
        }

        default: {
            fprintf(stderr, "'cos' unimplemented.\n");
            exit(13);
        }
    }

    return output;
}

Token tan_token(Token* t1)
{
    Token output;

    output.type = TOK_DOUBLE;

    switch (t1->type) {
        case TOK_LONG: {
            output.as.f64 = tan(t1->as.i64);
            break;
        }
        case TOK_DOUBLE: {
            output.as.f64 = tan(t1->as.f64);
            break;
        }

        default: {
            fprintf(stderr, "'tan' unimplemented.\n");
            exit(14);
        }
    }

    return output;
}