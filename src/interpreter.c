#include <stdio.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include "brackets.h"

static void generic_error_handle();
static size_t open_and_read_file(const char* filename, char** buf_ptr);
static void read_file(int fd, char* buf, size_t file_size);
static void execute(const char* code, size_t size);

int main(int argc, char* argval[]) {
    if(argc != 2) {
        fprintf(stderr, "Usage: %s [FILE]\nBrainf*ck interpreter.\n", argval[0]);
        exit(-1);
    }
    char* code;
    size_t file_size = open_and_read_file(argval[1], &code);
    execute(code, file_size);
    free(code);
    return 0;
}

void generic_error_handle() {
    fprintf(stderr, "Unknown error.");
    exit(-1);
}

size_t open_and_read_file(const char* filename, char** buf_ptr) {
    int fd = open(filename, O_RDONLY);
    if(fd == -1) {
        switch(errno) {
        case EACCES:
            fprintf(stderr, "Permission to file \'%s\' denied.\n", filename);
            break;
        case ENOENT:
            fprintf(stderr, "File \'%s\' not found.\n", filename);
            break;
        default:
            generic_error_handle();
        }
        exit(-1);
    }
    struct stat file_stat;
    if(fstat(fd, &file_stat) != 0) {
        close(fd);
        generic_error_handle();
    }
    if(!S_ISREG(file_stat.st_mode)) {
        close(fd);
        fprintf(stderr, "\'%s\' is not a regular file.\n", filename);
        exit(-1);
    }
    *buf_ptr = malloc(file_stat.st_size);
    if(*buf_ptr == NULL) {
        close(fd);
        if(errno == ENOMEM)
            fprintf(stderr, "Error. Memory exceeded.");
        else
            generic_error_handle();
        exit(-1);
    }
    read_file(fd, *buf_ptr, file_stat.st_size);
    close(fd);
    return file_stat.st_size;
}

static void read_file(int fd, char* buf, size_t file_size) {
    size_t bytes_read = 0;
    while(bytes_read < file_size) {
        int read_return = read(fd, buf + bytes_read, file_size - bytes_read);
        if(read_return == -1) {
            if(errno != EINTR) {
                free(buf);
                close(fd);
                generic_error_handle();
            }
            continue;
        }
        if(read_return == 0)
            break;
        bytes_read += (size_t)read_return;
    }
}

static char tape[30000];
static void execute(const char* code, size_t size) {
    unsigned br_nr = num_of_brackets(code, size);
    bracket_info* br_info_buf = malloc(br_nr * sizeof(bracket_info));
    fill_bracket_info(br_info_buf, code, size, br_nr);
    int p = 0;
    unsigned bracket_id = 0;
    struct stack last_not_jump;
    size_t pos = 0;
    while(pos < size) {
        switch(code[pos]) {
        case '>':
            p++;
            break;
        case '<':
            p--;
            break;
        case '+':
            tape[p]++;
            break;
        case '-':
            tape[p]--;
            break;
        case '.':
            putchar(tape[p]);
            break;
        case ',':
            tape[p] = getchar();
            break;
        case '[':
            if(tape[p] == 0) {
                pos = br_info_buf[bracket_id].close_pos;
                bracket_id += (br_info_buf[bracket_id].between_count + 1);
            }
            else {
                push(&last_not_jump, bracket_id);
                bracket_id++;
            }
            break;
        case ']':
            bracket_id = top(&last_not_jump);
            pop(&last_not_jump);
            pos = br_info_buf[bracket_id].open_pos - 1;
            break;
        }
        pos++;
    }
    free(br_info_buf);
}
