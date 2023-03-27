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

  int right = (rank + 1) % size; 
  int left = (rank == 0) ? size - 1 : rank - 1;

  printf("Processor %d has neighbors %d and %d \n", rank, left, right);

  /* Exchange variable a, notice the send-recv order */
  if (rank % 2 == 0) {
    MPI_Send(&a, 1, MPI_DOUBLE, right, 111, MPI_COMM_WORLD);
    MPI_Recv(&b, 1, MPI_DOUBLE, left, 222, MPI_COMM_WORLD, &status);
    printf("Processor %d got %f from processor %d \n", rank, b, left);
  } else {
    MPI_Recv(&b, 1, MPI_DOUBLE, left, 111, MPI_COMM_WORLD, &status);
    MPI_Send(&a, 1, MPI_DOUBLE, right, 222, MPI_COMM_WORLD);
    printf("Processor %d got %f from processor %d \n", rank, b, left);
  } 
  MPI_Finalize(); 

  return 0;
}
