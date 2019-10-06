#ifndef READ_CONFIG_H
#define READ_CONFIG_H
#include <stdio.h>
#include <string.h>

typedef struct _config_data
{
    char* config_data;
} ServerConfig;

// For jail
ServerConfig server_configurations;
void init_configurations();

void read_config_file(char* data, ServerConfig* sc);
int repair_config_file();
void create_config_file();

#endif