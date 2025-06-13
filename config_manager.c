#include "config_manager.h"
#include "constants.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <ctype.h>

char* get_config_path() {
    static char config_path[MAX_PATH_LENGTH];
    const char* home = getenv("HOME");
    if (!home) {
        return NULL;
    }
    snprintf(config_path, sizeof(config_path), "%s/%s", home, CONFIG_DIR);
    return config_path;
}

void create_config_dir() {
    char* config_dir = get_config_path();
    if (config_dir) {
        struct stat st = {0};
        if (stat(config_dir, &st) == -1) {
            mkdir(config_dir, 0700);
        }
    }
}

int save_api_key(const char* api_key) {
    create_config_dir();

    char* config_dir_path = get_config_path();
    if (!config_dir_path) {
        fprintf(stderr, RED "Error: Could not get config directory path.\n" RESET);
        return 1;
    }

    char config_file_path[MAX_PATH_LENGTH];
    if (strlen(config_dir_path) + strlen(CONFIG_FILE) + 2 > sizeof(config_file_path)) {
        fprintf(stderr, RED "Error: Path too long for config_file_path buffer in save_api_key.\n" RESET);
        return 1;
    }
    snprintf(config_file_path, sizeof(config_file_path), "%s/%s", config_dir_path, CONFIG_FILE);

    FILE* file = fopen(config_file_path, "w");
    if (!file) {
        perror(RED "Error opening config file for writing" RESET);
        return 1;
    }
    
    fprintf(file, "%s\n", api_key);
    fclose(file);
    
    chmod(config_file_path, 0600);
    
    return 0;
}

int load_api_key(char* api_key, size_t max_len) {
    char* config_dir_path = get_config_path();
    if (!config_dir_path) {
        // Optionally print an error, or let main handle it by calling setup
        return 1; 
    }

    char config_file_path[MAX_PATH_LENGTH];
    if (strlen(config_dir_path) + strlen(CONFIG_FILE) + 2 > sizeof(config_file_path)) {
        api_key[0] = '\0';
        return 1;
    }
    snprintf(config_file_path, sizeof(config_file_path), "%s/%s", config_dir_path, CONFIG_FILE);

    FILE* file = fopen(config_file_path, "r");
    if (!file) {
        api_key[0] = '\0';
        return 1;
    }
    
    if (fgets(api_key, max_len, file) == NULL) {
        fclose(file);
        api_key[0] = '\0';
        return 1;
    }
    
    fclose(file);
    
    api_key[strcspn(api_key, "\n")] = 0;
    
    if (strlen(api_key) == 0) {
        return 1;
    }
    
    return 0;
}

int delete_api_key_file() {
    char* config_dir_path = get_config_path();
    if (!config_dir_path) {
        fprintf(stderr, RED "Error: Could not get config directory path.\n" RESET);
        return 1;
    }
    char config_file_path[MAX_PATH_LENGTH];
    if (strlen(config_dir_path) + strlen(CONFIG_FILE) + 2 > sizeof(config_file_path)) {
        fprintf(stderr, RED "Error: Path too long for config_file_path buffer in delete_api_key_file.\n" RESET);
        return 1;
    }
    snprintf(config_file_path, sizeof(config_file_path), "%s/%s", config_dir_path, CONFIG_FILE);

    if (access(config_file_path, F_OK) == -1) {
        printf(YELLOW "API key file not found. Nothing to delete.\n" RESET);
        return 0;
    }

    if (remove(config_file_path) == 0) {
        printf(GREEN "API key deleted successfully.\n" RESET);
        return 0;
    } else {
        perror(RED "Error deleting API key file" RESET);
        return 1;
    }
}

int is_valid_api_key(const char* api_key) {
    if (!api_key || strlen(api_key) < 20) {
        return 0;
    }
    if (strncmp(api_key, "AIza", 4) != 0) {
    }
    for (size_t i = 0; i < strlen(api_key); i++) {
        if (!isalnum(api_key[i]) && api_key[i] != '-' && api_key[i] != '_') {
            return 0;
        }
    }
    return 1;
}

