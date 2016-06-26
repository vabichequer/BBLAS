#ifdef __cplusplus
extern "C" {
#endif
	bool ils_(int num_threads, int N, double *a, double *b, double *x, int len);
#ifdef __cplusplus
}
#endif

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stddef.h> /* defines NULL */
#include <sys/time.h>
#include <omp.h>
#include <pthread.h>

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

void *first(void *ptr)
{
	pthreads_args *args = (pthreads_args*)ptr;
	double *a = args->a;
	double *b = args->b;
	double *x = args->x;
	double *xO = args->xO;
	double sum;
	int len = args->len, start = args->start, step = args->step;

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

void *second(void *ptr)
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

bool ils_(int threads, int N, double *a, double *b, double *x, int len)
{
	pthreads_args *args;
	pthread_t task_id[threads];
	bool overtol = true;
	int k = 1, step = malloc(*threads*sizeof(int)), start = 0, SIZE = len;
	double distance, accum, accumr[threads], TOL = 1.0e-15;
	double *xO = (double *)malloc(SIZE * sizeof(double));

	cout << "Using PTHREADS" << endl;

	// Divide the code up equally amongst all of the threads
	if (SIZE % threads == 0) {
		for (i = 0; i<threads; i++) *(step + i) = SIZE / threads;
	}
	else {
		for (i = 0; i<threads; i++) *(step + i) = SIZE / threads;
		for (i = 0; i<SIZE % threads; i++) *(step + i) = *(step + i) + 1;
	}

	while (k <= N && overtol)
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
			args->start = start;
			args->step = *(step + j);

			pthread_create(&(task_id[j]), NULL, &first, (void *)args);

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
			args->accumr = accumr;
			args->start = start;
			args->step = *(step + i);

			pthread_create(&(task_id[i]), NULL, &second, (void *)args);

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
			cout << "The algorithm took " << k << " iterations to get these results." << endl;
			overtol = false;
		}

		k++;
	}

	return overtol;
}
#elif _OPENMP
bool ils_(int threads, int N, double *a, double *b, double *x, int len)
{
	int k = 1, i = 0, j = 0, SIZE = len;
	double sum = 0;
	double xO[SIZE] = { 0, 0, 0 }, TOL = 1.0e-15;
	bool overtol = true;
	double distance;

	omp_set_num_threads(threads);

	for (k = 1; k <= N; k++)
	{
#pragma omp parallel for shared(i) private(sum) 
		//cout << "K: " << k << endl << "TID: " << omp_get_thread_num() << endl << "Number of threads: " << omp_get_num_threads() << endl;
		for (i = 0; i < SIZE; i++)
		{
			sum = *(b + i);
#pragma omp parallel for shared(j) reduction(- : sum)
			for (int j = 0; j < SIZE; j++) if (i != j) sum -= *(a + i * SIZE + j) * *(xO + j);
			*(x + i) = sum / *(a + i * SIZE + i);
		}

		double accum = 0;	

#pragma omp parallel for shared(i) reduction(+ : accum)
		for (i = 0; i < SIZE; i++)
		{
			accum += (*(x + i) - *(xO + i)) * (*(x + i) - *(xO + i));
		}

		distance = sqrt(accum);
			
		if (distance < TOL)
		{
			cout << "Using OPENMP" << endl;
			cout << "The algorithm took " << k << " iterations to get these results." << endl;			
			overtol = false;
			k = N + 1;
		}

#pragma omp parallel for shared(xO, i)
		for (i = 0; i < SIZE; i++) *(xO + i) = *(x + i);
	}
	return overtol;
}

#else
bool ils_(int threads, int N, double *a, double *b, double *x, int len)
{
	int k = 1, i, SIZE = len;
	double sum = 0, distance;
	double *xO = (double *)malloc(SIZE * sizeof(double)), TOL = 1.0e-15;
	bool overtol;

	cout << "Serial code" << endl;
	for (k = 1; k <= N; k++)
	{
		//cout << "K: " << k << endl << "TID: " << omp_get_thread_num() << endl << "Number of threads: " << omp_get_num_threads() << endl;
		for (i = 0; i < SIZE; i++)
		{
			sum = *(b + i);
			for (int j = 0; j < SIZE; j++) if (i != j) sum -= *(a + i * SIZE + j)  * *(xO + j);
			*(x + i) = sum / *(a + i * SIZE + i);
			//cout << "Resultado: " << *(x + i) << " " << sum << " " << *(a + i * SIZE + i) << endl;
		}

		double accum = 0;
		for (i = 0; i < SIZE; i++)
		{
			accum += (*(x + i) - *(xO + i)) * (*(x + i) - *(xO + i));
		}

		//cout << "Soma: " << accum << endl;
		distance = sqrt(accum);

		if (distance < TOL)
		{
			cout << "The algorithm took " << k << " iterations to get these results." << endl;
			overtol = false;
			k = N + 1;
		}

		for (i = 0; i < SIZE; i++) *(xO + i) = *(x + i);
	}
	return overtol;
}
#endif

/*int main()
{
	int N, pos = 0, lines = 0, SIZE = 4;
	double *a = (double *)malloc((SIZE * SIZE) * sizeof(double)), *b = (double *)malloc(SIZE * sizeof(double)), *x = (double *)malloc(SIZE * sizeof(double)), TOL;
	string line, number;

	for (int i = 0; i < SIZE; i++)	x[i] = 0;
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
				TOL = atof(number.c_str());
				line = line.substr(pos + 1);
				pos = line.find(" ");
				number = line.substr(0, pos);
				N = atof(number.c_str());
				i = SIZE - 1;
			}
			else *(a + SIZE * lines + i) = atof(number.c_str());
			line = line.substr(pos + 1);
		}
		lines++;
	}

	cout << "You can use, at most, " << omp_get_max_threads() << " threads." << endl;

	double cpu, wall;
	cpu = cputime_();
	wall = walltime_();

	bool fail = ils_(1, N, (double *)a, b, x, TOL, SIZE);

	cout << "Wall: " << walltime_() - wall << endl << "CPU: " << cputime_() - cpu << endl;

	if (!fail)
	{
		cout << "The procedure was successsful." << endl;
		for (int i = 0; i < SIZE; i++)
		{
			cout << "x[" << i << "] = " << x[i] << endl;
		}
	}
	else cout << "Maximum number of iterations reached. The procedure was not successful." << endl;
}*/
