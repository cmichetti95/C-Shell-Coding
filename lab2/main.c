/* Connor Michetti */

/* To compile my file, just type make into the command line, it will compile the program into an executable.
 * To run the program, simply type main followed by three different integers, all seperated by spaces. E.g.
 * main 20 1 1
 * The first integer is the time the main will sleep, the following two integers are how many producer threads
 * and how many consumer threads respectively.

/* A program to solve the bounded buffer problem. */

#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include "buffer.h"

/* buffer indexing */
int in = 0;
int out = 0;

/* semaphores, mutex, and buffer array */
sem_t empty;
sem_t full;
pthread_mutex_t mutex;
buffer_item buffer[BUFFER_SIZE];

/* insert function */
int insert_item(buffer_item item)
{
	/* insert item */
	buffer[in] = item;
	printf("producer produced %d\n", item);
	
	/* increment where to insert */
	in = (in + 1) % BUFFER_SIZE;

	/* return 0 if success, -1 if error */
	return 0;
}

/* remove function */
int remove_item(buffer_item *item)
{
	/* remove item */
	
	*item = buffer[out];
	printf("consumer consumed %d\n", *item);
	
	/* increment where we're removing from */
	out = (out + 1) % BUFFER_SIZE;
	
	/*return 0 if success, -1 if error */
	return 0;

}

/* producer function */
void *producer(void *param)
{
	unsigned int temp;
	buffer_item random;

	while(1)
	{

	    /* sleep for random amount of time */
	    random = rand_r(&temp);
	    sleep(temp % 3);

	    sem_wait(&empty);
	    pthread_mutex_lock(&mutex);

	    /* checking for errors */
	    if(insert_item(random) < 0) 
	    {
		fprintf(stderr, "Error: insert_item failure");	
		exit(-1);	
	    }

	    sem_post(&full);
	    pthread_mutex_unlock(&mutex);

	}
}

/* consumer function */
void *consumer(void *param)
{
	unsigned int temp;
	buffer_item random;

	while(1)
	{

	    /* sleep for random amount of time */
	    random = rand_r(&temp);
	    sleep(random % 3);

	    sem_wait(&full);
	    pthread_mutex_lock(&mutex);

	    /* checking for errors */
	    if(remove_item(&random) < 0)
	    {
		fprintf(stderr, "Error: remove_item failure");
		exit(-2);
	    }

	    sem_post(&empty);
	    pthread_mutex_unlock(&mutex);

	}

}

/* main */
int main(int argc, char* argv[])
{
	int sleepyTime, prodThreads, conThreads;
	pthread_attr_t attr;


	if(argc != 4)
	{
	    printf("Improper amount of args");
	}
	else
	{

	/* Get command line args */
	char *a = argv[1];
	char *b = argv[2];
	char *c = argv[3];

	sleepyTime = atoi(a);
	prodThreads = atoi(b);
	conThreads = atoi(c);

	/* Initialize mutex, semaphores, and attribte */
	pthread_mutex_init(&mutex, NULL);
	sem_init(&empty, 0, BUFFER_SIZE);
	sem_init(&full, 0, 0);
	pthread_attr_init(&attr);

	/* Initialize producer and consumer arrays */
	pthread_t producers[prodThreads];
	pthread_t consumers[conThreads];

	/* make producer threads */
	int i;
	for(i = 0; i < prodThreads; i++)
	{
	    pthread_create(&producers[i], &attr, producer, NULL);
	}
	
	/* make consumer threads */
	int j;
	for(j = 0; j < conThreads; j++)
	{
	   pthread_create(&consumers[j], &attr, consumer, NULL);
	}

	/* Sleep */
	sleep(sleepyTime);

	/* Destroy semaphores and mutex */
	pthread_mutex_destroy(&mutex);
	sem_destroy(&full);
	sem_destroy(&empty);

	/* Exit */
	exit(0);
	}

	return 0;
}
