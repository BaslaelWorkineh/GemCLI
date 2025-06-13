#include "utils.h"
#include "constants.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

void print_banner() {
    printf(BRIGHT_BLUE "+--------------------------------------------------------------+" RESET "\n");
    printf(BRIGHT_BLUE "|" BRIGHT_CYAN "                      GemCLI                                  " BRIGHT_BLUE "|" RESET "\n");
    printf(BRIGHT_BLUE "|" RESET "             AI Assistant in Your Terminal                    " BRIGHT_BLUE "|" RESET "\n");
    printf(BRIGHT_BLUE "+--------------------------------------------------------------+" RESET "\n");
}

void print_usage() {
    printf("\n");
    printf(BOLD "Usage:" RESET "\n");
    printf("  gemini [options] <your question>\n");
    printf("  gemini --help | -h\n");
    printf("  gemini --change-key\n");
    printf("  gemini --change-model\n");
    printf("  gemini --delete-key\n\n");

    printf(BOLD "Options:" RESET "\n");
    printf("  --model <model_name>   Specify the Gemini model to use for this question only.\n");
    printf("  --help, -h             Show this help message and exit.\n");
    printf("  --change-key           Prompt to enter and save a new API key.\n");
    printf("  --change-model         Interactively change the default model (persistent).\n");
    printf("  --delete-key           Delete the currently stored API key.\n\n");

    printf(BOLD "Examples:" RESET "\n");
    printf("  gemini what is the weather like today?\n");
    printf("  gemini --model gemini-1.5-flash-latest explain quantum computing\n");
    printf("  gemini --change-key\n");
    printf("  gemini --change-model\n");
    printf("  gemini --delete-key\n");
    printf("  gemini --help\n\n");

    printf(BOLD "Setup:" RESET "\n");
    printf("  On first run, you'll be prompted to enter your Gemini API key.\n");
    printf("  Get your API key from: " CYAN "https://aistudio.google.com/apikey" RESET "\n");
    printf("  The key will be securely stored in %s/%s\n", CONFIG_DIR, CONFIG_FILE);
    printf("  Your preferred model will be saved in %s/%s\n\n", CONFIG_DIR, MODEL_FILE);
}

void url_encode(const char* input, char* output, size_t max_len) {
    const char* hex = "0123456789ABCDEF";
    size_t pos = 0;
    
    for (size_t i = 0; i < strlen(input) && pos < max_len - 4; i++) {
        unsigned char c = input[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            output[pos++] = c;
        } else if (c == ' ') {
            output[pos++] = '+';
        } else {
            if (pos + 3 >= max_len) break;
            output[pos++] = '%';
            output[pos++] = hex[c >> 4];
            output[pos++] = hex[c & 15];
        }
    }
    output[pos] = '\0';
}


