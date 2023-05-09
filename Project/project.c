#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <math.h>
#include <time.h>
#include <string.h>
#include <limits.h>
#include "prop.h"
#include "project.h"

/*
Project in Parallel & Distributed Computing
Uppsala University, VT23

Monte Carlo computations & Stochastic Simulation algorithm to simulate malaria epidemic

By Ture Hassler

mpirun -np 2 ./project 10 out.txt
*/

// Global variables
const int length_of_x = 7;
const int lengt_of_w = 15;
const int time_limit = 100;
const int nr_bins = 20;


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

    // Initialize parameters & allocate memory
    int N = atoi(argv[1]);
    int n = N/size;
    char *output_file = argv[2];
    int* local_result, *global_result;
    int x0[] = {900, 900, 30, 330, 50, 270, 20};
    int* x;

    // Allocate memory
    local_result = (int*)malloc(n * sizeof(int));
    x = (int*)malloc(7 * sizeof(int));


    // Set initial state
    // x0 = (int[]){900, 900, 30, 330, 50, 270, 20};

    // initialize random seed
    srand(time(NULL) + rank);

    // Start timer
    double start = MPI_Wtime();

    // Run simulation n times per process
    for (int i = 0; i < n; i++)
    {   
        // Run Gillespie simulation
        local_result[i] = gillespie_simulation(x0, x); 
    }

    // find max and min elements locally
    int global_max, global_min;
    int max = local_result[0];
    int min = local_result[0];
    for (int i = 1; i < n; i++)
    {
        if (local_result[i] > max)
            max = local_result[i];
        if (local_result[i] < min)
            min = local_result[i];
    }

    // find max and min elements globally
    MPI_Reduce(&max, &global_max, 1, MPI_INT, MPI_MAX, 0, MPI_COMM_WORLD);
    MPI_Reduce(&min, &global_min, 1, MPI_INT, MPI_MIN, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        printf("Max: %d, Min: %d\n", max, min);
    }

    // Create histogram bins
    int diff = global_max - global_min;
    int bin_size = diff / nr_bins;
    int remainder = diff % nr_bins;
    int sum = global_min;
    int* bins = (int*)malloc(nr_bins * sizeof(int));
    for (int i = 0; i < nr_bins; i++)
    {
        sum += bin_size + (i < remainder ? 1 : 0); // attempt to make bins as equal as possible
        bins[i] = sum;
    }

    // Count number of elements in each bin
    int* bin_counts = (int*)malloc(nr_bins * sizeof(int));
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < nr_bins; j++)
        {
            if (local_result[i] < bins[j])
            {
                bin_counts[j]++;
                break;
            }
        }
    }

    // Sum up results of all bins
    int* global_bin_counts = (int*)malloc(nr_bins * sizeof(int));
    MPI_Reduce(bin_counts, global_bin_counts, nr_bins, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // Stop timer
    double time = MPI_Wtime() - start;

    // Print results
    if (rank == 0)
    {
        // Print histogram
        printf("Histogram:\n");
        for (int i = 0; i < nr_bins; i++)
        {
            printf("%d-%d: %d\n", bins[i] - bin_size, bins[i], global_bin_counts[i]);
        }

    }

    // Print max time 
    double max_time;
    MPI_Reduce(&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        printf("Max time: %fs \n", max_time);
    }


    // Free memory
    free(local_result);
    free(x);

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
    double w[15];
    double cum_w[15];

    while (t < time_limit)
    {   
        // Calculate propensities
        prop(x, w);

        // Calculate a0
        a0 = 0;
        for (int i = 0; i < lengt_of_w; i++)
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

