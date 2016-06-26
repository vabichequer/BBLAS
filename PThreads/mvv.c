#ifdef __cplusplus
extern "C" {
#endif
    void pthmvv_(int *threads, int *len, double *ma, double *vb, double *vr);
#ifdef __cplusplus
    }
#endif

#include <stdlib.h>
#include <pthread.h>

typedef struct
{
	double *ma;
	double *vb;
	double *vr;
	double len;
	int start;
	int step;
} mvv_arguments;

void *mvv_worker(void *ptr)
{
	mvv_arguments *mvv_args = (mvv_arguments*)ptr;
	int i, j, alength = mvv_args->len;
	int end = mvv_args->start + mvv_args->step;
	int start = mvv_args->start;

	for (i = start; i < end; i++) 
	{
		for (j = 0; j < alength; j++) 
		{
			*(mvv_args->vr + i) = *(mvv_args->ma + (alength*i) + j) * *(mvv_args->vb + j);
		}
	}
	pthread_exit(NULL);
}

void pthmvv_(int *threads, int *len, double *ma, double *vb, double *vr)
{
	mvv_arguments *args;
	int i, start = 0, *step = malloc(*threads*sizeof(int));
	void *returns;
	double result;
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
		args = (mvv_arguments*)malloc(sizeof(mvv_arguments));
		args->ma = ma;
		args->vb = vb;
		args->vr = vr;
		args->len = *len;
		args->start = start;
		args->step = *(step + i);

		pthread_create(&(workers[i]), NULL, &mvv_worker, (void *)args);

		start += args->step;
	}

	for (i = 0; i < *threads; i++)
	{
		pthread_join(workers[i], &returns);
	}
}