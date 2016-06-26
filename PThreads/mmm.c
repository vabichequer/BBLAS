#ifdef __cplusplus
extern "C" {
#endif
	void pthmmm_(int *threads, int *len, double *a, double *b, double *c);
#ifdef __cplusplus
    }
#endif

#include <stdlib.h>
#include <pthread.h>

// Define a "global" structure here 
typedef struct {
	int id;
	int veclen;
	int rowstart;
	int stride;
	double *A;
	double *B;
	double *C;
} matrix_worker;

// This is the actual thread worker 
void mmmp(matrix_worker *work_orderp) {

	double *MA = work_orderp->A;
	double *MB = work_orderp->B;
	double *MC = work_orderp->C;

	int veclen = work_orderp->veclen;
	int rowstart = work_orderp->rowstart;
	int rowstop = rowstart + work_orderp->stride;

	int i, j, k;
	int strip, mod;
#ifdef STRIP8
	strip = 8;
	mod = veclen % strip;

	for (i = rowstart; i<rowstop; i++) {
		for (j = 0; j<veclen; j++) {
			*(MC + (i*veclen + j)) = 0.0;
			for (k = 0; k<mod; k++) {
				*(MC + (i*veclen + j)) += *(MA + (i*veclen + k)) * *(MB + (k*veclen + j));
			}
			for (k = mod; k<veclen; k += strip) {
				*(MC + (i*veclen + j)) += *(MA + (i*veclen + k)) * *(MB + (k   *veclen + j))
					+ *(MA + (i*veclen + k + 1)) * *(MB + ((k + 1)*veclen + j))
					+ *(MA + (i*veclen + k + 2)) * *(MB + ((k + 2)*veclen + j))
					+ *(MA + (i*veclen + k + 3)) * *(MB + ((k + 3)*veclen + j));
				+*(MA + (i*veclen + k + 4)) * *(MB + ((k + 4)*veclen + j));
				+*(MA + (i*veclen + k + 5)) * *(MB + ((k + 5)*veclen + j));
				+*(MA + (i*veclen + k + 6)) * *(MB + ((k + 6)*veclen + j));
				+*(MA + (i*veclen + k + 7)) * *(MB + ((k + 7)*veclen + j));
			}
		}
	}

#else

	strip = 4;
	mod = veclen % strip;

	for (i = rowstart; i<rowstop; i++) {
		for (j = 0; j<veclen; j++) {
			*(MC + (i*veclen + j)) = 0.0;
			for (k = 0; k<mod; k++) {
				*(MC + (i*veclen + j)) += *(MA + (i*veclen + k)) * *(MB + (k*veclen + j));
			}
			for (k = mod; k<veclen; k += strip) {
				*(MC + (i*veclen + j)) += *(MA + (i*veclen + k)) * *(MB + (k   *veclen + j))
					+ *(MA + (i*veclen + k + 1)) * *(MB + ((k + 1)*veclen + j))
					+ *(MA + (i*veclen + k + 2)) * *(MB + ((k + 2)*veclen + j))
					+ *(MA + (i*veclen + k + 3)) * *(MB + ((k + 3)*veclen + j));
			}
		}
	}
#endif
}

void pthmmm_(int *threads, int *len, double *a, double *b, double *c) {

	// This is the matrix multiply code that is called to orchestrate the thread workers
	// void mmm_( int *len,  double *a, double *b, double *c ){

	int i;
	int start = 0;
	pthread_t task_id[*threads];
	int *steps = malloc(*threads*sizeof(int));
	matrix_worker *work_orderp;

	// Divide the code up equally amongst all of the threads
	if (*len % *threads == 0) {
		for (i = 0; i<*threads; i++) *(steps + i) = *len / *threads;
	}
	else {
		for (i = 0; i<*threads; i++) *(steps + i) = *len / *threads;
		for (i = 0; i<*len % *threads; i++) *(steps + i) = *(steps + i) + 1;
	}

	start = 0;
	for (i = 0; i<*threads; i++) {

		work_orderp = (matrix_worker *)malloc(sizeof(matrix_worker));
		work_orderp->id = i;
		work_orderp->veclen = *len;
		work_orderp->rowstart = start;
		work_orderp->stride = *(steps + i);
		work_orderp->A = a;
		work_orderp->B = b;
		work_orderp->C = c;

		pthread_create(&(task_id[i]), NULL, (void *)mmmp, (void *)work_orderp);

		start += work_orderp->stride;
	}

	for (i = 0; i<*threads; i++) pthread_join(task_id[i], NULL);


}