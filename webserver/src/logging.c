#include "../include/logging.h"
#include <syslog.h>
#include <string.h>
#include <stdlib.h>
#include <linux/limits.h>
#include <unistd.h>

char* LOGGING_PATH_LOG_FILE = NULL;
char* LOGGING_PATH_LOG_ERR_FILE = NULL;

/**
 * Close the logging
 * */
void logging_close()
{
    if (logging_file)
    {
        fclose(logging_fd);
        fclose(logging_fd_err);
    }
    else
    {
        closelog();
    }
    
    free(LOGGING_PATH_LOG_FILE);
    free(LOGGING_PATH_LOG_ERR_FILE);
}

/**
 * Tell the logging to get the paths to the files that is should use
 * */
void logging_get_path()
{
    char path[PATH_MAX];
    getcwd(path, sizeof(path));
    
    char log[] = "/log/server.log";
    LOGGING_PATH_LOG_FILE = malloc(strlen(path) + strlen(log) + 1);
    strcpy(LOGGING_PATH_LOG_FILE, path);
    strcat(LOGGING_PATH_LOG_FILE, log);

    char log_err[] = "/log/server.err";
    LOGGING_PATH_LOG_ERR_FILE = malloc(strlen(path) + strlen(log_err) + 1);
    strcpy(LOGGING_PATH_LOG_ERR_FILE, path);
    strcat(LOGGING_PATH_LOG_ERR_FILE, log_err);
}

/**
 * Open the two files or the syslog
 * */
void logging_open(int to_file)
{
    logging_file = to_file;

    if (logging_file)
    {
        logging_fd = fopen(LOGGING_PATH_LOG_FILE, "a");
        // To be able to write to file before closing it
        setlinebuf(logging_fd);
        logging_fd_err = fopen(LOGGING_PATH_LOG_ERR_FILE, "a");
        // To be able to write to file before closing it
        setlinebuf(logging_fd_err);
    }
    else
    {
        openlog("CardServer", LOG_NDELAY, LOG_USER);
    }
    
}

/**
 * Internal function! Loggs a request to the file descriptor
 * 
 * fd: The file descriptor that is to be written to
 * ip: The ipv4 of the client
 * userid: the name of the client user, for now it is not in use
 * time: The time the response was sent to the client
 * request: The request line of the request from the client
 * response_code: HTTP status code
 * size_in_bytes: Size of the response
 * referer: Referer, the page that the request comes from
 * user_agent: Which browser the request was sent from
 * */
void _logging_log_f(FILE* fd, char* ip, char* userid, char* time, char*  request, int response_code, char* size_in_bytes, char* referer, char* user_agent)
{
    if (fd)
    {
        if (referer == NULL)
            fprintf(fd, "%s - %s %s \"%s\" %d %s\n", ip, userid, time, request, response_code, size_in_bytes);
        else
            fprintf(fd, "%s - %s %s \"%s\" %d %s \"%s\" \"%s\"\n", ip, userid, time, request, response_code, size_in_bytes, referer, user_agent);
    }
}

/**
 * Internal function! Loggs a request to either the syslog or the files and specify the priority of the log.
 * 
 * pri: Priority of the log, LOG_ERR or LOG_INFO
 * ip: The ipv4 of the client
 * userid: the name of the client user, for now it is not in use
 * time: The time the response was sent to the client
 * request: The request line of the request from the client
 * response_code: HTTP status code
 * size_in_bytes: Size of the response
 * referer: Referer, the page that the request comes from
 * user_agent: Which browser the request was sent from
 * */
void _logging_log(int pri, char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent)
{
    int length = snprintf(NULL, 0, "%d", size_in_bytes);
    char* size = malloc(length + 1);
    snprintf(size, length + 1, "%d", size_in_bytes);

    char* r = request;
    int tooLong = 0;
    if (strlen(r) >= PATH_MAX)
    {
        tooLong = 1;
        r = malloc(20);
        strcpy(r, "[Request Too long]");
    }

    if (logging_file)
    {
        if (referer == NULL)
            _logging_log_f((pri == LOG_ERR?logging_fd_err:logging_fd), ip, (userid != NULL?userid:"-"), 
                time, r, response_code, (size_in_bytes != 0?size:"-"), NULL, NULL);
        else
            _logging_log_f((pri == LOG_ERR?logging_fd_err:logging_fd), ip, (userid != NULL?userid:"-"), 
                time, r, response_code, (size_in_bytes != 0?size:"-"), referer, user_agent);
    }
    else
    {
        if (referer == NULL)
            syslog(pri, "%s - %s %s \"%s\" %d %s", ip, (userid != NULL?userid:"-"), 
                time, r, response_code, (size_in_bytes != 0?size:"-"));
        else
            syslog(pri, "%s - %s %s \"%s\" %d %s \"%s\" \"%s\"", ip, (userid != NULL?userid:"-"), 
                time, r, response_code, (size_in_bytes != 0?size:"-"), referer, user_agent);
    }

    free(size);
    if (tooLong == 1)
    {
        free(r);
    }
}

/**
 * Logs a request with priority of LOG_INFO with the Combined Log Format.
 * 
 * ip: The ipv4 of the client
 * userid: the name of the client user, for now it is not in use
 * time: The time the response was sent to the client
 * request: The request line of the request from the client
 * response_code: HTTP status code
 * size_in_bytes: Size of the response
 * referer: Referer, the page that the request comes from
 * user_agent: Which browser the request was sent from
 * */
void logging_log_clg(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent)
{
    _logging_log(LOG_INFO, ip, userid, time, request, response_code, size_in_bytes, referer, user_agent);
}

/**
 * Logs a request with priority of LOG_ERR with the Combined Log Format.
 * 
 * ip: The ipv4 of the client
 * userid: the name of the client user, for now it is not in use
 * time: The time the response was sent to the client
 * request: The request line of the request from the client
 * response_code: HTTP status code
 * size_in_bytes: Size of the response
 * referer: Referer, the page that the request comes from
 * user_agent: Which browser the request was sent from
 * */
void logging_log_err_clg(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes, char* referer, char* user_agent)
{
    _logging_log(LOG_ERR, ip, userid, time, request, response_code, size_in_bytes, referer, user_agent);
}

/**
 * Logs a request with priority of LOG_INFO with the Common Log Format.
 * 
 * ip: The ipv4 of the client
 * userid: the name of the client user, for now it is not in use
 * time: The time the response was sent to the client
 * request: The request line of the request from the client
 * response_code: HTTP status code
 * size_in_bytes: Size of the response
 * */
void logging_log(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes)
{
    logging_log_clg(ip, userid, time, request, response_code, size_in_bytes, NULL, NULL);
}

/**
 * Logs a request with priority of LOG_ERR with the Common Log Format.
 * 
 * ip: The ipv4 of the client
 * userid: the name of the client user, for now it is not in use
 * time: The time the response was sent to the client
 * request: The request line of the request from the client
 * response_code: HTTP status code
 * size_in_bytes: Size of the response
 * */
void logging_log_err(char* ip, char* userid, char* time, char* request, int response_code, int size_in_bytes)
{
    logging_log_err_clg(ip, userid, time, request, response_code, size_in_bytes, NULL, NULL);
}