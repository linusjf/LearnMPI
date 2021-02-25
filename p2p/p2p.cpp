#include <iostream>
#include <mpi.h>
#include <cstdlib>
#include <time.h>

int main(int argc, char **argv) {
  srand(time(0));
  int first_num = rand();
  int second_num = rand();
  int third_num = rand();
  int fourth_num = rand();
  int fifth_num = rand();
  int val;
  MPI_Init(&argc, &argv);
  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  switch (rank) {
   case 0:
    MPI_Send(&first_num, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    MPI_Recv(&val, 1, MPI_INT, 4, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::cout << " process # " << rank << " : " << first_num << " + " << val  << " = " << first_num + val << std::endl;
     break;

   case 1:
    MPI_Recv(&val, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(&second_num, 1, MPI_INT, 2, 0, MPI_COMM_WORLD);
    std::cout << " process # " << rank << " : " << second_num << " - " << val  << " = " << second_num - val << std::endl;
     break;

  case 2:
    MPI_Send(&third_num, 1, MPI_INT, 3, 0, MPI_COMM_WORLD);
    MPI_Recv(&val, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::cout << " process # " << rank << " : " << third_num << " * " << val  << " = " << third_num *
       val << std::endl;
     break;

  case 3:
    MPI_Recv(&val, 1, MPI_INT, 2, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Send(&fourth_num, 1, MPI_INT, 4, 0, MPI_COMM_WORLD);
    std::cout << " process # " << rank << " : " << fourth_num << " / " << val  << " = " << fourth_num /
       val << std::endl;
     break;

  case 4:
    MPI_Send(&fifth_num, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    MPI_Recv(&val, 1, MPI_INT, 3, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    std::cout << " process # " << rank << " : " << fifth_num << " % " << val  << " = " << fifth_num %
       val << std::endl;
     break;
  }
  MPI_Finalize();
}
