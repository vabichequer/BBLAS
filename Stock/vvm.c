#ifdef __cplusplus
extern "C" {
#endif
	void vvm_(int *threads, int *len, double *va, double *vb, double *mr);
#ifdef __cplusplus
}
#endif

void  vvm_(int *threads, int *len, double *va, double *vb, double *mr) {

	int i, j, alength = *len;

	for (i = 0; i<alength; i++) {
		for (j = 0; j<alength; j++) {
			*(mr + (alength*i) + j) = *(va + i) * *(vb + j);
		}
	}

}