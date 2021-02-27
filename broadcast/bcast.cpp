#include <iomanip>
#include <iostream>
#include <mpi.h>

using namespace std;

double sumall(double *buffer, int count) {
 double sum = 0.0;
  for (int i = 0; i < count; i++) 
      sum += buffer[i];
  return sum;
}

double sumpositive(double *buffer, int count) {
 double sum = 0.0;
  for (int i = 0; i < count; i++) {
    if (buffer[i] > 0)
      sum += buffer[i];
  }
  return sum;
}
    
double sumnegative(double *buffer, int count) {
 double sum = 0.0;
  for (int i = 0; i < count; i++) {
    if ( buffer[i] < 0)
      sum += buffer[i];
  }
    return sum;
}

int main(int argc, char **argv) {
  int rank, size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  int n_elements;
  double *buffer;

  if (rank == 0) {
    cin >> n_elements;
    buffer = new double[n_elements];
    for (int i = 0; i < n_elements; i++)
      cin >> buffer[i];
  }

  MPI_Bcast(&n_elements, 1, MPI_INT, 0, MPI_COMM_WORLD);
  if (rank != 0) {
    buffer = new double[n_elements];
    cout << "buffer allocated for process " << rank << endl;
  }

  MPI_Bcast(buffer, n_elements, MPI_DOUBLE, 0, MPI_COMM_WORLD);
  cout << "buffer for process " << rank << endl;
  for (int i = 0; i < n_elements; i++)
      cout << buffer[i] << " ";
  cout << endl;
  double sum = 0.0;
  switch (rank) {
    case 0:
      sum = sumall(buffer,n_elements);
      break;
    case 1:
      sum = sumpositive(buffer,n_elements);
      break;
    case 2:
      sum = sumnegative(buffer,n_elements);
      break;
  }

  cout << "sum for process #" << rank << " : " << setprecision(16) << sum << endl;

  MPI_Finalize();
  delete[] buffer;
  return 0;
}

