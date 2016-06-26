#ifdef __cplusplus
extern "C" {
#endif
	void vvm_(int *threads, int *len, double *va, double *vb, double *mr);
#ifdef __cplusplus
}
#endif

#ifdef OPENMP
void  vvm_(int *threads, int *len, double *va, double *vb, double *mr) {

	int i, j, alength = *len;

	omp_set_num_threads(*threads);

#pragma omp prallel shared(alength, va, vb, mr) private(i, j) 
	{
#pragma omp for
		for (i = 0; i < alength; i++) {
			for (j = 0; j < alength; j++) {
				*(mr + (alength*i) + j) = *(va + i) * *(vb + j);
			}
		}
	}

}
#elif PTHREADS
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
	int start = vvm_args->start;

	for (i = start; i < end; i++) {
		for (j = start; j < end; j++) {
			*(vvm_args->mr + (alength*i) + j) = *(vvm_args->va + i) * *(vvm_args->vb + j);
		}
	}
	pthread_exit(NULL);
}

void vvm_(int *threads, int *len, double *va, double *vb, double *mr)
{
	vvm_arguments *args;
	int i, start = 0, *step = malloc(*threads*sizeof(int));
	void *returns;
	double result, temp;
	pthread_t workers[*threads];

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
#else

// Computes matrix-vector multiplications 

void  vvm_(int *threads, int *len, double *va, double *vb, double *mr) {

	int i, j, alength = *len;

	for (i = 0; i<alength; i++) {
		for (j = 0; j<alength; j++) {
			*(mr + (alength*i) + j) = *(va + i) * *(vb + j);
		}
	}

}
#endif