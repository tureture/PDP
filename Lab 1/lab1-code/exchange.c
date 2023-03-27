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
  MPI_Status status[2];
  MPI_Request request[2];

  MPI_Init(&argc, &argv);               /* Initialize MPI               */
  MPI_Comm_size(MPI_COMM_WORLD, &size); /* Get the number of processors */
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* Get my number                */
  
  a = 100.0 + (double) rank;  /* Different a on different processors */

  int right = (rank + 1) % size; 
  int left = (rank == 0) ? size - 1 : rank - 1;

  printf("Processor %d has neighbors %d and %d \n", rank, left, right);

  /* Exchange variable a, notice the send-recv order */
  MPI_Isend(&a, 1, MPI_DOUBLE, right, 1, MPI_COMM_WORLD, &request[0]);
  MPI_Irecv(&b, 1, MPI_DOUBLE, left, 1, MPI_COMM_WORLD, &request[1]);

  MPI_Waitall(2, request, status);

  printf("Processor %d got %f from processor %d \n", rank, b, left);

  MPI_Finalize(); 


  return 0;
}
