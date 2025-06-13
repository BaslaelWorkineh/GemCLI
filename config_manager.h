#ifndef CONFIG_MANAGER_H
#define CONFIG_MANAGER_H

#include <stdio.h>
#include <stddef.h>

int setup_api_key();
int load_api_key(char* api_key, size_t max_len);
int save_api_key(const char* api_key);
int delete_api_key_file();
int is_valid_api_key(const char* api_key);
void create_config_dir();
char* get_config_path();


int load_model(char* model_name, size_t max_len);
int save_model(const char* model_name);
int setup_model();

#endif // CONFIG_MANAGER_H
