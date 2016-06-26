#include <stdio.h>
#include <math.h>
#include <iostream> 
#include <stdlib.h>
#include <fstream>
#include <sys/time.h>
#include <pthread.h>

using namespace std;

pthread_mutex_t lock;

typedef struct
{
	int id;
	int i;
	int SIZE;
	double *a;
	double *b;
	double *A;
	double *x;
	int start;
	int step, step2;
} pth_arguments;

void *first(void *ptr)
{
	pth_arguments *args = (pth_arguments *)ptr;
	int start = args->start, step2 = args->step2, p = 0, SIZE = args->SIZE;
	double *A = args->A, *a = args->a, *b = args->b, U;

	pthread_mutex_lock(&lock);
	cout << "First: " << start << " " << step2 << endl;
	pthread_mutex_unlock(&lock);

	for (int i = start; i < (start + step2); i++)
	{
		p = i;
		for (int j = i + 1; j < SIZE; j++)
		{
			if (fabs(*(A + j * (SIZE + 1) + i)) > fabs(*(A + i * (SIZE + 1) + i)))
			{
				for (int k = 0; k < SIZE; k++)
				{
					U = *(A + i * (SIZE + 1) + k);
					*(A + i * (SIZE + 1) + k) = *(A + j * (SIZE + 1) + k);
					*(A + j * (SIZE + 1) + k) = U;
				}
			}
		}

		while ((*(A + p * (SIZE + 1) + i) == 0) && (p <= SIZE - 1)) p++;

		if (p != i)
		{
			for (int k = 0; k < SIZE; k++)
			{
				U = *(A + i * (SIZE + 1) + k);
				*(A + i * (SIZE + 1) + k) = *(A + p * (SIZE + 1) + k);
				*(A + p * (SIZE + 1) + k) = U;
			}
		}
	}
}

void *second(void *ptr)
{
	pth_arguments *args = (pth_arguments *)ptr;

	double *A = args->A;
	int start = args->start, step = args->step, SIZE = args->SIZE, i = args->i;

	for (int j = start; j < start + step; j++)
	{
		double m = *(A + (SIZE + 1) * j + i) / *(A + (SIZE + 1) * i + i);
		for (int k = i + 1; k <= SIZE; k++)
		{
			*(A + (SIZE + 1) * j + k) -= m * *(A + (SIZE + 1) * i + k);
		}
	}
}

void *third(void *ptr)
{
	pth_arguments *args = (pth_arguments *)ptr;
	double *A = args->A, *x = args->x, sumax;
	int start = args->start, step = args->step, SIZE = args->SIZE;

	for (int i = start; i >= 0; i--)
	{
		sumax = 0;
		for (int j = i + 1; j < SIZE; j++)
		{
			sumax = sumax + *(A + i * (SIZE + 1) + j) * *(x + j);
		}
		*(x + i) = (*(A + i * (SIZE + 1) + SIZE) - sumax) / *(A + i * (SIZE + 1) + i);
	}
}

void *copy(void *ptr)
{
	pth_arguments *args = (pth_arguments *)ptr;
	double *A = args->A, *a = args->a, *b = args->b;
	int SIZE = args->SIZE, start = args->start, step = args->step;

	for (int i = start; i < (start + step); i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			*(A + i * (SIZE + 1) + j) = *(a + i * SIZE + j);
		}
		*(A + i * (SIZE + 1) + SIZE) = *(b + i);
	}
}

