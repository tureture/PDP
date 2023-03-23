/**********************************************************************
 * A simple "hello world" program for MPI/C
 *
 **********************************************************************/

#include <mpi.h>
#include <stdio.h>

int main(int argc, char *argv[]) {

  MPI_Init(&argc, &argv);               /* Initialize MPI               */
  
  int rank, size;

  MPI_Comm_size(MPI_COMM_WORLD, &size); /* Get the number of processors */
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); /* Get my number                */

  if (rank == 0){
    printf("Number of processors: %d \n", size);
  }


  printf("Hello World from %d!\n", rank);             /* Print a message              */

  MPI_Finalize();                       /* Shut down and clean up MPI   */

  return 0;
}