int setup_api_key() {
    char api_key_buffer[256];
    
    printf("\n" BRIGHT_GREEN "[KEY] GemCLI API Key Setup\n" RESET);
    printf("--------------------------------------------------------------------------------\n");
    printf("To use GemCLI, you need a free API key from Google.\n\n");

    printf(BOLD "Steps to get your API key:\n" RESET);
    printf("1. Visit: " CYAN "https://aistudio.google.com/apikey" RESET "\n");
    printf("2. Sign in with your Google account\n");
    printf("3. Click 'Create API Key'\n");
    printf("4. Copy the generated key\n");
    
    printf("Enter your Gemini API key: ");
    fflush(stdout);
    
    if (fgets(api_key_buffer, sizeof(api_key_buffer), stdin) == NULL) {
        printf(RED "Error reading API key.\n" RESET);
        return 1;
    }
    
    api_key_buffer[strcspn(api_key_buffer, "\n")] = 0;
    
    if (!is_valid_api_key(api_key_buffer)) {
        printf(RED "Invalid API key format. Please try again.\n" RESET);
        return 1;
    }
    
    if (save_api_key(api_key_buffer) != 0) {
        return 1;
    }
    
    printf(GREEN "Success: API key saved successfully!\n" RESET);
    return 0;
}

int load_model(char* model_name, size_t max_len) {
    char* config_dir_path = get_config_path();
    if (!config_dir_path) {
        return 1;
    }

    char model_file_path[MAX_PATH_LENGTH];
    if (strlen(config_dir_path) + strlen(MODEL_FILE) + 2 > sizeof(model_file_path)) {
        model_name[0] = '\0';
        return 1;
    }
    snprintf(model_file_path, sizeof(model_file_path), "%s/%s", config_dir_path, MODEL_FILE);

    FILE* file = fopen(model_file_path, "r");
    if (!file) {
        strncpy(model_name, DEFAULT_MODEL, max_len - 1);
        model_name[max_len - 1] = '\0';
        return 0;
    }

    if (fgets(model_name, max_len, file) == NULL) {
        fclose(file);
        strncpy(model_name, DEFAULT_MODEL, max_len - 1);
        model_name[max_len - 1] = '\0';
        return 0;
    }

    fclose(file);

    model_name[strcspn(model_name, "\n")] = 0;

    if (strlen(model_name) == 0) {
        strncpy(model_name, DEFAULT_MODEL, max_len - 1);
        model_name[max_len - 1] = '\0';
    }

    return 0;
}

int save_model(const char* model_name) {
    create_config_dir();

    char* config_dir_path = get_config_path();
    if (!config_dir_path) {
        return 1;
    }

    char model_file_path[MAX_PATH_LENGTH];
    if (strlen(config_dir_path) + strlen(MODEL_FILE) + 2 > sizeof(model_file_path)) {
        return 1;
    }
    snprintf(model_file_path, sizeof(model_file_path), "%s/%s", config_dir_path, MODEL_FILE);

    FILE* file = fopen(model_file_path, "w");
    if (!file) {
        return 1;
    }

    fprintf(file, "%s\n", model_name);
    fclose(file);

    chmod(model_file_path, 0600);

    return 0;
}

int setup_model() {
    char model_buffer[128];
    char current_model[128];

    // Load current model
    if (load_model(current_model, sizeof(current_model)) != 0) {
        strncpy(current_model, DEFAULT_MODEL, sizeof(current_model) - 1);
        current_model[sizeof(current_model) - 1] = '\0';
    }

    printf("\n" BRIGHT_GREEN "[MODEL] GemCLI Model Selection\n" RESET);
    printf("--------------------------------------------------------------------------------\n");
    printf("Current model: " CYAN "%s\n" RESET, current_model);
    printf("\n");

    printf(BOLD "Available models:\n" RESET);
    printf("1. " CYAN "gemini-1.5-flash-latest" RESET " (default - fastest)\n");
    printf("2. " CYAN "gemini-1.5-pro-latest" RESET " (latest pro model)\n");
    printf("3. " CYAN "gemini-1.5-flash" RESET "\n");
    printf("4. " CYAN "gemini-1.5-pro" RESET "\n");
    printf("5. " CYAN "gemini-2.0-flash-exp" RESET " (experimental)\n");
    printf("\n");
    printf("You can also enter a custom model name.\n");
    printf("\n");

    printf("Enter new model name (or press Enter to keep current): ");
    fflush(stdout);

    if (fgets(model_buffer, sizeof(model_buffer), stdin) == NULL) {
        printf(RED "Error reading model name.\n" RESET);
        return 1;
    }

    model_buffer[strcspn(model_buffer, "\n")] = 0;
    if (strlen(model_buffer) == 0) {
        printf(YELLOW "Keeping current model: %s\n" RESET, current_model);
        return 0;
    }


    if (strlen(model_buffer) < 3 || strlen(model_buffer) > 100) {
        printf(RED "Invalid model name. Please try again.\n" RESET);
        return 1;
    }

    if (save_model(model_buffer) != 0) {
        printf(RED "Error saving model configuration.\n" RESET);
        return 1;
    }

    printf(GREEN "Success: Model changed to '%s'!\n" RESET, model_buffer);
    return 0;
}
