# Makefile to build boxcar diffusion Program 
#
# Andrew J. Pounds, Ph.D.
# Departments of Chemistry and Computer Science
# Mercer University
# Fall 2011 
#

F95 = gfortran   
CC = gcc 

debug ?= n
ifeq ($(debug), y)
    CFLAGS += -g -DDEBUG
else
    CFLAGS += -O3 
endif

OMPOBJ_PATH = ./OpenMP/
PTHOBJ_PATH = ./PThreads/
STKOBJ_PATH = ./Stock/
MKROOM_PATH = ./Makeroom/
OMPOBJS = $(OMPOBJ_PATH)dot.o $(OMPOBJ_PATH)mmm.o $(OMPOBJ_PATH)mvv.o $(OMPOBJ_PATH)vvm.o $(OMPOBJ_PATH)ils.o $(OMPOBJ_PATH)dls.o
PTHOBJS = $(PTHOBJ_PATH)dot.o $(PTHOBJ_PATH)mmm.o $(PTHOBJ_PATH)mvv.o $(PTHOBJ_PATH)vvm.o $(PTHOBJ_PATH)ils.o $(PTHOBJ_PATH)dls.o 
STKOBJS = $(STKOBJ_PATH)dot.o $(STKOBJ_PATH)mmm.o $(STKOBJ_PATH)mvv.o $(STKOBJ_PATH)vvm.o $(STKOBJ_PATH)ils.o $(STKOBJ_PATH)dls.o
MATRIXGENOBJ = $(MKROOM_PATH)matrixgenerator.o

ATLASLIBS = -L/usr/local/ATLAS/lib -lptlapack -lptf77blas -lptcblas -latlas -lpthread

OBJS = array.o zeromat.o walltime.o cputime.o

all: driver atlasdriver 

atlasdriver : atlasdriver.o $(OBJS)    
	$(F95) -o atlasdriver atlasdriver.o $(OBJS) $(ATLASLIBS) 

atlasdriver.o : atlasdriver.f90 array.o   
	$(F95) $(FFLAGS) -c atlasdriver.f90  

driver : driver.o $(OBJS)    
	$(F95) -o driver driver.o $(OBJS) libuparbblas.a -lstdc++ -fopenmp -pthread

driver.o : driver.f90 array.o   
	$(F95) $(FFLAGS) -c driver.f90  

zeromat.o : zeromat.f90    
	$(F95) $(FFLAGS)  -c zeromat.f90  

array.o : array.f90
	$(F95) -c array.f90
  
allbblas: blaslibomp blaslibpth blaslibstock matrixgen
	ar rcs libuparbblas.a $(OMPOBJS) $(PTHOBJS) $(STKOBJS) $(MATRIXGENOBJ)
  
blaslibomp:
	cd OpenMP; make
	ar rcs libomp_bbblas.a $(OMPOBJS)

blaslibpth:
	cd PThreads; make
	ar rcs libpthreads_bblas.a $(PTHOBJS)
  
blaslibstock:
	cd Stock; make
	ar rcs libbblas.a $(STKOBJS)

matrixgen:
	cd Makeroom; make mg
	
# Timing Library targets 

walltime.o : walltime.c
	$(CC)  -c walltime.c

cputime.o : cputime.c
	$(CC)  -c cputime.c

lib: cputime.o walltime.o
	ar -rc liblbstime.a cputime.o walltime.o
	ranlib liblbstime.a

# Default Targets for Cleaning up the Environment
clean :
	rm *.o

pristine :
	rm *.o
	touch *.c *.f90 
	rm *.mod
	rm driver atlasdriver

ctags :
	ctags *.f90 
