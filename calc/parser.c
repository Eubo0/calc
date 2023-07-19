#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include "scanner.h"
#include "token.h"
#include "parser.h"

TokenStack* operator_stack = NULL;
TokenStack* output_stack = NULL;

Token t = DEFAULT_TOKEN;

void init_parser()
{
    operator_stack = alloc_token_stack();
    output_stack = alloc_token_stack();
}

void cleanup_parser()
{
    free_token_stack(operator_stack);

    /* 
        NOTE: Remember to scrub all the tokens on the output stack to be safe.
        Since we hand out references to the output stack, the responsibilty
        should fall on other parts of the program to not call 'free_parser()' too early.
    */
    free_token_stack(output_stack);
}

void parse_expr()
{
    /*
        TODO: functions get pushed straight onto the operator stack
        https://en.wikipedia.org/wiki/Shunting_yard_algorithm
    */
    Token temp;

    next_token(&t);

    while (t.type != TOK_EOF) {
        if (t.type == TOK_LONG || t.type == TOK_DOUBLE) {

            push_token_stack(output_stack, &t);

        } else if (IS_OPERATOR(t.type)) {
            /* TODO... */
            while ((STACK_TOP(operator_stack).type != TOK_LPAR && operator_stack->size > 0)
                && (get_precedence(&STACK_TOP(operator_stack)) > get_precedence(&t) 
                    || (get_precedence(&STACK_TOP(operator_stack)) == get_precedence(&t) 
                        && get_associativity(&t) == ASS_LEFT)
                    )
            ) {
                temp = pop_token_stack(operator_stack);
                push_token_stack(output_stack, &temp);
            }

            push_token_stack(operator_stack, &t);

        } else if (t.type == TOK_COMMA) {
            /* Unecessary until we implement functions... */
        } else if (t.type == TOK_LPAR) {
            
            push_token_stack(operator_stack, &t);
        
        } else if (t.type == TOK_RPAR) {
            
            while (operator_stack->base[operator_stack->size - 1].type != TOK_LPAR) {
                /* NOTE: asserting non-empty happens in 'pop_token_stack()' */
                temp = pop_token_stack(operator_stack);
                push_token_stack(output_stack, &temp);
            }
            assert(operator_stack->size > 0);
            assert(operator_stack->base[operator_stack->size - 1].type = TOK_LPAR);
            pop_token_stack(operator_stack);

            /*
                TODO: if there is a function on top of the operator stack, then
                push it into the output queue...
            */
        } else {

                printf("I'm confused :( '%d'\n", t.type);
                exit(69);
        
        }

        next_token(&t);
    }

    while (operator_stack->size != 0) {
        assert(operator_stack->base[operator_stack->size - 1].type != TOK_LPAR);

        temp = pop_token_stack(operator_stack);
        push_token_stack(output_stack, &temp);
    }
}

TokenStack* get_output_stack()
{
    return output_stack;
}