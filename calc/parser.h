#ifndef CALC_PARSER_H
#define CALC_PARSER_H

#include "token.h"

void init_parser();
void cleanup_parser();
void parse_expr();
TokenStack* get_output_stack();

#endif