bool pthdls_(int *num_threads, int *N, double *a, double *b, double *x)
{
	int p = 0, start = 0, SIZE = *N, threads = *num_threads;
	int *step = (int *)malloc(threads*sizeof(int));
	double *A = (double *)malloc((SIZE*(SIZE + 1)) * sizeof(double));
	pthread_t task_id[threads];
	pth_arguments *args;

	cout << "Using PTHREADS" << endl;

	pthread_mutex_init(&lock, NULL);

	// Divide the code up equally amongst all of the threads
	if (SIZE % threads == 0) {
		for (int i = 0; i < threads; i++) *(step + i) = SIZE / threads;
	}
	else {
		for (int i = 0; i < threads; i++) *(step + i) = SIZE / threads;
		for (int i = 0; i < SIZE % threads; i++) *(step + i) = *(step + i) + 1;
	}

	cout << "1" << endl;

	for (int i = 0; i < threads; i++)
	{
		args = (pth_arguments *)malloc(sizeof(pth_arguments));
		args->SIZE = SIZE;
		args->a = a;
		args->b = b;
		args->A = A;
		args->start = start;
		args->step = *(step + i);

		pthread_create(&(task_id[i]), NULL, &copy, (void *)args);

		start += args->step;
	}

	for (int i = 0; i < threads; i++) pthread_join(task_id[i], NULL);

	cout << "2" << endl; //segfault

	start = 0;
	for (int i = 0; i < threads; i++)
	{
		args = (pth_arguments *)malloc(sizeof(pth_arguments));
		args->id = i;
		args->SIZE = SIZE;
		args->a = a;
		args->b = b;
		args->A = A;
		args->start = start;
		if ((start + *(step + i)) == SIZE && *(step + i) != 0) args->step2 = *(step + i) - 1;
		else args->step2 = *(step + i);

		pthread_create(&(task_id[i]), NULL, &first, (void *)args);

		start += args->step2;
	}

	for (int i = 0; i < threads; i++) pthread_join(task_id[i], NULL);

	cout << "3" << endl;

	for (int i = 0; i < SIZE - 1; i++)
	{
		start = i + 1;
		for (int k = 0; k < threads; k++) *(step + k) = (SIZE - start) / threads;
		for (int k = 0; k < (SIZE - start) % threads; k++) *(step + k) = *(step + k) + 1;

		for (int j = 0; j < threads; j++)
		{
			args = (pth_arguments *)malloc(sizeof(pth_arguments));
			args->A = A;
			args->id = j;
			args->SIZE = SIZE;
			args->start = start;
			args->step = *(step + j);
			args->i = i;

			pthread_create(&(task_id[j]), NULL, &second, (void *)args);

			start += args->step;
		}

		for (int i = 0; i < threads; i++) pthread_join(task_id[i], NULL);
	}

	*(x + SIZE - 1) = *(A + (SIZE + 1) * (SIZE - 1) + (SIZE)) / *(A + (SIZE + 1) * (SIZE - 1) + (SIZE - 1));

	cout << "4" << endl;

	if (SIZE % threads == 0) {
		for (int i = 0; i < threads; i++) *(step + i) = SIZE / threads;
	}
	else {
		for (int i = 0; i < threads; i++) *(step + i) = SIZE / threads;
		for (int i = 0; i < SIZE % threads; i++) *(step + i) = *(step + i) + 1;
	}


	start = SIZE - 1;
	for (int i = threads - 1; i >= 0; i--)
	{
		args = (pth_arguments *)malloc(sizeof(pth_arguments));
		args->id = i;
		args->SIZE = SIZE;
		args->x = x;
		args->A = A;
		args->start = start;
		args->step = *(step + i);

		pthread_create(&(task_id[i]), NULL, &third, (void *)args);

		start -= args->step;
	}

	for (int i = 0; i < threads; i++) pthread_join(task_id[i], NULL);

	cout << "5" << endl;

	pthread_mutex_destroy(&lock);

	return true;
}

int main()
{
	double a[3][3] = { {30, 5, 20}, {20, 30, 5}, {5, 20, 30} };
	double b[3] = { 9, 0, -1 };
	double x[3] = { 0, 0, 0 };
	int threads = 8, len = 3;

	pthdls_(&threads, &len, (double *)a, b, x);
	
	for (int i = 0; i < 3; i++) cout << x[i] << endl;

	return 0;
}