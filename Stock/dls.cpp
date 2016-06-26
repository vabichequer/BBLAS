#ifdef __cplusplus
extern "C" {
#endif
	bool dls_(int *num_threads, int *N, double *a, double *b, double *x);
#ifdef __cplusplus
}
#endif

#include <stdio.h>
#include <math.h>
#include <iostream> 
#include <stdlib.h>
#include <fstream>
#include <sys/time.h>
#include <pthread.h>

using namespace std;
bool dls_(int *num_threads, int *N, double *a, double *b, double *x)
{
	int p = 0, SIZE = *N;
	double *A = (double *)malloc((SIZE * (SIZE + 1)) * sizeof(double)), U, sumax;

	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			*(A + i * (SIZE + 1) + j) = *(a + i * SIZE + j);
		}
	}

	for (int i = 0; i < SIZE; i++)
	{
		*(A + i * (SIZE + 1) + SIZE) = *(b + i);
	}

	for (int i = 0; i < SIZE - 1; i++)
	{
		p = i;
		for (int j = i + 1; j < SIZE; j++)
		{
			if (fabs(*(A + j * (SIZE + 1) + i)) > fabs(*(A + i * (SIZE + 1) + i)))
			{
				for (int k = 0; k < SIZE; k++)
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
			//cout << "No unique solution." << endl;
			return false;
		}
		else
		{
			if (p != i)
			{
				for (int k = 0; k < SIZE; k++)
				{
					U = *(A + i * (SIZE + 1) + k);
					*(A + i * (SIZE + 1) + k) = *(A + p * (SIZE + 1) + k);
					*(A + p * (SIZE + 1) + k) = U;
				}
			}
		}

		for (int j = i + 1; j < SIZE; j++)
		{
			double m = *(A + j * (SIZE + 1) + i) / *(A + i * (SIZE + 1) + i);
			for (int k = i + 1; k <= SIZE; k++)
			{
				*(A + j * (SIZE + 1) + k) = *(A + j * (SIZE + 1) + k) - m**(A + i * (SIZE + 1) + k);
			}
		}
	}

	if (*(A + (SIZE - 1) * (SIZE + 1) + (SIZE - 1)) == 0)
	{
		//cout << "No uniquer solution." << endl;
		return false;
	}

	*(x + SIZE - 1) = *(A + (SIZE - 1) * (SIZE + 1) + SIZE) / *(A + (SIZE - 1) * (SIZE + 1) + (SIZE - 1));
	for (int i = SIZE - 1; i >= 0; i--)
	{
		sumax = 0;
		for (int j = i + 1; j < SIZE; j++)
		{
			sumax = sumax + *(A + i * (SIZE + 1) + j) * *(x + j);
			//cout << sumax << " " << A[i][j] << " " << *(x + j) << endl;
		}
		*(x + i) = (*(A + i * (SIZE + 1) + SIZE) - sumax) / *(A + i * (SIZE + 1) + i);
	}
	return true;
}