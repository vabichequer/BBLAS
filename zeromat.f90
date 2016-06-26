
subroutine zeromat ( msize )

use array
integer :: msize


! Initialize Box to Sero
do i = 1, msize
   do j = 1, msize
          matrixa(j,i) = 0.0D0 
          matrixc(j,i) = 0.0D0 
   end do
end do

end subroutine zeromat
