#ifdef __cplusplus
extern "C" {
#endif
	bool ompdls_(int *num_threads, int *N, double *a, double *b, double *x);
#ifdef __cplusplus
}
#endif

#include <stdio.h>
#include <math.h>
#include <iostream> 
#include <stdlib.h>
#include <fstream>
#include <sys/time.h>
#include <omp.h>

using namespace std;

bool ompdls_(int *num_threads, int *N, double *a, double *b, double *x)
{
	int p = 0, i, j, k, SIZE = *N;
	double *A = (double *)malloc((SIZE * (SIZE + 1)) * sizeof(double)), U, sumax;
	bool success = true;
	
	omp_set_num_threads(*num_threads);

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