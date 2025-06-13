#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <ctype.h>

#include "constants.h"
#include "config_manager.h"
#include "utils.h"
#include "gemini_api.h"


int main(int argc, char* argv[]) {
    char model_name_str[128];
    int model_changed = 0;
    if (load_model(model_name_str, sizeof(model_name_str)) != 0) {
        strncpy(model_name_str, DEFAULT_MODEL, sizeof(model_name_str) - 1);
        model_name_str[sizeof(model_name_str) - 1] = '\0';
    }

    if (argc < 2) {
        print_banner();
        print_usage();
        return 1;
    }

    if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
        if (argc != 2) {
             fprintf(stderr, YELLOW "Warning: Additional arguments with --help are ignored.\n" RESET);
        }
        print_banner();
        print_usage();
        return 0;
    }

    if (strcmp(argv[1], "--change-key") == 0) {
        if (argc != 2) {
            print_banner();
            print_usage();
            fprintf(stderr, RED "Error: --change-key does not take additional arguments.\n" RESET);
            return 1;
        }
        print_banner();
        setup_api_key();
        return 0;
    }

    if (strcmp(argv[1], "--delete-key") == 0) {
        if (argc != 2) {
            print_banner();
            print_usage();
            fprintf(stderr, RED "Error: --delete-key does not take additional arguments.\n" RESET);
            return 1;
        }
        print_banner();
        delete_api_key_file();
        return 0;
    }

    if (strcmp(argv[1], "--change-model") == 0) {
        if (argc != 2) {
            print_banner();
            print_usage();
            fprintf(stderr, RED "Error: --change-model does not take additional arguments.\n" RESET);
            return 1;
        }
        print_banner();
        setup_model();
        return 0;
    }

    char current_api_key[256] = {0};
    char current_question[MAX_QUERY_LENGTH] = {0};
    int current_arg_idx = 1;

    if (current_arg_idx < argc && strcmp(argv[current_arg_idx], "--model") == 0) {
        current_arg_idx++;
        if (current_arg_idx < argc) {
            strncpy(model_name_str, argv[current_arg_idx], sizeof(model_name_str) - 1);
            model_name_str[sizeof(model_name_str) - 1] = '\0';
            model_changed = 1;
            current_arg_idx++;
        } else {
            print_banner();
            print_usage();
            fprintf(stderr, RED "Error: Model name missing after --model option.\n" RESET);
            return 1;
        }
    }

    if (current_arg_idx >= argc) {
        fprintf(stderr, RED "Error: No question provided!\n" RESET);
        print_banner();
        print_usage();
        return 1;
    }

    for (int i = current_arg_idx; i < argc; i++) {
        if (strlen(current_question) + strlen(argv[i]) + (strlen(current_question) > 0 ? 1 : 0) < MAX_QUERY_LENGTH) {
            if (strlen(current_question) > 0) {
                strcat(current_question, " ");
            }
            strcat(current_question, argv[i]);
        } else {
            fprintf(stderr, YELLOW "Warning: Question truncated as it exceeds maximum length.\n" RESET);
            break;
        }
    }
    
    if (strlen(current_question) == 0) {
        fprintf(stderr, RED "Error: No question provided after parsing options!\n" RESET);
        print_banner();
        print_usage();
        return 1;
    }

    if (model_changed) {
        save_model(model_name_str);
    }
    
    if (load_api_key(current_api_key, sizeof(current_api_key)) != 0) {
        printf(YELLOW "No API key found or error loading it. Let's set one up!\n" RESET);
        if (setup_api_key() != 0) {
            // setup_api_key prints its own errors
            fprintf(stderr, RED "Failed to setup API key. Exiting.\n" RESET);
            return 1;
        }
        if (load_api_key(current_api_key, sizeof(current_api_key)) != 0) {
            fprintf(stderr, RED "Failed to load API key after setup. Exiting.\n" RESET);
            return 1;
        }
        printf("\n");
    }
    

    printf(BRIGHT_CYAN "Q: " RESET BOLD "%s\n" RESET, current_question);
    
    if (query_gemini(current_question, current_api_key, model_name_str) != 0) {
        return 1;
    }
    
    return 0;
}
