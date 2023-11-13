#include <iostream>
#include <cstdlib>
#include <cmath>
#include <mpi.h>
#include <cstddef>

constexpr int DOUBLE_MAX = 10;
struct CustomData {
  int n_values;
  double values[DOUBLE_MAX];
};

int main(int argc, char **argv) {
  
  MPI_Init(&argc, &argv);

  int rank, size;

  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  constexpr int n_structure_per_process = 5; // M = 5

  // Random generator init
  srand(rank * 10);
  
  // Creating the dataset
  CustomData data[n_structure_per_process];

  // Generating the data
  for (int i=0; i < n_structure_per_process; ++i) {
    // Terrible way of generating random numbers, don't reproduce this at home
    data[i].n_values = rand() % DOUBLE_MAX + 1; 
    for (int j=0; j < DOUBLE_MAX; ++j)
      data[i].values[j] = (j < data[i].n_values ? (double)rand() / (double)RAND_MAX : 0.0);
  }

  // 1- Here, create all the properties to call MPI_Type_create_struct
  MPI_Aint displacements[2]  = {};
  int block_lengths[2]  = {};
  MPI_Datatype types[2] = {};
  MPI_Datatype custom_dt;

  // 2- Create the type, and commit it


  // Gathering the data
  CustomData *gathered_data = nullptr;

  if (rank == 0)
    gathered_data = new CustomData[n_structure_per_process * size];
  
  MPI_Gather(data, n_structure_per_process, custom_dt,
	     gathered_data, n_structure_per_process, custom_dt, 0, MPI_COMM_WORLD);

  // And printing it
  if (rank == 0) {
    for (int i=0; i < size; ++i) {
      for (int j=0; j < n_structure_per_process; ++j) {
	int data_id = i * n_structure_per_process + j; // Linear index

	std::cout << "Data structure " << data_id << " : [";
	int N = gathered_data[data_id].n_values;
	
	for (int k=0; k < N; ++k) {
	  std::cout << gathered_data[data_id].values[k] << (k == N-1 ? "]" : "; ");
	}
	std::cout << std::endl;
      }
    }
  }
  
  
  MPI_Finalize();
  
  return 0;
}
