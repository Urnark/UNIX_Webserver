#include <stdio.h>
#include <stdlib.h>

#include "../include/thread.h"

#define NUM_THREADS 5

void* printMessage(void* threadid)
{
    printf("\n%d: Hello World!\n",(int) threadid);
	pthread_exit(NULL);
}

int main(int argc, char const *argv[])
{
	init_threads(5);

	for (int rc = 0, i = 0; i < NUM_THREADS; i++)
	{
		rc = new_thread(printMessage);
		if (rc) {
			fprintf(stderr, "ERROR: pthread_create() returned %d\n", rc);
			exit(-1);
		}
	
	}

	terminate_threads();
    return 0;
}
