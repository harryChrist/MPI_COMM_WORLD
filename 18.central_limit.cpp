#include <iostream>
#include <cstdlib>
#include <mpi.h>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int size, rank;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  constexpr int buffer_count = 5000;
  float buffer[buffer_count];
  memset(buffer, 0, sizeof(buffer));
  
  // Uniform sampling, generating the numbers and doing 1000 repetitions
  for (int rep=0; rep < 1000; ++rep) {
    for (int i=0; i < buffer_count; ++i) {
      float val = (float)rand() / RAND_MAX;
      buffer[i] += val;
    }
  }

  // TODO : create a buffer called reception and call MPI_Reduce to sum all the variables
  // over all the processes and store the result on process 0.
  // In the end, you should have buffer_count variables.
  float reception[buffer_count];
  MPI_Reduce(buffer, reception, buffer_count, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

  // Now we print the results
  if (rank == 0) {
    for (int i=0; i < buffer_count; ++i)
      std::cout << reception[i] << std::endl;
  }
  

  MPI_Finalize();
  
  return 0;
}
