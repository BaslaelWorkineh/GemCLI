#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stddef.h>

void print_banner();
void print_usage();
void url_encode(const char* input, char* output, size_t max_len);
void clean_json_text(const char* input, char* output, size_t max_len);

#endif // UTILS_H
