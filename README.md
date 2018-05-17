# mpi-md5
# make file
  mpicc –Wall –o mpi-md5 mpi-md5.c –lcrypto –lssl

# run file
mpirun –np <số tiến trình> -host <[tên host,]> ./mpi-md5 <mã md5 32 kí tự> <số kí tự tối đa của mật khẩu>.
