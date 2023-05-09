#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include "prop.h"
#include "project.h"

/*
Project in Parallel & Distributed Computing
Uppsala University, VT23

Monte Carlo computations & Stochastic Simulation algorithm to simulate malaria epidemic

By Ture Hassler
*/


// Main function
int main(int argc, char *argv[])
{   
    // Initialize MPI
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD,&size);

    // read input parameters
    if (rank == 0) 
    {
        if (argc != 3)
            {
                printf("Usage: %s <total number of iterations> <output file>\n", argv[0]);
                return -1;
            }

    }
    
    // Initialize parameters
    int N = atoi(argv[1]);
    int n = N/size;
    char *output_file = argv[2];

    
    
    
    // Start timer
    double time = MPI_Wtime();


    // Stop timer
    time = MPI_Wtime() - time;

    // Print max time 
    double max_time;
    MPI_Reduce(&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        printf("Max time: %fs \n", max_time);
    }


    // Free memory


    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}

int gillespie_simulation(int *x0)
{
    // Initialize variables
    double t = 0, a0, timestep;
    int reaction_index, final_x;

    // Allocate memory for state vector
    int x[7];
    memcpy(x, x0, 7 * sizeof(int));

    // Allocate memory for propensity vector
    double w[7];
    double cum_w[7];

    while (t < 100)
    {
        // Calculate propensities
        prop(x, w);

        // Calculate a0
        a0 = 0;

    }



    return final_x;
}



