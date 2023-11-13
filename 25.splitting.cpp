// You have access to the following global variables :
int world_rank, world_size;
MPI_Comm custom_comm1, custom_comm2, custom_comm3, tmp;

// The tmp communicator is provided if you need to point somewhere, where you don't
// care about the outputted communicator. For instance, when using MPI_UNDEFINED as color.

// world_rank : rank of the process on MPI_COMM_WORLD
// world_size : size of MPI_COMM_WORLD
// These two variables are already initialised when splitting() is called

void splitting() {
    // Define a color variable for MPI_Comm_split
    int color;

    // 1- First splitting here
    // Processes 0-3 in custom_comm1 and processes 4-6 in custom_comm2
    if (world_rank >= 0 && world_rank <= 3) {
        color = 1; // Color for custom_comm1
        MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &custom_comm1);
    } else if (world_rank >= 4 && world_rank <= 6) {
        color = 2; // Color for custom_comm2
        MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &custom_comm2);
    } else {
        // Process not part of custom_comm1 or custom_comm2
        MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, world_rank, &tmp);
    }

    // 2- Second splitting here
    // Processes 0 and 4 in custom_comm3
    if (world_rank == 0 || world_rank == 4) {
        color = 3; // Color for custom_comm3
        MPI_Comm_split(MPI_COMM_WORLD, color, world_rank, &custom_comm3);
    } else {
        // Process not part of custom_comm3
        MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, world_rank, &tmp);
    }
}
