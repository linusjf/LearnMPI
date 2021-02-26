#include <iostream>
#include <math.h>
#include <mpi.h>
#include <unistd.h>

using namespace std;

void race_scenario(int buffer[], int buffer_size, int rank) {
  MPI_Request request;
  if (rank == 0) {
    // initialising buffer
    for (int i = 0; i < buffer_size; ++i) {
      buffer[i] = i * i;
      cout << buffer[i] << " ";
    }
    cout << endl;
    MPI_Isend(buffer, buffer_size, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
    sleep(5);
  } else {
    //MPI_Status status;
    for (int i = 0; i < buffer_size; ++i)
      buffer[i] = 0;
    MPI_Irecv(buffer, buffer_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
    sleep(5);
    //MPI_Wait(&request,&status);
    cout << "Process 1 received" << endl;
    for (int i = 0; i < buffer_size; ++i)
      cout << buffer[i] << " ";
    cout << endl;
  }
}

int main(int argc, char **argv) {
  int rank;
  int size;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  constexpr int buffer_size = 10;
  int buffer[buffer_size];

  race_scenario(buffer, buffer_size, rank);
  MPI_Finalize();
  return 0;
}
