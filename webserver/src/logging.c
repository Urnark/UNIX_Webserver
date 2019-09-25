#include "../include/logging.h"
#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const char LOGGING_PATH_LOG_FILE[] = "log/server.log";
const char LOGGING_PATH_LOG_ERR_FILE[] = "log/server.err";

void logging_log_to_file(int to_file)
{
    logging_file = to_file;
}

void _logging_log_f(const char* path, char* ip, char* userid, char* time, char*  request, int response_code, char* size_in_bytes, char* referer, char* user_agent)
{
    FILE * f = fopen(path, "a");
    if (f)
    {
        if (referer == NULL)
            fprintf(f, "%s - %s %s \"%s\" %d %s\n", ip, userid, time, request, response_code, size_in_bytes);
        else
            fprintf(f, "%s - %s %s \"%s\" %d %s \"%s\" \"%s\"\n", ip, userid, time, request, response_code, size_in_bytes, referer, user_agent);
        
        fclose(f);
    }
}

void _logging_log(int pri, char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent)
{
    int length = snprintf(NULL, 0, "%d", size_in_bytes);
    char* size = malloc(length + 1);
    snprintf(size, length + 1, "%d", size_in_bytes);

    if (logging_file)
    {
        if (referer == NULL)
            _logging_log_f((pri == LOG_ERR?LOGGING_PATH_LOG_ERR_FILE:LOGGING_PATH_LOG_FILE), ip, (userid != NULL?userid:"-"), 
                time, request, response_code, (size_in_bytes != 0?size:"-"), NULL, NULL);
        else
            _logging_log_f((pri == LOG_ERR?LOGGING_PATH_LOG_ERR_FILE:LOGGING_PATH_LOG_FILE), ip, (userid != NULL?userid:"-"), 
                time, request, response_code, (size_in_bytes != 0?size:"-"), referer, user_agent);
    }
    else
    {
        openlog("CardServer", LOG_NDELAY, LOG_USER);
        if (referer == NULL)
            syslog(pri, "%s - %s %s \"%s\" %d %s %s %s", ip, (userid != NULL?userid:"-"), 
                time, request, response_code, (size_in_bytes != 0?size:"-"));
        else
            syslog(pri, "%s - %s %s \"%s\" %d %s \"%s\" \"%s\"", ip, (userid != NULL?userid:"-"), 
                time, request, response_code, (size_in_bytes != 0?size:"-"), referer, user_agent);
        
        closelog();
    }

    free(size);
}

void logging_log_clg(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent)
{
    _logging_log(LOG_INFO, ip, userid, time, request, response_code, size_in_bytes, referer, user_agent);
}

void logging_log_err_clg(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent)
{
    _logging_log(LOG_ERR, ip, userid, time, request, response_code, size_in_bytes, referer, user_agent);
}

void logging_log(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes)
{
    logging_log_clg(ip, userid, time, request, response_code, size_in_bytes, NULL, NULL);
}

void logging_log_err(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes)
{
    logging_log_clg(ip, userid, time, request, response_code, size_in_bytes, NULL, NULL);
}