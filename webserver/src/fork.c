#include "../include/fork.h"

int *client_fork(void *args)
{
	int fork_id = ((Fork_args *)args)->id;
	Client *client = &((Fork_args *)args)->client;

	printf("\n%d: Start Connection!\n", ((Fork_args *)args)->id);

	Request_t request = request_received(client);
	gather_response_information(&request, client);
	free_headers(&request.headers);

	shutdown(client->socket, SHUT_WR);
	return 0;
}

void exec_preforking()
{
	array_create_array(&fork_array, sizeof(Fork_array_info), 6);
	pid_t origin = getpid(void);
	pid_t p;
	p = fork();
	p = fork();
	printf("pid: %d", p);

	int running = 1;
	while (shoudStopRunning(running))
	{
		if (p != origin)
		{
			Fork_array_info info;

			info.free_fork = 1;

			if (fork_array[p].element)
			{
				info = fork_array[p].element;
				if (info.kill_fork == 0)
				{
					array_remove_element(fork_array, p);
					exit(0);
				}
				fork_array[p].element = info;
			}
			else
			{
				info.id = p;
				info.kill_fork = 1;
				array_push_element(fork_array, info);
			}
			int accept_connection;
			Client client = connectToClient(&accept_connection);
			if (accept_connection == 1)
			{
				Fork_args *args = malloc(sizeof(Fork_args));
				args->client = client;
				int rc = client_fork((void *)args, info);

				if (rc)
				{
					fprintf(stderr, "ERROR: Could not handle clientrequest.");
					exit(-1);
				}
				info.free_fork = 0;
				fork_array[p].element = info;
			}
		}
		else
		{
			int check_fork;
			int busy_fork;
			for (fork_array[check_fork].element)
			{
				Fork_array_info check = fork_array[check_fork].element;
				if (check.free_fork == 1)
				{
					busy_fork++;
				}
				check_fork++;
			}
			if (busy_fork == fork_array.size)
			{
				p = fork();
			}
			else
			{
				Fork_array_info toKill = fork_array[fork_array.size].element;
				toKill.kill_fork = 0;
			}
		}
	}
	request_stop_reciving_data = 1;
}
