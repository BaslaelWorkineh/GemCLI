#ifndef GEMINI_API_H
#define GEMINI_API_H

#include <stdio.h>
#include <stddef.h>

int query_gemini(const char* question, const char* api_key, const char* model_name);
void parse_and_display_response(const char* json_response);
void clean_json_text(const char* input, char* output, size_t max_len);

#endif // GEMINI_API_H
