#ifndef CALC_SCANNER_H
#define CALC_SCANNER_H

#include "token.h"

void init_scanner(char* src);
void next_token(Token* target);
void cleanup_scanner();

#endif