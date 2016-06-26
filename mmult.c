#ifdef __cplusplus
extern "C" {
#endif
    void mmult_(  double *a, int *lena,
                  double *b, int *lenb,
                  double *c, int *lenc); 
#ifdef __cplusplus
    }
#endif

void mmult_( double *a, int *lena, double *b, int *lenb, double *c, int *lenc){
   
double temp;
int i, j, k;

for (i=0; i<*lena; i++) {
    for (j=0; j<*lenb; j++) {
       temp = 0.0;
       for (k=0; k<*lena; k++) {
          temp = temp + *(a+(i* *lena+k)) * *(b+(j* *lenb+k));
       }
       *(c+(i* *lenc+j)) = temp;
    }
}
}       
