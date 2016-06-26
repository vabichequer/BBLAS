CC = gcc 
++ = g++

CFLAGS = -O3 -march=nocona -funroll-loops

all: jacobi gauss mmm vvm mvv dot

jacobi:
	$(++) $(CFLAGS) -c ils.cpp -o ils.o  
	
gauss:
	$(CC) $(CFLAGS) -c dls.cpp -o dls.o 
	
mmm:
	$(CC) $(CFLAGS) -c mmm.c -o mmm.o

vvm:
	$(CC) $(CFLAGS) -c vvm.c -o vvm.o

mvv:
	$(CC) $(CFLAGS) -c mvv.c -o mvv.o 
	
dot:
	$(CC) $(CFLAGS) -c dot.c -o dot.o