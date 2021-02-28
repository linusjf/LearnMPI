#include <cmath>
#include <cstdlib>
#include <iostream>
#include <mpi.h>
#include <time.h>
#include <math.h>

using namespace std;

void compute(int total, int local, float points[][3], int rank) {

  float local_sum[3] = {0.0f,0.0f,0.0f};
  for (int i = 0; i < local; i++) {
    local_sum[0] += points[i][0];
    local_sum[1] += points[i][1];
    local_sum[2] += points[i][2];
  }
  float barycentre[3];
  MPI_Allreduce(&local_sum,&barycentre,3, MPI_FLOAT,MPI_SUM,MPI_COMM_WORLD);
  // find centroid
  for (int i = 0; i < 3; i++)
    barycentre[i] /= total;

  cout << "Distances for process #" << rank << endl;
  cout << endl;
  for (int i = 0; i < local; i++) {
    float squared = pow(points[i][0] - barycentre[0],2)
      + pow(points[i][1] - barycentre[1],2)
      + pow(points[i][2] - barycentre[2],2);
    float dist = sqrt(squared);
    cout << dist << " ";
  }
  cout << endl << endl;
}

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  int rank, size;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int elems_per_process = atoi(argv[1]);
  int total_count = size * elems_per_process;
  float mypoints[elems_per_process][3];
  srand(time(NULL) * (rank + 1));
  for (int i = 0; i < elems_per_process; i++) {
    for (int j = 0; j < 3; j++)
      mypoints[i][j] = rand() / (float)RAND_MAX * 100;
  }
  compute(total_count, elems_per_process, mypoints, rank);
  MPI_Finalize();
  return 0;
}
