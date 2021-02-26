#include <iostream>
#include <math.h>
#include <mpi.h>
#include <unistd.h>

void play_blocking_scenario(int buffer[], int buffer_size, int rank) {
  // initialising buffer
  for (int i = 0; i < buffer_size; ++i)
    buffer[i] = rank == 0 ? i * 2 : 0;
  MPI_Barrier(MPI_COMM_WORLD);
  // Starting the chronometer
  double time = -MPI_Wtime();
  // simulate working
  if (rank == 0) {
    sleep(3);
    std::cout << "First send for process # " << rank << std::endl;
    MPI_Send(buffer, buffer_size, MPI_INT, 1, 0, MPI_COMM_WORLD);
    sleep(6);
    for (int i = 0; i < buffer_size; ++i)
      buffer[i] = -i;
    std::cout << "Second send for process # " << rank << std::endl;
    MPI_Send(buffer, buffer_size, MPI_INT, 1, 1, MPI_COMM_WORLD);
  } else {
    sleep(5);
    std::cout << "First receive for process # " << rank << std::endl;
    MPI_Recv(buffer, buffer_size, MPI_INT, 0, 0, MPI_COMM_WORLD,
        MPI_STATUS_IGNORE);
    sleep(3);
    std::cout << "Second receive for process # " << rank << std::endl;
    MPI_Recv(buffer, buffer_size, MPI_INT, 0, 1, MPI_COMM_WORLD,
        MPI_STATUS_IGNORE);
  }
  time += MPI_Wtime();
  double final_time;
  MPI_Reduce(&time,&final_time,1,MPI_DOUBLE, MPI_MAX,0, MPI_COMM_WORLD);
  if (rank == 0)
    std::cout << "Total time for blocking scenario : " << final_time << std::endl;
}

int main(int argc, char **argv) {
  int rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int buffer_size = 100;
  int buffer[buffer_size];

  play_blocking_scenario(buffer, buffer_size, rank);
  MPI_Finalize();
  return 0;
}
