#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "token.h"
#include "scanner.h"
#include "parser.h"

int main(int argc, char* argv[]) {
    TokenStack* output_stack;
    TokenStack* value_stack;
    Token t1;
    Token t2;
    Token result;
    uint64_t ip;
    char* buffer;

    if (argc != 2) {
        fprintf(stderr, "USAGE: calc \"<expression>\"\n");
        exit(22);
    }

    /* char* buffer = "3.1415 * 5.3 ^ 2"; */
    buffer = argv[1];

    init_scanner(buffer);
    
    init_parser();
    parse_expr();

    output_stack = get_output_stack();
    value_stack = alloc_token_stack();

    /* Evaluate output stack here... */
    for (ip = 0; ip < output_stack->size; ip++) {
        switch (output_stack->base[ip].type) {
            /* TODO: Can do TOK_STRING here as well someday... */
            case TOK_DOUBLE:
            case TOK_LONG: {
                push_token_stack(value_stack, &output_stack->base[ip]);
                break;
            }

            case TOK_ADD: {
                t2 = pop_token_stack(value_stack);
                t1 = pop_token_stack(value_stack);

                result = add_tokens(&t1, &t2);
                
                push_token_stack(value_stack, &result);
                break;
            }
            case TOK_SUB: {
                t2 = pop_token_stack(value_stack);
                t1 = pop_token_stack(value_stack);

                result = sub_tokens(&t1, &t2);

                push_token_stack(value_stack, &result);
                break;
            }
            case TOK_MUL: {
                t2 = pop_token_stack(value_stack);
                t1 = pop_token_stack(value_stack);

                result = mul_tokens(&t1, &t2);

                push_token_stack(value_stack, &result);
                break;
            }
            case TOK_DIV: {
                t2 = pop_token_stack(value_stack);
                t1 = pop_token_stack(value_stack);

                result = div_tokens(&t1, &t2);

                push_token_stack(value_stack, &result);
                break;
            }
            case TOK_MOD: {
                t2 = pop_token_stack(value_stack);
                t1 = pop_token_stack(value_stack);

                result = mod_tokens(&t1, &t2);

                push_token_stack(value_stack, &result);
                break;
            }
            case TOK_EXP: {
                t2 = pop_token_stack(value_stack);
                t1 = pop_token_stack(value_stack);

                result = exp_tokens(&t1, &t2);

                push_token_stack(value_stack, &result);
                break;
            }
            default : {
                fprintf(stderr, "Unimplemented instruction.\n");
                exit(420);
            }
        }
    }

    print_token(&value_stack->base[0]);

    output_stack = NULL; /* Not really necessary, but prevents further misuse */
    free_token_stack(value_stack);

    /* 'cleanup_parser()' frees the output stack */
    cleanup_parser();
    cleanup_scanner();

    return 0;
}