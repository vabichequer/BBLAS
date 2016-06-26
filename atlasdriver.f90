program driver 

use array
integer :: DIM

real (kind=8) :: wall_start, wall_end
real (kind=8) :: cpu_start, cpu_end
real (kind=8) :: trace


integer :: startval, stopval, stepval, res
real (kind=8) :: walltime
real (kind=8) :: cputime 
external walltime, cputime

character (len=8) :: carg1, carg2

real (kind=8) :: one, zero ! needed for atlas

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

do i = 1, DIM 
     veca(i) = 1.0
     vecb(i) = 1.0 / sqrt( dble(DIM))
enddo

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

wall_start = walltime()
cpu_start = cputime()

one = 1.0D0
zero = 0.0D0

call dgemm('N', 'N', DIM, DIM, DIM, one, matrixa, DIM, &
                                         matrixb, DIM, zero, &
                                         matrixc, DIM )
cpu_end = cputime()
wall_end = walltime()

mflops  = 2 * dble(DIM)**3/ (cpu_end-cpu_start) / 1.0e6
mflops2 = 2 * dble(DIM)**3/ (wall_end-wall_start)/ 1.0e6
 
print *, "MMM: ", DIM, cpu_end-cpu_start, wall_end-wall_start,  mflops, mflops2

wall_start = walltime()
cpu_start = cputime()

call sgesv(DIM, 1, matrixa, DIM, vecb, veca, DIM, res)
cpu_end = cputime()
wall_end = walltime()

mflops  = 2 * dble(DIM)**3/ (cpu_end-cpu_start) / 1.0e6
mflops2 = 2 * dble(DIM)**3/ (wall_end-wall_start)/ 1.0e6
 
print *, "DLS: ", DIM, cpu_end-cpu_start, wall_end-wall_start,  mflops, mflops2

wall_start = walltime()
cpu_start = cputime()

one = 1.0D0
zero = 0.0D0

call ddot(DIM, veca, 1, vecb, 1)
cpu_end = cputime()
wall_end = walltime()

mflops  = 2 * dble(DIM)**3/ (cpu_end-cpu_start) / 1.0e6
mflops2 = 2 * dble(DIM)**3/ (wall_end-wall_start)/ 1.0e6
 
print *, "DOT: ", DIM, cpu_end-cpu_start, wall_end-wall_start,  mflops, mflops2
 
deallocate(matrixa)
deallocate(matrixb)
deallocate(matrixc)
deallocate(veca)
deallocate(vecb)

end program driver 
 
