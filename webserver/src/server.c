#include "../include/server.h"

/**
 * Function which handles the actual request and response for a conected client
 * for all request methods implemented.
 * 
 * args: holds information about the connected client and the id of the handler.
 * */
void *client_function(void *args)
{
    int thread_id = ((Thread_args *)args)->id;
    Client *client = &((Thread_args *)args)->client;
    int use_jail = ((Thread_args *)args)->use_jail;
    int is_fork = ((Thread_args *)args)->is_fork;

    //printf("\n%d: Start Connection!\n", ((Thread_args *)args)->id);

    Request_t request = request_received(client, use_jail);
    gather_response_information(&request, client);
    free_headers(&request.headers);

    shutdown(client->socket, SHUT_RDWR);
    close(client->socket);
    if (is_fork == 0)
    {
        thread_manager_exit_thread(args);
    }
}

/**
 * Stops the server from running and quit it properly. 
 * Only if not run as a deamon.
 * 
 * running: true or false.
 * */
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

/**
 * Handler of the thread-request-method. 
 * Connects to a client and creates a new thread for handling the request and response.
 * Terminates the thread after it is done or sends a kill command,
 * if the server has to stop running.
 * 
 * use_jail: if jail is true or false.
 * deamon: if deamon is true or false.
 * */
void handle_threading(int use_jail, int daemon)
{
    thread_manager_init_threads(START_NUM_THREADS);
    int running = 1;
    while (daemon==1?1:shoudStopRunning(running))
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

/**
 * signal handler to force children to stop handling a request and kill themselves.
 * */
void handler(int sig_nr)
{
    if (sig_nr == SIGUSR1)
    {
        //printf("SIGUSR1 to %d\n", getpid());
        request_stop_reciving_data = 1;
    }
}

/**
 * Handler of the fork-request-method.
 * Connects to a client and executes fork. 
 * The child will handle the request and response properly,
 * while the parent will connect to a new client or quit.
 * 
 * use_jail: if jail is true or false.
 * deamon: if deamon is true or false.
 * */
void handle_fork(int use_jail, int daemon)
{
    signal(SIGCHLD, SIG_IGN);
    
    int running = 1;
    while (daemon==1?1:shoudStopRunning(running))
    {
        int accept_connection;
        Client client = connectToClient(&accept_connection);
        if (accept_connection == 1)
        {
            pid_t p_two;
            p_two = fork();
            if (p_two == 0)
            {
                signal(SIGUSR1, handler);

                Thread_args *args = malloc(sizeof(Thread_args));
                args->client = client;
                args->use_jail = use_jail;
                args->is_fork == 1;
                args->id = getpid();
                client_function((void *)args);
                free(args);
                exit(0);
            }
        }
    }
    signal(SIGUSR1, SIG_IGN);
    kill(0, SIGUSR1);
}

/**
 * Make the process a daemon.
 * 
 * document_root_path: change to the www-folder/ source folder of the webpages.
 * */
void create_a_deamon(char *document_root_path)
{
    int fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    
    umask(0);
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0)
    {
        fprintf(stderr, "Error: getrlimit\n");
        free_configurations();
        exit(1);
    }

    pid = fork();
    if (pid != 0)
    {
        free_configurations();
        exit(0);
    }
    setsid();
    if(signal(SIGHUP, SIG_IGN) < 0)
    {
        fprintf(stderr, "Error: sigaction\n");
        free_configurations();
        exit(1);
    }
    pid = fork();
    if (pid != 0)
    {
        free_configurations();
        exit(0);
    }
    if (chdir(document_root_path))
    {
        fprintf(stderr, "Error: chdir\n");
        free_configurations();
        exit(1);
    }
    if (rl.rlim_max == RLIM_INFINITY)
    {
        rl.rlim_max = 1024;
    }
    printf("PID: %d\n", getpid());
    int i;
    for (i = 0; i < rl.rlim_max; i++)
    {
        close(i);
    }

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
}

/**
 * Starts the main serverprocess with the specification given.
 * 
 * document_root_path: Path to the webcontent, eg: index.html
 * port: port the server is to be connected to.
 * log: if a log is writen to syslog or a file.
 * deamon: if the process is to be run as a daemon.
 * setting: which request-method is to use.
 * use_jail: if jail is to be used or realpath.
 * */
void start_server(char *document_root_path, int port, int log, int deamon, int setting, int use_jail)
{
    check_www_path();

    logging_get_path();
    read_error_page(document_root_path);

    if (deamon == 1)
    {
        create_a_deamon(document_root_path);
    }
    else
    {
        printf("Press ENTER to stop the server.\n");
    }
    

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
    createSocket(1000);
    setToNonBlocking();

    request_init(document_root_path, use_jail);

    if (setting == 1)
    {
        handle_fork(use_jail, deamon);
    }
    else if (setting == 0)
    {
        handle_threading(use_jail, deamon);
    }

    closeServer();
    
    free_error_page();
    logging_close();
}

void change_chroot(char *document_root_path)
{
    printf("%s\n", document_root_path);

    // check that we have the proper privileges (need root for chroot)
    if (geteuid() != 0 && getegid() != 0)
    {
        fprintf(stderr, "Error: no root privileges.\n");
        exit(EXIT_FAILURE);
    }

    // Get the UID of the uid and gui of the user that started the server.
    struct passwd* pwd = getpwnam(getlogin());

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

    //setregid(pwd->pw_gid, pwd->pw_gid);
    //setreuid(pwd->pw_uid, pwd->pw_uid);
}