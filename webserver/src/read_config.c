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