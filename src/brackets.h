#ifndef BRACKETS_H
#define BRACKETS_H

#include <stdlib.h>
#include <stdio.h>

#include "stack.h"

typedef struct bracket_info {
    size_t open_pos;
    size_t close_pos;
    unsigned between_count;
} bracket_info;

static unsigned num_of_brackets(const char* code, size_t size) {
    unsigned counter = 0;
    int checker = 0;
    for(size_t i = 0; i < size; i++) {
        if(code[i] == '[') {
            checker++;
            counter++;
        }
        else if(code[i] == ']')
            checker--;
        if(checker < 0) {
            fprintf(stderr, "Syntax error. Unexpected \']\'\n");
            free((void*)code);
            exit(-1);
        }
    }
    if(checker != 0) {
        fprintf(stderr, "Syntax error. No closing \']\'\n");
        free((void*)code);
        exit(-1);
    }
    return counter;
}

static void calculate_between(bracket_info* info_buf, unsigned index, unsigned bracket_nr) {
    if(index >= bracket_nr)
        return;
    info_buf[index].between_count = 0;
    unsigned i = index + 1;
    while(i < bracket_nr && info_buf[i].close_pos < info_buf[index].close_pos) {
        calculate_between(info_buf, i, bracket_nr);
        info_buf[index].between_count += (info_buf[i].between_count + 1);
        i += (info_buf[i].between_count + 1);
    }
    calculate_between(info_buf, i, bracket_nr);
}

static void fill_bracket_info(bracket_info* info_buf, const char* code, size_t size, unsigned bracket_nr) {
    unsigned pos_in_info_buf = 0;
    struct stack s;
    for(size_t i = 0; i < size; i++) {
        if(code[i] == '[') {
            info_buf[pos_in_info_buf].open_pos = i;
            push(&s, pos_in_info_buf++);
        }
        if(code[i] == ']') {
            info_buf[top(&s)].close_pos = i;
            pop(&s);
        }
    }
    calculate_between(info_buf, 0, bracket_nr);
}

#endif