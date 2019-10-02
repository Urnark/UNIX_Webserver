#include "../include/read_config.h"

int read_config_file(char* data, ServerConfig sc)
{
	static const char filename[]="lab2-config";
    char buffer[256];
    FILE* f = fopen(filename , "r");

    if(f)
    {
        fgets(buffer, 256, f);
        char *ptr = strstr(buffer, data);
        if (ptr != NULL)
        {
            sc.config_data = buffer + strlen(data);
	        printf("buffer %s", sc.config_data);
        }

    }
}