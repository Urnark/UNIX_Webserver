#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include "../include/threadManager.h"
#include "../include/socket.h"

#define NUM_THREADS 5

typedef struct thread_args{
	int id;
	Client client;
} Thread_args;

void* client_thread(void* args)
{
	int thread_id = ((Thread_args*)args)->id;
	Client* client = &((Thread_args*)args)->client;

    printf("%d: Hello World!\n", ((Thread_args*)args)->id);

	char buf[512] = "Hello!\n";
	if(send(client->socket, buf, 8, 0) == -1) {
		fprintf(stderr, "ERROR: Can not send 'Hello' to the client.\n");
	}

	memset(buf, 0, sizeof(buf));
	if (recv(client->socket, buf, sizeof(buf), 0) == -1) {
		fprintf(stderr, "ERROR: Can not recive string from the client.\n");
	}
	printf("From client [%d]: %s", thread_id, buf);

	close(client->socket);
	thread_manager_exit_thread(thread_id);
}

int shoudlStopRunning(int running)
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
	// Initilize the server
	set_ip_type(0);
	set_port(4444);
	set_protocol(0);
	createSocket(10);
	setToNonBlocking();

	thread_manager_init_threads(NUM_THREADS);

	int running = 1;
	while (running)
	{
		running = shoudlStopRunning(running);

		int accept_connection;
		Client client = connectToClient(&accept_connection);
		if (accept_connection == 1)
		{
			Thread_args args;
			args.client = client;
			int rc = thread_manager_new_thread(client_thread, (void*)&args);
			if (rc) {
				fprintf(stderr, "ERROR: pthread_create() returned %d\n", rc);
				exit(-1);
			}
		}
	}

	thread_manager_terminate_threads();
	closeServer();

    return 0;
}
