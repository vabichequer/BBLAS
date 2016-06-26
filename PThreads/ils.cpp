#ifdef __cplusplus
extern "C" {
#endif
	void pthils_(int *num_threads, int *len, double *a, double *b, double *x);
#ifdef __cplusplus
}
#endif

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <string>
#include <stdlib.h>
#include <stddef.h> /* defines NULL */
#include <sys/time.h>
#include <pthread.h>

using namespace std;

typedef struct
{
	int id;
	double *a;
	double *b;
	double *x;
	double *xO;
	double *accumr;
	int len;
	int start;
	int step;
} pthreads_args;

void *first_ils(void *ptr)
{
	pthreads_args *args = (pthreads_args*)ptr;
	double *a = args->a;
	double *b = args->b;
	double *x = args->x;
	double *xO = args->xO;
	double sum;
	int SIZE = args->len, start = args->start, step = args->step;

	/*
	for (i = 0; i < SIZE; i++)
	{
		sum = *(b + i);
		for (int j = 0; j < SIZE; j++) if (i != j) sum -= *(a + i * SIZE + j)  * *(xO + j);
		*(x + i) = sum / *(a + i * SIZE + i);
		//cout << "Resultado: " << *(x + i) << " " << sum << " " << *(a + i * SIZE + i) << endl;
	}
	*/

	for (int i = start; i < (start + step); i++)
	{
		sum = *(b + i);
		for (int j = 0; j < SIZE; j++)	if (i != j)	sum -= *(a + i * SIZE + j) * *(xO + j);
		*(x + i) = sum / *(a + i * SIZE + i);
	}
}

void *second_ils(void *ptr)
{
	pthreads_args *args = (pthreads_args*)ptr;
	double *a = args->a;
	double *b = args->b;
	double *x = args->x;
	double *xO = args->xO;
	int len = args->len, start = args->start, step = args->step;
	double accum = 0;
	
	//cout << args->id << " " << start << " " << step << endl;

	for (int i = start; i < (start + step); i++)
	{
		accum += (*(x + i) - *(xO + i)) * (*(x + i) - *(xO + i));
	}

	*(args->accumr + args->id) = accum;

	for (int i = start; i < (start + step); i++) *(xO + i) = *(x + i);
}

void pthils_(int *num_threads, int *len, double *a, double *b, double *x)
{
	int threads = *num_threads, k = 1, *step = (int *)malloc(threads*sizeof(int)), start = 0, SIZE = *len;
	double distance, accum, accumr[threads], TOL = 1.0e-15;
	double *xO = (double *)malloc(SIZE * sizeof(double));
	bool overtol = true;
	pthreads_args *args;
	pthread_t task_id[threads];

	// Divide the code up equally amongst all of the threads
	if (SIZE % threads == 0) {
		for (int i = 0; i<threads; i++) *(step + i) = SIZE / threads;
	}
	else {
		for (int i = 0; i<threads; i++) *(step + i) = SIZE / threads;
		for (int i = 0; i<SIZE % threads; i++) *(step + i) = *(step + i) + 1;
	}

	while (overtol)
	{
		start = 0;
		for (int j = 0; j < threads; j++)
		{
			args = (pthreads_args *)malloc(sizeof(pthreads_args));
			args->id = j;
			args->a = a;
			args->b = b;
			args->x = x;
			args->xO = xO;
			args->len = *len;
			args->start = start;
			args->step = *(step + j);

			pthread_create(&(task_id[j]), NULL, &first_ils, (void *)args);

			start += args->step;
		}

		for (int m = 0; m < threads; m++)
		{
			pthread_join(task_id[m], NULL);
		}
		
		start = 0;
		accum = 0;
		for (int i = 0; i < threads; i++)
		{
			args = (pthreads_args *)malloc(sizeof(pthreads_args));
			args->id = i;
			args->a = a;
			args->b = b;
			args->x = x;
			args->xO = xO;
			args->len = *len;
			args->accumr = accumr;
			args->start = start;
			args->step = *(step + i);

			pthread_create(&(task_id[i]), NULL, &second_ils, (void *)args);

			start += args->step;
		}


		for (int i = 0; i < threads; i++)
		{
			pthread_join(task_id[i], NULL);
			accum += accumr[i];
			accumr[i] = 0;
		}

		//cout << "Soma: " << accum << endl;
		distance = sqrt(accum);
		if (distance < TOL)
		{
			//cout << "The algorithm took " << k << " iterations to get these results." << endl;
			overtol = false;
		}

		k++;
	}
}