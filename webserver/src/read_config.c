#include "../include/read_config.h"

char* read_config_file(char* data)
{
    char* buffer = NULL;
    FILE* f = fopen("server.lab2-config" , "r");

    if(f)
    {
        fgets(buffer, 256, f);
        char *ptr = strstr(buffer, data);
        if (ptr != NULL)
        {
            printf("buffer %s", buffer);
            buffer = buffer + strlen(data);
            printf("buffer %s", buffer);
        }
    }

    return buffer;

}