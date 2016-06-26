#ifdef __cplusplus
extern "C" {
#endif
	bool dls_(int num_threads, int N, double *a, double *b, double *x);
#ifdef __cplusplus
}
#endif

#include <stdio.h>
#include <math.h>
#include <iostream> 
#include <stdlib.h>
#include <fstream>
#include <sys/time.h>

double factor = 1.0e-6;

using namespace std;

double cputime_()
{
	return  (double)clock() / (double)CLOCKS_PER_SEC;
}

double walltime_()
{
	struct timeval tp;
	int rtn;
	double seconds;

	rtn = gettimeofday(&tp, NULL);

	seconds = tp.tv_sec + factor * tp.tv_usec;

	return  seconds;
}

#ifdef _POSIX_THREADS
#include <pthread.h>

pthread_mutex_t lock;

typedef struct
{
	int id;
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
	int start = args->start, step = args->step, step2 = args->step2, p = 0, SIZE = args->SIZE;
	double *A = args->A, *a = args->a, *b = args->b, U;

	for (int i = start; i < (start + step); i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			*(A + i * (SIZE + 1) + j) = *(a + i * SIZE + j);
		}
		*(A + i * (SIZE + 1) + SIZE) = *(b + i);
	}
	
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

		if (p == SIZE)
		{
			cout << "No unique solution." << endl;
			return false;
		}
		else
		{
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
}

void *second(void *ptr)
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

void *third(void *ptr)
{
	pth_arguments *args = (pth_arguments *)ptr;

	double *A = args->A;
	int start = args->start, step = args->step2, SIZE = args->SIZE;

	for (int i = start; i < (start + step); i++)
	{
		for (int j = i + 1; j < SIZE; j++)
		{
			double m = *(A + (SIZE + 1) * j + i) / *(A + (SIZE + 1) * i + i);
			for (int k = i + 1; k <= SIZE; k++)
			{
				*(A + (SIZE + 1) * j + k) -= m * *(A + (SIZE + 1) * i + k);
			}
		}
	}
}

