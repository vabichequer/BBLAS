#ifdef __cplusplus
extern "C" {
#endif
	void ompils_(int *num_threads, int *len, double *a, double *b, double *x);
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
#include <omp.h>

using namespace std;

void ompils_(int *num_threads, int *len, double *a, double *b, double *x)
{
	int k = 1, i = 0, j = 0, SIZE = *len;
	double sum = 0;
	double *xO = (double *)malloc(SIZE * sizeof(double)), TOL = 1.0e-15;
	bool overtol = true;
	double distance;


	omp_set_num_threads(*num_threads);

	while(overtol)
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
			//cout << "The algorithm took " << k << " iterations to get these results." << endl;			
			overtol = false;
		}

#pragma omp parallel for shared(xO, i)
		for (i = 0; i < SIZE; i++) *(xO + i) = *(x + i);
	}
}
