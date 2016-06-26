extern "C"
{
	void matrixgenerator_(int *dim, double *a, double *b);
}

#include <stdio.h>		
#include <stdlib.h>     
#include <time.h>    
#include <fstream>
#include <iostream>

using namespace std;

void matrixgenerator_(int *dim, double *a, double *b)
{
	int accum = 0, temp, SIZE = *dim;
	ofstream file;

	srand(time(NULL));

	for (int i = 0; i < SIZE; i++)
	{
		for (int j = 0; j < SIZE; j++)
		{
			temp = rand() % 101;
			accum += abs(temp);
			*(a + SIZE * i + j) = temp;
		}
		*(a + SIZE * i + i) = accum;
		*(b + i) = rand() % 101;
		accum = 0;
	}
}