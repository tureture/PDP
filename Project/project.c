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

mpirun -np 2 ./project 10 out.txt
*/


// Main function
int main(int argc, char *argv[])
{   
    printf("Start \n");
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
    
    printf("After reading input parameters");



    // Initialize parameters & allocate memory
    int N = atoi(argv[1]);
    int n = N/size;
    char *output_file = argv[2];
    int* local_result = (int*)malloc(n * sizeof(int));
    int* x0;
    int* x = (int*)malloc(7 * sizeof(int));

    // Set initial state
    x0 = (int[]){900, 900, 30, 330, 50, 270, 20};

    printf("After allocating memory and stuff");

    // initialize random seed
    srand(time(NULL) + rank);

    // Start timer
    double start = MPI_Wtime();

    // Run simulation n times per process
    for (int i = 0; i < n; i++)
    {
        // Run Gillespie simulation
        local_result[i] = gillespie_simulation(x0, x);
        printf("Rank %d: %d\n", rank, local_result[i]);
      
    }




    // Stop timer
    double time = MPI_Wtime() - start;

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

int gillespie_simulation(int *x0, int *x)
{
    // Initialize variables
    double t = 0, a0, timestep, u1, u2;
    int reaction_index;

    // Copy initial state to x
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
        for (int i = 0; i < 7; i++)
        {
            a0 += w[i];
            cum_w[i] = a0;
        }

        // Generate 2 random numbers
        u1 = (double)rand() / (double)RAND_MAX;
        u2 = (double)rand() / (double)RAND_MAX;

        // Calculate elapsed time
        timestep = -log(u1) / a0;

        // Calculate reaction index
        reaction_index = 0;
        while (cum_w[reaction_index] < u2 * a0)
        {
            reaction_index++;
        }

        // Update state
        update_state(x, reaction_index);

        // Update time
        t += timestep;
    }

    return x[0];
}

// Update state from index 
void update_state(int* x, int reaction_index)
{
    switch (reaction_index)
    {
        case 0:
            x[0] += 1;
            break;
        case 1:
            x[0] -= 1;
            break;
        case 2:
            x[0] -= 1;
            x[2] += 1;
            break;
        case 3:
            x[1] += 1;
            break;
        case 4:
            x[1] -= 1;
            break;
        case 5:
            x[1] -= 1;
            x[3] += 1;
            break;
        case 6:
            x[2] -= 1;
            break;
        case 7:
            x[2] -= 1;
            x[4] += 1;
            break;
        case 8:
            x[3] -= 1;
            break;
        case 9:
            x[3] -= 1;
            x[5] += 1;
            break;
        case 10:
            x[4] -= 1;
            break;
        case 11:
            x[4] -= 1;
            x[6] += 1;
            break;
        case 12:
            x[5] -= 1;
            break;
        case 13:
            x[0] += 1;
            x[6] -= 1;
            break;
        case 14:
            x[6] -= 1;
            break;
        default:
            perror("Invalid reaction index");
            break;
    }
}

