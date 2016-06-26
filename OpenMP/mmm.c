#ifdef __cplusplusextern "C" {#endif    void ompmmm_(int *threads, int *len, double *a, double *b, double *c);#ifdef __cplusplus    }#endifvoid ompmmm_(int *threads, int *len, double *a, double *b, double *c) {	int i, j, k;	int veclen = *len;	double sum;	omp_set_num_threads(*threads);#pragma omp parallel shared(veclen,a,b,c) private(i,j,k) reduction(+ : sum)  	{#pragma omp for		for (i = 0; i<veclen; i++) {			for (j = 0; j<veclen; j++) {				sum = 0.0;				for (k = 0; k<veclen; k++) {					sum += *(a + (i*veclen + k)) * *(b + (k*veclen + j));				}				*(c + (i*veclen + j)) = sum;			}		}	}}