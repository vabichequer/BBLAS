#ifdef __cplusplus
extern "C" {
#endif
	void ils_(int *num_threads, int *len, double *a, double *b, double *x);
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

using namespace std;

void ils_(int *num_threads, int *len, double *a, double *b, double *x)
{
	int k = 1, i, SIZE = *len, N = *len;
	double sum = 0, distance;
	double *xO = (double *)malloc(SIZE * sizeof(double)), TOL = 1.0e-15;

	while(k <= N)
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

		if (distance < TOL)	break;

		for (i = 0; i < SIZE; i++) *(xO + i) = *(x + i);

		k++;
	}
}
