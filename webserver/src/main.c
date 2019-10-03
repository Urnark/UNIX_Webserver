#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/threadManager.h"
#include "../include/socket.h"
#include "../include/request.h"
#include "../include/response.h"
#include "../include/logging.h"

#define START_NUM_THREADS 5

typedef struct thread_args{
	int id;
	Client client;
} Thread_args;

void* client_thread(void* args)
{
	int thread_id = ((Thread_args*)args)->id;
	Client* client = &((Thread_args*)args)->client;

    printf("\n%d: Start Connection!\n", ((Thread_args*)args)->id);

	Request_t request = request_received(client);
	gather_response_information(&request, client);
	free_headers(&request.headers);
	
	//close(client->socket);
	shutdown(client->socket, SHUT_WR);
	thread_manager_exit_thread(args);
}

int shoudStopRunning(int running)
{
	// For checking if the server should be terminated.
	int retval;
	fd_set read_fds;
	struct timeval tv;

	// Watch stdin(fd 0) for input.
	FD_ZERO(&read_fds);
	FD_SET(0, &read_fds);
	// Block for 1 second
	tv.tv_sec = 0;
	tv.tv_usec = 5000;
	
	retval = select(1, &read_fds, NULL, NULL, &tv);
	if (retval != 0)
	{
		printf("Stop Running\n");
		// Flush input stream
		char* buffer[512];
		read(0, buffer, sizeof(buffer));
		return 0;
	}
	return 1;
}

int main(int argc, char const *argv[])
{
	logging_log_to_file(1);

	// Initilize the server
	set_ip_type(0);
	set_port(4444);
	set_protocol(0);
	createSocket(10);
	setToNonBlocking();

	thread_manager_init_threads(START_NUM_THREADS);

	request_init();

	int running = 1;
	while (shoudStopRunning(running))
	{
		int accept_connection;
		Client client = connectToClient(&accept_connection);
		if (accept_connection == 1)
		{
			Thread_args* args = malloc(sizeof(Thread_args));
			args->client = client;
			int rc = thread_manager_new_thread(client_thread, (void*)args);
			if (rc) {
				fprintf(stderr, "ERROR: pthread_create() returned %d\n", rc);
				exit(-1);
			}
		}
	}
	request_stop_reciving_data = 1;

	thread_manager_terminate_threads();
	closeServer();

    return 0;
}
