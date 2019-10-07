#include "../include/server.h"

void *client_function(void *args)
{
	int thread_id = ((Thread_args *)args)->id;
	Client *client = &((Thread_args *)args)->client;
	int use_jail = ((Thread_args*)args)->use_jail;
    int is_fork = ((Thread_args *)args)->is_fork;

	printf("\n%d: Start Connection!\n", ((Thread_args *)args)->id);

	Request_t request = request_received(client, use_jail);
	gather_response_information(&request, client);
	free_headers(&request.headers);

	//close(client->socket);
	shutdown(client->socket, SHUT_WR);
    if (is_fork == 0)
    {
        thread_manager_exit_thread(args);
    }
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
		char *buffer[512];
		read(0, buffer, sizeof(buffer));
		return 0;
	}
	return 1;
}

void handle_threading(int use_jail)
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
            args->use_jail = use_jail;
            args->is_fork = 0;
            int rc = thread_manager_new_thread(client_function, (void *)args);
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

void handle_fork(int use_jail)
{
    pid_t origin = getpid();
    pid_t p;
    p = fork();
    printf("pid: %d", p);
    printf("pid: %d", origin);
    int running = 1;
    while (shoudStopRunning(running))
    {
        if (p != origin)
        {
            int accept_connection;
            Client client = connectToClient(&accept_connection);
            if (accept_connection == 1)
            {
                Thread_args *args = malloc(sizeof(Thread_args));
                args->client = client;
                args->use_jail = use_jail;
                pid_t p_two;
                p_two = fork();
                printf("pid: %d", p_two);
                if (p_two != 0)
                {
                    args->is_fork == 1;
                    client_function((void *)args);
                    exit(0);
                }
            }
        }
        else
        {
            //handle childsignals
        }
    }
    request_stop_reciving_data = 1;
}

void create_a_deamon()
{
    //code here 
}

void start_server(char* document_root_path, int port, int log, int deamon, int setting, int use_jail)
{
	// Open log files before using chroot
	logging_open(log);
	
	if (use_jail == 1)
	{
		change_chroot(document_root_path);
	}

    // Initilize the server
    set_ip_type(0);
    set_port(port);
    set_protocol(0);
    createSocket(10);
    setToNonBlocking();

    request_init(document_root_path, use_jail);
    
    if (setting == 1)
    {
        if (deamon == 1)
        {
            create_a_deamon();
        }
        handle_fork(use_jail);
    }
    else if(setting == 0)
    {
        if (deamon == 1)
        {
            create_a_deamon();
        }
        handle_threading(use_jail);
    }

    closeServer();

	logging_close();
}

void change_chroot(char* document_root_path)
{
	printf("%s\n", document_root_path);

	// check that we have the proper privileges (need root for chroot)
   	if (geteuid() != 0 && getegid() != 0) {
     	fprintf(stderr, "Error: no root privileges.\n");
      	exit(EXIT_FAILURE);
    }

	chdir(document_root_path);
	if (chroot(document_root_path) == -1)
	{
        // This should never happend, because this is check before chroot is called
		if (errno == EPERM)
		{
			fprintf(stderr, "Error: chroot insufficient privilege.\n");
			exit(EXIT_FAILURE);
		}
		fprintf(stderr, "Error: in chroot.\n");
	}

	gid_t orig_gid = getgid();
 	uid_t orig_uid = getuid();
    
	setregid(-1, orig_gid);
	setreuid(-1, orig_uid);

	if ((getegid() != orig_gid) || (geteuid() != orig_uid)) {
		fprintf(stderr, "Error: Failed to drop privileges, aborting\n");
		exit(EXIT_FAILURE);
	}
}