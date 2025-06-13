#include "gemini_api.h"
#include "constants.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <ctype.h>
void clean_json_text(const char* input, char* output, size_t max_len) {
    size_t i = 0, j = 0;
    if (!input || !output || max_len == 0) return;

    while (i < strlen(input) && j < max_len - 1) {
        if (input[i] == '\\' && i + 1 < strlen(input)) {
            switch (input[i + 1]) {
                case 'n': output[j++] = '\n'; i += 2; break;
                case 't': output[j++] = '\t'; i += 2; break;
                case 'r': output[j++] = '\r'; i += 2; break;
                case '\\': output[j++] = '\\'; i += 2; break;
                case '"': output[j++] = '"'; i += 2; break;
                default: output[j++] = input[i++]; break;
            }
        } else {
            output[j++] = input[i++];
        }
    }
    output[j] = '\0';
}

void parse_and_display_response(const char* json_response) {
    const char* text_marker = "\"text\":";
    const char* text_start = strstr(json_response, text_marker);

    if (!text_start) {
        const char* error_marker = "\"message\":";
        const char* error_start = strstr(json_response, error_marker);
        if (error_start) {
            error_start += strlen(error_marker);
            while (*error_start == ' ' || *error_start == '"') error_start++;
            const char* error_end = strchr(error_start, '"');
            if (error_end) {
                printf(RED "Error from API: " RESET);
                fwrite(error_start, 1, error_end - error_start, stdout);
                printf("\n");
                return;
            }
        }
        printf(RED "Error: Could not parse 'text' field from Gemini response.\n" RESET);
        return;
    }

    text_start += strlen(text_marker);
    while (*text_start == ' ' || *text_start == '"') text_start++;
    const char* text_end = text_start;
    int in_string_escape = 0;
    while (*text_end) {
        if (in_string_escape) {
            in_string_escape = 0;
        } else if (*text_end == '\\') {
            in_string_escape = 1;
        } else if (*text_end == '"') {
            break;
        }
        text_end++;
    }
    
    if (!*text_end && *text_start == '\0') {
         printf(RED "Error: Parsed text field is empty or invalid.\n" RESET);
         return;
    }


    size_t text_length = text_end - text_start;
    if (text_length == 0 && *text_start != '"') {
         printf(RED "Error: Parsed text field is empty or invalid.\n" RESET);
         return;
    }


    char* extracted_text = malloc(text_length + 1);
    if (!extracted_text) {
        printf(RED "Error: Memory allocation failed for extracted_text.\n" RESET);
        return;
    }
    strncpy(extracted_text, text_start, text_length);
    extracted_text[text_length] = '\0';

    char* final_text = malloc(text_length + 1);
    if (!final_text) {
        free(extracted_text);
        printf(RED "Error: Memory allocation failed for final_text.\n" RESET);
        return;
    }

    clean_json_text(extracted_text, final_text, text_length + 1);

    printf(BRIGHT_GREEN "A: " RESET "%s\n", final_text);

    free(extracted_text);
    free(final_text);
}


int query_gemini(const char* question, const char* api_key, const char* model_name) {
    char curl_command[MAX_QUERY_LENGTH * 4 + 256];
    char json_payload[MAX_QUERY_LENGTH * 2 + 200];
    char temp_question_escaped[MAX_QUERY_LENGTH * 2];
    size_t o = 0;
    for(size_t i=0; question[i] != '\0' && o < sizeof(temp_question_escaped) -1; ++i) {
        if (question[i] == '"' || question[i] == '\\') {
            if (o + 1 < sizeof(temp_question_escaped) -1) {
                temp_question_escaped[o++] = '\\';
                temp_question_escaped[o++] = question[i];
            } else break;
        } else {
            temp_question_escaped[o++] = question[i];
        }
    }
    temp_question_escaped[o] = '\0';

    snprintf(json_payload, sizeof(json_payload),
        "{\"contents\":[{\"parts\":[{\"text\":\"%s\"}]}]}",
        temp_question_escaped);

    snprintf(curl_command, sizeof(curl_command),
        "curl -s -X POST -H 'Content-Type: application/json' "
        "-d '%s' "
        "'https://generativelanguage.googleapis.com/v1beta/models/%s:generateContent?key=%s' "
        "-o %s",
        json_payload, model_name, api_key, TEMP_FILE);

    int result = system(curl_command);
    if (result != 0) {
        fprintf(stderr, RED "Error: Failed to execute curl command (code: %d).\n" RESET, result);
        return 1;
    }

    FILE* file = fopen(TEMP_FILE, "r");
    if (!file) {
        fprintf(stderr, RED "Error: Could not read response file '%s'.\n" RESET, TEMP_FILE);
        return 1;
    }

    char response[MAX_RESPONSE_LENGTH];
    size_t bytes_read = fread(response, 1, sizeof(response) - 1, file);
    response[bytes_read] = '\0';
    fclose(file);

    unlink(TEMP_FILE);

    if (bytes_read == 0 && strlen(response) == 0) {
        fprintf(stderr, RED "Error: Response file was empty.\n" RESET);
        return 1;
    }
    
    parse_and_display_response(response);

    return 0;
}
