CC = gcc
++ = g++

CFLAGS = -O3 -march=nocona -funroll-loops

all: jacobi gauss mmm vvm mvv dot

jacobi:
	$(++) $(CFLAGS) -c ils.cpp -o ils.o -pthread 
	
gauss:
	$(++) $(CFLAGS) -c dls.cpp -o dls.o -pthread
	
mmm:
	$(CC) $(CFLAGS) -c mmm.c -o mmm.o -pthread

vvm:
	$(CC) $(CFLAGS) -c vvm.c -o vvm.o -pthread

mvv:
	$(CC) $(CFLAGS) -c mvv.c -o mvv.o -pthread
	
dot:
	$(CC) $(CFLAGS) -c dot.c -o dot.o -pthread
	
	