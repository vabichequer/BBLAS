#ifdef __cplusplus
extern "C" {
#endif
	void pthvvm_(int *threads, int *len, double *va, double *vb, double *mr);
#ifdef __cplusplus
}
#endif

#include <stdlib.h>
#include <pthread.h>

typedef struct
{
	double *va;
	double *vb;
	double *mr;
	double len;
	int start;
	int step;
} vvm_arguments;

void *vvm_worker(void *ptr)
{
	vvm_arguments *vvm_args = (vvm_arguments*)ptr;
	int i, j, alength = vvm_args->len;
	int end = vvm_args->start + vvm_args->step;
	int start = vvm_args->start, size = vvm_args->len;
	double *va = vvm_args->va, *vb = vvm_args->vb, *mr = vvm_args->mr;

	for (i = start; i < end; i++) {
		for (j = 0; j < size; j++) {
			*(mr + (alength*i) + j) = *(va + i) * *(vb + j); 
		}
	}
	pthread_exit(NULL);
}

void pthvvm_(int *threads, int *len, double *va, double *vb, double *mr)
{
	vvm_arguments *args;
	int i, start = 0, *step = malloc(*threads*sizeof(int));
	void *returns;
	double result, temp;
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
		args = (vvm_arguments*)malloc(sizeof(vvm_arguments));
		args->va = va;
		args->vb = vb;
		args->mr = mr;
		args->len = *len;
		args->start = start;
		args->step = *(step + i);

		pthread_create(&(workers[i]), NULL, &vvm_worker, (void *)args);

		start += args->step;
	}

	for (i = 0; i < *threads; i++)
	{
		pthread_join(workers[i], &returns);
	}
}