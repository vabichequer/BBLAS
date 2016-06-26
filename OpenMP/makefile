CC = gcc 
++ = g++

CFLAGS = -O3 -march=nocona -funroll-loops
 
all: jacobi gauss mmm vvm mvv dot

jacobi:
	$(++) $(CFLAGS) -c ils.cpp -o ils.o -fopenmp 
	
gauss:
	$(++) $(CFLAGS) -c dls.cpp -o dls.o -fopenmp
	
mmm:
	$(CC) $(CFLAGS) -c mmm.c -o mmm.o -fopenmp

vvm:
	$(CC) $(CFLAGS) -c vvm.c -o vvm.o -fopenmp

mvv:
	$(CC) $(CFLAGS) -c mvv.c -o mvv.o -fopenmp
	
dot:
	$(CC) $(CFLAGS) -c dot.c -o dot.o -fopenmp
	
	