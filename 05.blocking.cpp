#include <iostream>
#include <mpi.h>
#include <cstdlib>

int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);

  int rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  // Read the local value of the process
  // local_value will hold a specific int for process 0, and another for process 1
  int local_value;
  local_value = atoi(argv[1]);

  int other_value;

  if (rank == 0) {
    // Here, enter the code for the first process :
    // 1- Send the value to process 1
    MPI_Send(&local_value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    // 2- Receive the value from process 1 (in other_value)
    MPI_Recv(&other_value, 1, MPI_INT, 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // 3- Print the sum of the two values on stdout 
    std::cout << local_value + other_value << std::endl;
  }
  else {
    // Here enter the code for the second process :
    // 1- Receive the value from process 0 (in other_value)
    MPI_Recv(&other_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    // 2- Send the value to process 0
    MPI_Send(&local_value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    // 3- Print the product of the two values on stdout
    std::cout << local_value * other_value << std::endl;
  }
  
  MPI_Finalize();
  
  return 0;
}
