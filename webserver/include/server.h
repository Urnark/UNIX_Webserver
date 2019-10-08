#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/resource.h>

#include "../include/threadManager.h"
#include "../include/socket.h"
#include "../include/request.h"
#include "../include/response.h"
#include "../include/logging.h"

#define START_NUM_THREADS 5

typedef struct thread_args
{
	int id;
	Client client;
	int use_jail;
    int is_fork;
} Thread_args;

void start_server(char* document_root_path, int port, int log, int deamon, int setting, int use_jail);
void change_chroot(char* document_root_path);


#endif