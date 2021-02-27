#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char **argv) {
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  constexpr int count = 5;

  int buff1[count];

  if (rank == 0) {
    for (int i = 0; i < count; i++)
      buff1[i] = i * i;
  }
  MPI_Bcast(&buff1, count, MPI_INT, 0, MPI_COMM_WORLD);
  cout << "Buffer data for process #" << rank << endl;
  for (int i = 0; i < count; i++)
    cout << buff1[i] << " ";
  cout << endl;
  
  MPI_Finalize();
  return 0;
}
