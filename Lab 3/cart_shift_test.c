#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    int rank, size;
    int dims[2] = {2, 2}; // dimensions of the Cartesian topology
    int periods[2] = {0, 0}; // no periodicity in either dimension
    int coords[2]; // Cartesian coordinates of the process
    int nw_rank, se_rank; // ranks of the northwest and southeast elements
    MPI_Comm comm_cart;

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    if (size != 4) {
        printf("Error: this program must be run with 4 processes\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }

    MPI_Cart_create(MPI_COMM_WORLD, 2, dims, periods, 0, &comm_cart);
    MPI_Comm_rank(comm_cart, &rank);
    MPI_Cart_coords(comm_cart, rank, 2, coords);

    // Shift to the northwest
    MPI_Cart_shift(comm_cart, 0, -1, &nw_rank, NULL);
    MPI_Cart_shift(comm_cart, 1, -1, &nw_rank, NULL);

    // Shift to the southeast
    MPI_Cart_shift(comm_cart, 0, 1, &se_rank, NULL);
    MPI_Cart_shift(comm_cart, 1, 1, &se_rank, NULL);

    printf("Process %d: northwest = %d, southeast = %d\n", rank, nw_rank, se_rank);

    MPI_Finalize();
    return 0;
}