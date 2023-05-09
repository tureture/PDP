/**********************************************************************
 * Point-to-point communication using MPI
 *
 **********************************************************************/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  int rank, size;
  double a, b;
  MPI_Status status;

  MPI_Init(&argc, &argv);               /* Initialize MPI               */
  MPI_Comm_size(MPI_COMM_WORLD, &size); /* Get the number of processors */
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* Get my number                */
  
  a = 100.0 + (double) rank;  /* Different a on different processors */

  /* Exchange variable a, notice the send-recv order */

  if (rank == 0) {
    MPI_Send(&a, 1, MPI_DOUBLE, 1, 1, MPI_COMM_WORLD);
    printf("Processor 0 sent %f to processor 1\n", a);
    MPI_Recv(&b, 1, MPI_DOUBLE, size-1, size-1, MPI_COMM_WORLD, &status);
    printf("Processor 0 got %f from processor %d\n", b, size-1);
  }

  for (int i = 1; i < size-1; i++) {
    if (rank==i){
      MPI_Recv(&b, 1, MPI_DOUBLE, i-1, i, MPI_COMM_WORLD, &status);
      printf("Processor %d got %f from processor %d...\n ...and sent %f to processor %d\n", rank, b, rank-1, a, rank+1);
      MPI_Send(&a, 1, MPI_DOUBLE, i+1, i+1, MPI_COMM_WORLD);
    }
  } 

  if (rank==size-1){
    MPI_Recv(&b, 1, MPI_DOUBLE, size-2, size-1, MPI_COMM_WORLD, &status);
    printf("Processor %d got %f from processor %d...\n ...and sent %f to processor %d\n", rank, b, rank-1, a, 0);
    MPI_Send(&a, 1, MPI_DOUBLE, 0, size-1, MPI_COMM_WORLD);
  }
  

  MPI_Finalize(); 

  return 0;
}