bool dls_(int threads, int N, double *a, double *b, double *x)
{
	int p = 0, start = 0, SIZE = N;
	int *step = (int *)malloc(threads*sizeof(int));
	double *A = (double *)malloc((SIZE*(SIZE + 1)) * sizeof(double));
	pthread_t task_id[threads];
	pth_arguments *args;

	cout << "Using PTHREADS" << endl;

	if (pthread_mutex_init(&lock, NULL) != 0)
	{
		printf("\n mutex init failed\n");
		return 1;
	}

	// Divide the code up equally amongst all of the threads
	if (SIZE % threads == 0) {
		for (int i = 0; i<threads; i++) *(step + i) = SIZE / threads;
	}
	else {
		for (int i = 0; i < threads; i++) *(step + i) = SIZE / threads;
		for (int i = 0; i < SIZE % threads; i++) *(step + i) = *(step + i) + 1;
	}

	for (int i = 0; i < threads; i++)
	{
		args = (pth_arguments *)malloc(sizeof(pth_arguments));
		args->id = i;
		args->SIZE = SIZE;
		args->a = a;
		args->b = b;
		args->A = A;
		args->start = start;
		args->step = *(step + i);
		if (i == threads - 1) args->step2 = *(step + i) - 1;
		else args->step2 = *(step + i);

		pthread_create(&(task_id[i]), NULL, &first, (void *)args);

		start += args->step;
	}

	for (int i = 0; i < threads; i++) pthread_join(task_id[i], NULL);

	/**/

	for (int i = 0; i < SIZE - 1; i++)
	{
		for (int j = i + 1; j < SIZE; j++)
		{
			double m = *(A + (SIZE + 1) * j + i) / *(A  + (SIZE + 1) * i + i);
			for (int k = i + 1; k <= SIZE; k++)
			{
				*(A + (SIZE + 1) * j + k) = *(A + (SIZE + 1) * j + k) - m * *(A + (SIZE + 1) * i + k);
			}
		}
	}

	/*
	start = 0;
	for (int i = 0; i < threads; i++)
	{
		args = (pth_arguments *)malloc(sizeof(pth_arguments));
		args->A = A;
		args->id = i;
		args->SIZE = SIZE;
		args->start = start;
		if (i == threads - 1) args->step2 = *(step + i) - 1;
		else args->step2 = *(step + i);

		pthread_create(&(task_id[i]), NULL, &third, (void *)args);

		start += args->step;
	}

	for (int i = 0; i < threads; i++) pthread_join(task_id[i], NULL);

	/*

	for (int i = 0; i < SIZE - 1; i++)
	{
		for (int j = i + 1; j < SIZE; j++)
		{
			for (int k = i + 1; k <= SIZE; k++)
			{
				cout << i << " " << j << " " << k << " " << *(A + (SIZE + 1) * j + k) << endl;
			}
		}
	}

	/**/

	if (*(A + (SIZE + 1) * (SIZE - 1) + (SIZE - 1)) == 0)
	{
		cout << "No uniquer solution." << endl;
		return false;
	}

	*(x + SIZE - 1) = *(A + (SIZE + 1) * (SIZE - 1) + (SIZE)) / *(A + (SIZE + 1) * (SIZE - 1) + (SIZE - 1));	

	start = SIZE - 1;
	for (int i = 0; i < threads; i++)
	{
		args = (pth_arguments *)malloc(sizeof(pth_arguments));
		args->id = i;
		args->SIZE = SIZE;
		args->x = x;
		args->A = A;
		args->start = start;
		args->step = *(step + (threads - 1 - i));

		pthread_create(&(task_id[i]), NULL, &second, (void *)args);

		if (i == threads - 1) args->step2 = *(step + i) - 1;
		else args->step2 = *(step + i);

		pthread_create(&(task_id[i]), NULL, &third, (void *)args);

		start -= args->step;
	}

	for (int i = 0; i < threads; i++)
	{
		pthread_join(task_id[i], NULL);
	}

	return true;
}
#elif _OPENMP
#include <omp.h>
bool dls_(int threads, int N, double *a, double *b, double *x)
{
	int p = 0, i, j, k, SIZE = N;
	double *A = (double *)malloc((SIZE * (SIZE + 1)) * sizeof(double)), U, sumax;
	bool success = true;

	cout << "Using OPENMP" << endl;
	
	omp_set_num_threads(threads);

#pragma omp parallel for shared(i)
	for (i = 0; i < SIZE; i++)
	{
#pragma omp parallel for shared(A, j)
		for (j = 0; j < SIZE; j++)
		{
			*(A + i * (SIZE + 1) + j) = *(a + i * SIZE + j);
		}
	}

#pragma omp parallel for shared(i, A)
	for (i = 0; i < SIZE; i++)
	{
		*(A + i * (SIZE + 1) + SIZE) = *(b + i);
	}

//#pragma omp parallel for shared(i) private(p)
	for (i = 0; i < SIZE - 1; i++)
	{
		p = i;
#pragma omp parallel for shared(j, A) private(U)
		for (j = i + 1; j < SIZE; j++)
		{
			if (fabs(*(A + j * (SIZE + 1) + i)) > fabs(*(A + i * (SIZE + 1) + i)))
			{
#pragma omp parallel for shared(k, A) private(U)
				for (k = 0; k < SIZE; k++)
				{
					U = *(A + i * (SIZE + 1) + k);
					*(A + i * (SIZE + 1) + k) = *(A + j * (SIZE + 1) + k);
					*(A + j * (SIZE + 1) + k) = U;
				}
			}
		}

		while ((*(A + p * (SIZE + 1) + i) == 0) && (p <= SIZE - 1))
		{
			p++;
		}

		if (p == SIZE)
		{
			cout << "No unique solution." << endl;
			i = SIZE - 1;
			success = false;
		}
		else
		{
			if (p != i)
			{
#pragma omp parallel for shared(k, A) private(U)
				for (k = 0; k < SIZE; k++)
				{
					U = *(A + i * (SIZE + 1) + k);
					*(A + i * (SIZE + 1) + k) = *(A + p * (SIZE + 1) + k);
					*(A + p * (SIZE + 1) + k) = U;
				}
			}
		}

#pragma omp parallel for shared(j)
		for (j = i + 1; j < SIZE; j++)
		{
			double m = *(A + j * (SIZE + 1) + i) / *(A + i * (SIZE + 1) + i);
#pragma omp parallel for shared(k, A)
			for (k = i + 1; k <= SIZE; k++)
			{
				*(A + j * (SIZE + 1) + k) = *(A + j * (SIZE + 1) + k) - m**(A + i * (SIZE + 1) + k);
			}
		}
	}

	if (*(A + (SIZE - 1) * (SIZE + 1) + (SIZE - 1)) == 0)
	{
		cout << "No uniquer solution." << endl;
		return false;
	}

	*(x + SIZE - 1) = *(A + (SIZE - 1) * (SIZE + 1) + SIZE) / *(A + (SIZE - 1) * (SIZE + 1) + (SIZE - 1));

	for (i = SIZE - 1; i >= 0; i--)
	{
		sumax = 0;
#pragma omp parallel for shared(j) reduction(+ : sumax)
		for (j = i + 1; j < SIZE; j++)
		{
			sumax += *(A + i * (SIZE + 1) + j) * *(x + j);
			//cout << sumax << " " << A[i][j] << " " << *(x + j) << endl;
		}
		*(x + i) = (*(A + i * (SIZE + 1) + SIZE) - sumax) / *(A + i * (SIZE + 1) + i);
	}

	return success;
}
#else
bool dls_(int threads, int N, double *a, double *b, double *x)
{
	int p = 0, SIZE = N;
	double *A = (double *)malloc((SIZE * (SIZE + 1)) * sizeof(double)), U, sumax;

	cout << "Using stock code" << endl;

	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			*(A + i * (SIZE + 1) + j) = *(a + i * SIZE + j);
		}
	}

	for (int i = 0; i < SIZE; i++)
	{
		*(A + i * (SIZE + 1) + SIZE) = *(b + i);
	}

	for (int i = 0; i < SIZE - 1; i++)
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

		while ((*(A + p * (SIZE + 1) + i) == 0) && (p <= SIZE - 1))
		{
			p++;
		}

		if (p == SIZE)
		{
			cout << "No unique solution." << endl;
			return false;
		}
		else
		{
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

		for (int j = i + 1; j < SIZE; j++)
		{
			double m = *(A + j * (SIZE + 1) + i) / *(A + i * (SIZE + 1) + i);
			for (int k = i + 1; k <= SIZE; k++)
			{
				*(A + j * (SIZE + 1) + k) = *(A + j * (SIZE + 1) + k) - m**(A + i * (SIZE + 1) + k);
			}
		}
	}

	if (*(A + (SIZE - 1) * (SIZE + 1) + (SIZE - 1)) == 0)
	{
		cout << "No uniquer solution." << endl;
		return false;
	}

	*(x + SIZE - 1) = *(A + (SIZE - 1) * (SIZE + 1) + SIZE) / *(A + (SIZE - 1) * (SIZE + 1) + (SIZE - 1));
	for (int i = SIZE - 1; i >= 0; i--)
	{
		sumax = 0;
		for (int j = i + 1; j < SIZE; j++)
		{
			sumax = sumax + *(A + i * (SIZE + 1) + j) * *(x + j);
			//cout << sumax << " " << A[i][j] << " " << *(x + j) << endl;
		}
		*(x + i) = (*(A + i * (SIZE + 1) + SIZE) - sumax) / *(A + i * (SIZE + 1) + i);
	}
	return true;
}
#endif

