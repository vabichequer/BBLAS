#ifdef __cplusplus
extern "C" {
#endif
	double dot_(int *threads, double *a, double *b, int *len);
#ifdef __cplusplus
}
#endif

#ifdef OPENMP
double dot_(int *threads, double *a, double *b, int *len)
{
	double temp = 0.0;
	int i;
	int length = *len;
	
	omp_set_num_threads(*threads);

#pragma omp parallel shared(a, b, length) private(i) reduction(+ : temp)
	{
#pragma omp for
		for (i = 0; i < length; i++) temp += a[i] * b[i];
	}

	return temp;
}

#elif PTHREADS
#include <stdlib.h>
#include <pthread.h>

pthread_mutex_t lock;

typedef struct
{
	double *a;
	double *b;
	int start;
	int step;
	double result;
} dot_arguments;

void *dot_worker(void *ptr)
{
	dot_arguments *dot_args = (dot_arguments *)ptr;
	double temp = 0.0;
	double start = dot_args->start;
	double end = dot_args->start + dot_args->step;
	int i;

	for (i = start; i < end; i++)
	{
		temp += *(dot_args->a + i) * *(dot_args->b + i);
	}

	//mutex
	pthread_mutex_lock(&lock);
	dot_args->result = temp;
	pthread_mutex_unlock(&lock);

	pthread_exit(NULL);
}

double dot_(int *threads, double *a, double *b, int *len)
{
	dot_arguments *dargs;
	int i, start = 0, *step = malloc(*threads*sizeof(int));
	void *returns;
	double result;
	pthread_t workers[*threads];
	pthread_mutex_init(&lock, NULL);

	// Divide the code up equally amongst all of the threads
	if (*len % *threads == 0) {
		for (i = 0; i<*threads; i++) *(steps + i) = *len / *threads;
	}
	else {
		for (i = 0; i<*threads; i++) *(steps + i) = *len / *threads;
		for (i = 0; i<*len % *threads; i++) *(steps + i) = *(steps + i) + 1;
	}

	for (i = 0; i < *threads; i++)
	{
		dargs = (dot_arguments*)malloc(sizeof(dot_arguments));
		dargs->a = a;
		dargs->b = b;
		dargs->start = start;
		dargs->step = *(step + i);

		pthread_create(&(workers[i]), NULL, &dot_worker, (void *)dargs);

		start += dargs->step;
	}

	for (i = 0; i < *threads; i++)
	{
		pthread_join(workers[i], &returns);
	}

	pthread_mutex_destroy(&lock);

	result = dargs->result;
	return result;
}
#else
double dot_(int *threads, double *a, double *b, int *len)
{
	double temp = 0.0;
	int i;

	int length = *len;

	for (i = 0; i < length; i++) temp += a[i] * b[i];
	return temp;
}
#endif