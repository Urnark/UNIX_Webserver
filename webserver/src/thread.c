#include "../include/thread.h"

void *client_thread(void *args)
{
	int thread_id = ((Thread_args *)args)->id;
	Client *client = &((Thread_args *)args)->client;

	printf("\n%d: Start Connection!\n", ((Thread_args *)args)->id);

	Request_t request = request_received(client);
	gather_response_information(&request, client);
	free_headers(&request.headers);

	//close(client->socket);
	shutdown(client->socket, SHUT_WR);
	thread_manager_exit_thread(args);
}

void exec_threading()
{
	thread_manager_init_threads(START_NUM_THREADS);

	int running = 1;
	while (shoudStopRunning(running))
	{
		int accept_connection;
		Client client = connectToClient(&accept_connection);
		if (accept_connection == 1)
		{
			Thread_args *args = malloc(sizeof(Thread_args));
			args->client = client;
			int rc = thread_manager_new_thread(client_thread, (void *)args);
			if (rc)
			{
				fprintf(stderr, "ERROR: pthread_create() returned %d\n", rc);
				exit(-1);
			}
		}
	}
	request_stop_reciving_data = 1;

	thread_manager_terminate_threads();
}