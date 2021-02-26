#include <iostream>
#include <mpi.h>
#include <math.h>

int main(int argc, char **argv) {
  MPI_Status status;
  int num;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &num);

  double d = 100.00;
  int tag = 1;
  
  if (num == 0) {
   MPI_Ssend(&d,1,MPI_DOUBLE,num + 1,tag,MPI_COMM_WORLD);
   MPI_Recv(&d,1,MPI_DOUBLE,num + 1,tag,MPI_COMM_WORLD, &status);
  }
  else {
   MPI_Ssend(&d,1,MPI_DOUBLE,num - 1,tag,MPI_COMM_WORLD);
   MPI_Recv(&d,1,MPI_DOUBLE,num - 1,tag,MPI_COMM_WORLD, &status);
  }
  MPI_Finalize();
  return 0;
}
