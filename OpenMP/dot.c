#ifdef __cplusplus
extern "C" {
#endif
	double ompdot_(int *threads, double *a, double *b, int *len);
#ifdef __cplusplus
}
#endif

double ompdot_(int *threads, double *a, double *b, int *len)
{
	double temp = 0.0;
	int i;
	int length = *len;
	
	omp_set_num_threads(*threads);

#pragma omp parallel shared(a, b, length) private(i) reduction(+ : temp)
	{
#pragma omp for
		for (i = 0; i < length; i++) temp += a[i] * b[i];
	}

	return temp;
}