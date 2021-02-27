#include <cstdlib>
#include <iostream>
#include <mpi.h>

using namespace std;

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  constexpr int buffer_count = 5000;
  float buffer[buffer_count];
  memset(buffer,0, sizeof(buffer));
  // uniform sampling
  for (int rep = 0; rep < 1000; ++rep) {
   for (int i = 0; i < buffer_count; ++i)
     buffer[i] += rand() / (float)RAND_MAX;
  }
 
  float reception[buffer_count];
  MPI_Reduce(&buffer, &reception, buffer_count, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
  if (rank == 0) {
    for (int i = 0; i < buffer_count; ++i)
    cout << reception[i] << " ";
  cout << endl;
  }
  MPI_Finalize();
  return 0;
}
