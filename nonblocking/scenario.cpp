#include <iostream>
#include <math.h>
#include <mpi.h>
#include <unistd.h>

using namespace std;

void print_buffer(int buffer[], int buffer_size) {
  cout << "{ ";
  for (int i = 0; i < buffer_size; i++)
    cout << buffer[i] << " ";
  cout << " } " << endl;
}

void play_nonblocking_scenario(int buffer[], int buffer_size, int rank) {
  MPI_Request request;
  MPI_Status status;
  int request_finished = 0;
  // initialising buffer
  for (int i = 0; i < buffer_size; ++i)
    buffer[i] = rank == 0 ? i * 2 : 0;
  MPI_Barrier(MPI_COMM_WORLD);
  // Starting the chronometer
  double time = -MPI_Wtime();
  // simulate working
  if (rank == 0) {
    sleep(3);
    cout << "First send for process # " << rank << endl;
    MPI_Isend(buffer, buffer_size, MPI_INT, 1, 0, MPI_COMM_WORLD, &request);
    double time_left = 6000.0;
    while (time_left > 0.0 && request_finished != 0) {
      usleep(1000);
      MPI_Test(&request, &request_finished, &status);
      time_left -= 1000.0;
    }
    if (request_finished == 0)
      MPI_Wait(&request, &status);
    for (int i = 0; i < buffer_size; ++i)
      buffer[i] = -i;
    cout << "Second send for process # " << rank << endl;
    MPI_Isend(buffer, buffer_size, MPI_INT, 1, 1, MPI_COMM_WORLD, &request);
    request_finished = 0;
    time_left = 3000.0;
    while (time_left > 0.0 && request_finished != 0) {
      usleep(1000);
      MPI_Test(&request, &request_finished, &status);
      time_left -= 1000.0;
    }
    if (request_finished == 0)
      MPI_Wait(&request, &status);
  } else {
    sleep(5);
    cout << "First receive for process # " << rank << endl;
    MPI_Irecv(buffer, buffer_size, MPI_INT, 0, 0, MPI_COMM_WORLD, &request);
    MPI_Wait(&request, &status);
    print_buffer(buffer, buffer_size);
    sleep(3);
    cout << "Second receive for process # " << rank << endl;
    MPI_Irecv(buffer, buffer_size, MPI_INT, 0, 1, MPI_COMM_WORLD, &request);
    MPI_Wait(&request, &status);
    print_buffer(buffer, buffer_size);
  }
  time += MPI_Wtime();
  double final_time;
  MPI_Reduce(&time, &final_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
  if (rank == 0)
    cout << "Total time for non-blocking scenario : " << final_time
              << endl;
}

int main(int argc, char **argv) {
  int rank;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int buffer_size = 100;
  int buffer[buffer_size];

  play_nonblocking_scenario(buffer, buffer_size, rank);
  MPI_Finalize();
  return 0;
}
