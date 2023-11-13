#include <iostream>
#include <mpi.h>
#include <cmath>

// Global variables to store the rank of the process and the size
// of the communicator
int rank, size;

// Number of points on one side. The total number of points
// will be p_count * p_count.
constexpr int p_count = 512;

// Other global variables. We read them from the command line
int cutoff;
double min_x, max_x, min_y, max_y, dx, dy;

// The modulus of a complex number
double modulus(double x, double y) {
    return sqrt(x*x + y*y);
} 

// Multiplying a complex number by itself
void self_mul(double &x, double &y) {
    double ox = x*x - y*y;
    double oy = 2*x*y;
    x = ox;
    y = oy;
}

// Computation of the number of iterations on a set of points
// The result is stored in mset.
void compute_mandelbrot(double *points, int npts, int mset[]) {
    for (int i = 0; i < npts; ++i) {
        double px = points[i*2];
        double py = points[i*2 + 1];

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
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    min_x = std::stod(argv[1]);
    max_x = std::stod(argv[2]);
    min_y = std::stod(argv[3]);
    max_y = std::stod(argv[4]);
    dx = max_x - min_x;
    dy = max_y - min_y;
    cutoff = std::stoi(argv[5]);

    int total_points = p_count * p_count;
    int points_per_process = total_points / size;
    int remaining = total_points % size;

    double *points = NULL;
    double *local_points = new double[points_per_process * 2];
    int *local_mset = new int[points_per_process];

    if (rank == 0) {
        points = new double[p_count * p_count * 2];
        for (int yp = 0; yp < p_count; ++yp) {
            double py = min_y + dy * yp / p_count;
            for (int xp = 0; xp < p_count; ++xp) {
                int lid = yp * p_count * 2 + xp * 2;
                points[lid] = min_x + dx * xp / p_count;
                points[lid + 1] = py;
            }
        }
    }

    MPI_Scatter(points, points_per_process * 2, MPI_DOUBLE,
                local_points, points_per_process * 2, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    compute_mandelbrot(local_points, points_per_process, local_mset);

    int *mset = NULL;
    if (rank == 0) {
        mset = new int[total_points];
    }

    MPI_Gather(local_mset, points_per_process, MPI_INT, mset, 
               points_per_process, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank == 0) {
        for (int i = 0; i < total_points; ++i) {
            std::cout << mset[i] << (i % p_count == p_count - 1 ? "\n" : " ");
        }
        delete[] points;
        delete[] mset;
    }

    delete[] local_points;
    delete[] local_mset;

    MPI_Finalize();
    return 0;
}