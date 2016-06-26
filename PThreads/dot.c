#ifdef __cplusplus
extern "C" {
#endif
	double pthdot_(int *threads, double *a, double *b, int *len);
#ifdef __cplusplus
}
#endif

#include <stdlib.h>
#include <pthread.h>

typedef struct
{
	int id;
	double *a;
	double *b;
	int start;
	int step;
	double *result;
} dot_arguments;

void *dot_worker(void *ptr)
{
	dot_arguments *dot_args = (dot_arguments *)ptr;
	double temp = 0.0;
	double start = dot_args->start;
	double end = dot_args->start + dot_args->step;
	double *a = dot_args->a, *b = dot_args->b;
	int i;

	for (i = start; i < end; i++)
	{
		temp += *(a + i) * *(b + i);
	}

	*(dot_args->result + dot_args->id) = temp;
}


double pthdot_(int *threads, double *a, double *b, int *len)
{
	dot_arguments *dargs;
	int i, start = 0, *step = (int *)malloc(*threads*sizeof(int));
	double result[*threads], res;
	pthread_t workers[*threads];

	// Divide the code up equally amongst all of the threads
	if (*len % *threads == 0) {
		for (i = 0; i<*threads; i++) *(step + i) = *len / *threads;
	}
	else {
		for (i = 0; i<*threads; i++) *(step + i) = *len / *threads;
		for (i = 0; i<*len % *threads; i++) *(step + i) = *(step + i) + 1;
	}

	for (i = 0; i < *threads; i++)
	{
		dargs = (dot_arguments*)malloc(sizeof(dot_arguments));
		dargs->id = i;
		dargs->a = a;
		dargs->b = b;
		dargs->start = start;
		dargs->step = *(step + i);
		dargs->result = result;

		pthread_create(&(workers[i]), NULL, &dot_worker, (void *)dargs);

		start += dargs->step;
	}

	for (i = 0; i < *threads; i++)
	{
		pthread_join(workers[i], NULL);
		res += result[i];
	}

	return res;
}