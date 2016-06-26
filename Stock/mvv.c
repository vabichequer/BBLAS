#ifdef __cplusplus
extern "C" {
#endif
    void mvv_(int *threads, int *len, double *ma, double *vb, double *vr);
#ifdef __cplusplus
    }
#endif

void  mvv_(int *threads, int *len, double *ma, double *vb, double *vr){

	int i, j, alength = *len;

	for (i=0; i<alength; i++) {
		for (j=0; j<alength; j++) {
			*(vr+i) = *(ma + (alength*i) + j) * *(vb+j);
		}
	}

}