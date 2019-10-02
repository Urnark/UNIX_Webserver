#ifndef READ_CONFIG_H
#define READ_CONFIG_H
#include <stdio.h>
#include <string.h>

typedef struct _config_data
{
    char* config_data;
} ServerConfig;


int read_config_file(char* data, ServerConfig sc);

#endif