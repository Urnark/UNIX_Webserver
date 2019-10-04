#include "../include/read_config.h"
#include <stdlib.h>

void read_config_file(char* data, ServerConfig* sc)
{
	static const char filename[]="lab2-config";
    char buffer[256];
    FILE* f = fopen(filename , "r");

    if(f)
    {
        char* ptr = NULL;
        while (ptr == NULL && fgets(buffer, 256, f) != NULL)
        {
            ptr = strstr(buffer, data);
            if (ptr != NULL)
            {
                size_t length = strlen((char*)(buffer + strlen(data)));
                sc->config_data = malloc(length);
                strncpy(sc->config_data, buffer + strlen(data), length - 1);
                sc->config_data[length - 1] ='\0';
            }
        }
    }

    fclose(f);
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
