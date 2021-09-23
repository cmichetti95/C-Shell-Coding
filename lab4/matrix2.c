/* Connor Michetti */

/* To compile my file, simply type 'make' into the command line. To run the program, type 'exec_matrix' into the command line.
 * The program will prompt for number of threads, only enter an integer between 1 - 7, as the program does NOT error check for this
 * (nothing in the lab write-up specified having to do this).
 */

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/time.h>

/* Matricies rows/columns */
#define N 1200
#define M 1000
#define P 500

/* Matrix declarations */
int a[N][M];
int b[M][P];
int c[N][P];
int original[N][P];

/* Globals to help with indices between threads */
int totalThreads;
int threadNum;

void *multiplyArrays(void *param)
{
	int forIndex, forIndexTwo, i, j, k;

	/* Math for thread index (explanation below)
	 * Essentially should walk through. threadNum is which thread we're on and totalThreads is how many
	 * total thread there are. So for example if there are 7 threads and we're on thread 0
	 * then forIndex will be 0 * N / 7 which = 0. Then forIndexTwo will be (0 + 1) * 1200 / 7 = roughly 171 which 
	 * will give us how many columns we're going across (500 rows per column). threadNum then gets incremented for
	 * the next thread.
	 */

	forIndex = threadNum * N / totalThreads;
	forIndexTwo = (threadNum + 1) * N / totalThreads;
	threadNum++;

	/* Perform the multiplications based on the above math */
	for(i = forIndex; i < forIndexTwo; i++)
	{
	    for(j = 0; j < P; j++)
	    {
		c[i][j] = 0;
		for(k = 0; k < M; k++)
		{
		    c[i][j] += a[i][k] * b[k][j];
		}
	    }
	}


	pthread_exit(0);
}

int compare()
{
	int i, j, returnVal;
	returnVal = 0;

	/* Return non-zero if there's any discrepencies */
	for(i = 0; i < N; i++)
	{
	    for(j = 0; j < P; j++)
	    {
		if(original[i][j] != c[i][j])
		{
		     returnVal++;
		}
	    }
	}
	return returnVal;
}

void makeOrig()
{
	int i, j, k;

	/* Initializing "original" which is a matrix to check C against for correctness */
	for(i = 0; i < N; i++)
	{
	    for(j = 0; j < P; j++)
	    {
		original[i][j] = 0;
		for(k = 0; k < M; k++)
		{
		    original[i][j] += a[i][k] * b[k][j];
		}
	    }
	}
}

int main(void)
{
	int i, j, k, l, check, temp;
	pthread_attr_t attr;
	struct timeval runtime, endtime;

	/* Initialize matricies A and B */
	for(i = 0; i < N; i++)
	{
	    for(j = 0; j < M; j++)
	    {
		a[i][j] = i + j;
	    }
	}

	for(i = 0; i < M; i++)
	{
	    for(j = 0; j < P; j++)
	    {
		b[i][j] = j;
	    }
	}

	/* Initialize original */
	makeOrig();

	/* Get number of threads */
	printf("Enter a number between 1 - 7: \n");
	scanf("%d", &temp);
	printf("Threads Seconds Error\n");


	/* Initialize pthread stuff */
	pthread_t threads[temp];
	pthread_attr_init(&attr);
	totalThreads = 0;

	/* Loop that prints out 1 to n threads and their run times */
	for(k = 1; k <= temp; k++)
	{
	    totalThreads += 1;
	    double totalTime;
	    //clock_t runtime, runtimeTwo;

	    printf("%d ", k);
	    threadNum = 0;

	    gettimeofday(&runtime, 0);

	    /* Create threads */
	    for(l = 0; l < k; l++)
	    {
		if(pthread_create(&threads[l], &attr, multiplyArrays, NULL)) exit(1);

	    }
	    for(l = 0; l < k; l++)
	    {
		if(pthread_join(threads[l], NULL)) exit(1);

	    }

	    /* Calculate total runtime using gettimeofday */
	    gettimeofday(&endtime, 0);
	    long seconds = endtime.tv_sec - runtime.tv_sec;
	    long msec = endtime.tv_usec - runtime.tv_usec;
	    totalTime = seconds + msec*1e-6;

	    printf("%lf ", totalTime);

	    /* Check for discrepencies */
	    check = compare(original, c);
	    if(check > 0)
	    {
		printf("Error\n");
	    }
	    else
	    {
		printf("No Errors\n");
	    }
	}
	return 0;

}
