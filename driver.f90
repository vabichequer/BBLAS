program driver 

use array 
integer :: DIM

real (kind=8) :: wall_start, wall_end
real (kind=8) :: cpu_start, cpu_end
real (kind=8) :: trace


integer :: num_threads
double precision :: res
real (kind=8) :: walltime
real (kind=8) :: cputime 
real (kind=8) :: dot
real (kind=8) :: ompdot
real (kind=8) :: pthdot 
external walltime, cputime, dot, ompdot, pthdot

character (len=8) :: carg1, carg2

!modified to use command line arguments

call get_command_argument(1, carg1)
call get_command_argument(2, carg2)

! Use Fortran internal files to convert command line arguments to ints

read (carg1,'(i8)') DIM 
read (carg2,'(i8)') num_threads

allocate ( veca(DIM), stat=ierr)
allocate ( vecb(DIM), stat=ierr)
allocate ( matrixa(DIM,DIM), stat=ierr) 
allocate ( matrixb(DIM,DIM), stat=ierr)
allocate ( matrixc(DIM,DIM), stat=ierr)	 

  !call matrixgenerator(DIM, matrixa, veca);
  
  do i = 1, DIM
   veca(i) = i
   sumrow = 0.0
   do j = 1, DIM
   if ( i .ne. j) then
     call random_number(ranval)
     matrixa(j,i) = ranval
     sumrow = sumrow + ranval
     call random_number(ranval)
     if ( ranval < 0.5 ) matrixa(j,i) = -matrixa(j,i)
   endif
   matrixa(i,i) = sumrow + 1.0
   enddo
 enddo 
   
  do i = 1, DIM
   sumrow = 0.0
   do j = 1, DIM
   if ( i .ne. j) then
     call random_number(ranval)
     matrixb(j,i) = ranval
     sumrow = sumrow + ranval
     call random_number(ranval)
     if ( ranval < 0.5 ) matrixb(j,i) = -matrixb(j,i)
   endif
   matrixb(i,i) = sumrow + 1.0
   enddo
 enddo 
  
!!!!!!!!!!!!!!!!!!!!

 
  
 cpu_start = cputime();
 wall_start = walltime(); 
 call ils(num_threads, DIM, matrixa, veca, vecb); 
 cpu_end = cputime();
 wall_end = walltime(); 
 
 mflops  = 2 * dble(DIM)**3/ (cpu_end-cpu_start) / 1.0e6
 mflops2 = 2 * dble(DIM)**3/ (wall_end-wall_start)/ 1.0e6
 print *, "ILS: ", DIM, cpu_end-cpu_start, wall_end-wall_start,  mflops, mflops2
 
!!!!!!!!!!!!!!!!!!!!
 
deallocate(matrixa)
deallocate(matrixb)
deallocate(matrixc)
deallocate(veca)
deallocate(vecb)

end program driver 
 
