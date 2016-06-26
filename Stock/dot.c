#ifdef __cplusplus
extern "C" {
#endif
	double dot_(int *threads, double *a, double *b, int *len);
#ifdef __cplusplus
}
#endif

double dot_(int *threads, double *a, double *b, int *len)
{
	double temp = 0.0;
	int i;
	int length = *len;

	for (i = 0; i < length; i++) temp += *(a + i) * *(b + i);
	return temp;
}
