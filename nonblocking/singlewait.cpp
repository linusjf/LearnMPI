#include <mpi.h>
#include <stdio.h>
#include <iostream>

int main(int argc, char **argv) {
  int numtasks, rank, next, prev, buf[2], tag1 = 1, tag2 = 2;
  int completed;
  // required variable for non-blocking calls
  MPI_Request reqs[4];
  // required variable for Waitall routine
  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // determine left and right neighbours
  prev = rank - 1;
  next = rank + 1;
  if (rank == 0)
    prev = numtasks - 1;
  if (rank == (numtasks - 1))
    next = 0;

  std::cout << "Starting sends and receives for process # " << rank << std::endl;
  // post non-blocking sends and receives
  MPI_Irecv(&buf[0], 1, MPI_INT, prev, tag1, MPI_COMM_WORLD, &reqs[0]);
  MPI_Irecv(&buf[1], 1, MPI_INT, next, tag2, MPI_COMM_WORLD, &reqs[1]);

  MPI_Isend(&rank, 1, MPI_INT, prev, tag2, MPI_COMM_WORLD, &reqs[2]);
  MPI_Isend(&rank, 1, MPI_INT, next, tag1, MPI_COMM_WORLD, &reqs[3]);

  std::cout << "Waiting for any process to complete in process # " << rank << std::endl;
  MPI_Waitany(1, reqs, &completed, &status);

  std::cout << "Finished for process # " << completed << std::endl;
  MPI_Finalize();
  return 0;
}
