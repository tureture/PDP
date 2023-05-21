#include <stdio.h>
#include <mpi.h>

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank, size, n, k;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    n = 10;  // size of the 2D array
    k = n / size;  // number of rows per process

    // Allocate local 2D array
    int local_array[k][n];
    
    // Initialize local 2D array
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < n; j++) {
            local_array[i][j] = rank * k + i * n + j;
        }
    }

    // Print local 2D array
    printf("Local array for process %d:\n", rank);
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", local_array[i][j]);
        }
        printf("\n");
    }

    // Define MPI derived datatype
    int m = 2;  // number of consecutive rows to send
    MPI_Datatype contiguous_type;
    MPI_Type_contiguous(m * n, MPI_INT, &contiguous_type);
    MPI_Type_commit(&contiguous_type);

    // Send last two rows to right neighbor
    int dest = (rank + 1) % size;
    MPI_Send(&local_array[k-m][0], 1, contiguous_type, dest, 0, MPI_COMM_WORLD);

    // Receive last two rows from left neighbor
    int source = (rank + size - 1) % size;
    MPI_Status status;
    MPI_Recv(&local_array[0][0], 1, contiguous_type, source, 0, MPI_COMM_WORLD, &status);

    // Print updated local 2D array
    printf("Updated local array for process %d:\n", rank);
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d ", local_array[i][j]);
        }
        printf("\n");
    }

    MPI_Type_free(&contiguous_type);
    MPI_Finalize();

    return 0;
}


