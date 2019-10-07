#include "../include/read_config.h"
#include <stdlib.h>

void init_configurations()
{
	static const char filename[]="lab2-config";
    char buffer[256];
    FILE* f = fopen(filename , "r");

    if(f)
    {
        int pointer = ftell(f);
        fseek(f, 0L, SEEK_END);
        int size = ftell(f);
        fseek(f, pointer, SEEK_SET);

        server_configurations.config_data = malloc(size + 1);
        if (server_configurations.config_data)
        {
            fread(server_configurations.config_data, 1, size, f);
            server_configurations.config_data[size] = '\0';
        }
    }
    
    fclose(f);
}

void read_config_file(char* data, ServerConfig* sc)
{
    char* ptr = strstr(server_configurations.config_data, data);
    if (ptr != NULL)
    {
        size_t length = strlen((char*)(ptr + strlen(data)));
        sc->config_data = malloc(length);
        strncpy(sc->config_data, ptr + strlen(data), length - 1);
        sc->config_data[length - 1] ='\0';
    }
}

int repair_config_file(){
    static const char filename[]="lab2-config";
    int status;
    status = remove(filename);
    if (status == 0){
        create_config_file();
        printf("Successfully repaired \"lab2-config\" file.\n");
    }
    else if (status==1)
    {
        create_config_file();
        printf("Original file not found. New \"lab2-config\" file was created.\n");
    }
    return 0;
}

void create_config_file(){
    static const char filename[]="lab2-config";
    FILE* f = fopen(filename , "w");

    if(f){
        fprintf(f, "SERVER_NAME=Card_Server\n");
        fprintf(f, "SERVER_VERSION=0.2\n");
        fprintf(f, "SERVER_PORT=4444\n");
        fprintf(f, "SERVER_AS_DEAMON=1\n");
        fprintf(f, "SERVER_LOG=0\n");
        fprintf(f, "SERVER_SETTING=0\n");
    }

    fclose(f);
}
