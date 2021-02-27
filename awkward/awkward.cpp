#include <iostream>
#include <unistd.h>
#include <mpi.h>

using namespace std;

int main(int argc, char **argv) {
  MPI_Status status;
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  constexpr int small_count = 50;
  constexpr int large_count = 500000;

  int buff1[small_count];
  int buff2[large_count];

  constexpr int waiting_time = 5;

  if (rank == 0) {
    for (int i = 0; i < large_count; i++) {
      if (i < small_count)
        buff1[i] = i;
      buff2[i] = i;
    }
    MPI_Barrier(MPI_COMM_WORLD);
    double time = -MPI_Wtime();
    MPI_Send(&buff1, small_count, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
    cout << "Time to send first buffer : " << time + MPI_Wtime() << endl;
    MPI_Barrier(MPI_COMM_WORLD);
    time = -MPI_Wtime();
    MPI_Send(&buff2, large_count, MPI_INT, rank + 1, 1, MPI_COMM_WORLD);
    cout << "Time to send second buffer : " << time + MPI_Wtime() << endl;
  } else {
    MPI_Barrier(MPI_COMM_WORLD);
    sleep(waiting_time);
    MPI_Recv(&buff1, small_count, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
    cout << "Just received first buffer" << endl;
    MPI_Barrier(MPI_COMM_WORLD);
    sleep(waiting_time);
    MPI_Recv(&buff2, large_count, MPI_INT, rank - 1, 1, MPI_COMM_WORLD, &status);
    cout << "Just received second buffer" << endl;
  }
  MPI_Finalize();
  return 0;
}
