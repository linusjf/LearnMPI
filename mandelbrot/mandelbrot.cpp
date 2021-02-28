#include <iostream>
#include <math.h>
#include <mpi.h>

// global variables
int rank, size;
constexpr int p_count = 512;
int cutoff;
double min_x, min_y, max_x, max_y, dx, dy;

double modulus(double x, double y) { return sqrt(x * x + y * y); }

void self_mul(double &x, double &y) {
  double ox = x * x - y * y;
  double oy = x * y + y * x;
  x = ox;
  y = oy;
}

void compute_mandelbrot(double *points, int n_pts, int mset[]) {
  for (int i = 0; i < n_pts; i++) {
    double px = points[i * 2];
    double py = points[i * 2 + 1];
    int iteration = 0;
    double zx = 0;
    double zy = 0;
    while (iteration < cutoff) {
      self_mul(zx, zy);
      zx += px;
      zy += py;
      double mod = modulus(zx, zy);
      if (mod > 2.0f)
        break;
      iteration++;
    }
    if (iteration == cutoff)
      mset[i] = -1;
    else 
      mset[i] = iteration;
  }
}

int main(int argc, char **argv) {
  min_x = std::stod(argv[1]);
  max_x = std::stod(argv[2]);
  min_y = std::stod(argv[3]);
  max_y = std::stod(argv[4]);
  dx = max_x - min_x;
  dy = max_y - min_y;
  cutoff = std::stoi(argv[5]);

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);

  MPI_Barrier(MPI_COMM_WORLD);
  double *points = NULL;

  // if (rank == 0) {
  points = new double[p_count * p_count * 2];
  for (int yp = 0; yp < p_count; yp++) {
    double py = min_y + dy * yp / p_count;
    for (int xp = 0; xp < p_count; xp++) {
      double px = min_x + dy * xp / p_count;
      int lid = yp * p_count * 2 + xp * 2;
      points[lid] = px;
      points[lid + 1] = py;
    }
  }
  ///}
  MPI_Barrier(MPI_COMM_WORLD);
  int npts = p_count * p_count;
  int mset[npts];
  compute_mandelbrot(points, npts, mset);
  MPI_Barrier(MPI_COMM_WORLD);
  if (rank == 0) {
    for (int yp = 0; yp < p_count; yp++) {
      for (int xp = 0; xp < p_count; xp++)
        std::cout << mset[yp * p_count + xp] << " ";
      std::cout << std::endl;
    }
  }

  delete[] points;
  MPI_Finalize();
  return 0;
}