/*
int main()
{
	int SIZE = 4;
	double *a = (double *)malloc((SIZE * SIZE)*sizeof(double));
	double *b = (double *)malloc(SIZE * sizeof(double)), *x = (double *)malloc(SIZE * sizeof(double));

	if (a == NULL || b == NULL || x == NULL)
	{
		cout << "There was a problem allocating memory. Terminating the program..." << endl;
		free(a);
		free(b);
		free(x);
		return 1;
	}

	string line, number;
	int pos, lines = 0;
	ifstream datafile("jacobi.dat", ifstream::binary);

	while (!datafile.eof())
	{
		getline(datafile, line);
		for (int i = 0; i < SIZE; i++)
		{
			pos = line.find(" ");
			number = line.substr(0, pos);
			if (lines == SIZE) *(b + i) = atof(number.c_str());
			else if (lines == SIZE + 1)
			{
				// adapted from ils
			}
			else *(a + SIZE * lines + i) = atof(number.c_str());
			line = line.substr(pos + 1);
		}
		lines++;
	}

	double cpu, wall;
	cpu = cputime_();
	wall = walltime_();

	bool result = dls_(2, SIZE, a, b, x);

	cout << "Wall: " << walltime_() - wall << endl << "CPU: " << cputime_() - cpu << endl;

	if (result) for (int i = 0; i < SIZE; i++) cout << "x[" << i << "]: " << *(x + i) << endl;
	free(a);
	free(b);
	free(x);
}
*/