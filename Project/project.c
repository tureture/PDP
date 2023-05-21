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

mpirun -np 2 ./project 10 out.txt 0 0
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
        if (argc != 5)
        {
            printf("Usage: %s <total number of iterations> <output file> <write to file 0/1> <print timesplits & histogram 0/1> \n", argv[0]);
            return -1;
        }
    }

    // Initialize parameters & allocate memory
    int N = atoi(argv[1]);
    int n = N/size;
    char *output_file = argv[2];
    int write_to_file = atoi(argv[3]);
    int print = atoi(argv[4]);
    int* local_result;
    int x0[] = {900, 900, 30, 330, 50, 270, 20};
    int* x;
    double w[15];
    double cum_w[15];
    int time_updates[4] = {25, 50, 75, 100}; // added extra value to not have to check for end of array

    // Allocate memory
    local_result = (int*)malloc(n * sizeof(int));
    x = (int*)malloc(7 * sizeof(int));

    // Allocate shared memory for one-way communications & create window
    double shared_times[4 * size];
    //MPI_Win win;
    //MPI_Win_create(shared_times, 4 * size * sizeof(double), sizeof(double), MPI_INFO_NULL, MPI_COMM_WORLD, &win);

    // Initialize shared memory to 0
    if (rank == 0)
    {
        for (int i = 0; i < 4 * size; i++)
        {
            shared_times[i] = 0;
        }
    }

    // Initialize random seed
    srand(time(NULL) + rank);

    // Start timer
    double start = MPI_Wtime();

    // Start MPI one-sided communication epoch
    //MPI_Win_fence(0, win);

    // Run simulation n times per process
    for (int i = 0; i < n; i++)
    {   
        // Run Gillespie simulation
        local_result[i] = gillespie_simulation(x0, x, shared_times, win, rank, w, cum_w, time_updates); 
    }

    // find max and min elements locally
    int global_max, global_min;
    int max, min;
    if (n > 0) // getting rid of warning...
    {
        max = local_result[0];
        min = local_result[0];
    }

    for (int i = 1; i < n; i++)
    {
        if (local_result[i] > max)
            max = local_result[i];
        if (local_result[i] < min)
            min = local_result[i];
    }

    // find max and min elements globally
    MPI_Allreduce(&max, &global_max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
    MPI_Allreduce(&min, &global_min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);

    // Create histogram bins
    int diff = global_max - global_min;
    int bin_size = diff / nr_bins;
    int remainder = diff % nr_bins;
    int sum = global_min;
    int* bins = (int*)malloc(nr_bins * sizeof(int));

    for (int i = 0; i < nr_bins; i++)
    {
        sum += bin_size + (i < remainder ? 1:0); // attempt to make bins as equal as possible
        bins[i] = sum;
    }

    // Count number of elements in each bin
    int* bin_counts = (int*)malloc(nr_bins * sizeof(int));
    for (int i = 0; i < nr_bins; i++)
    {
        bin_counts[i] = 0;
    }

    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < nr_bins; j++)
        {
            if (local_result[i] <= bins[j])
            {
                bin_counts[j]++;
                break;
            }
        }
    }

    // Sum up results of all bins
    int* global_bin_counts = (int*)malloc(nr_bins * sizeof(int));
    MPI_Reduce(bin_counts, global_bin_counts, nr_bins, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    // End MPI one-sided communication epoch
    MPI_Win_fence(0, win);

    // Stop timer
    double time = MPI_Wtime() - start;

    // Print results
    if (rank == 0 && print)
    {
        print_histogram(bins, global_bin_counts, nr_bins, global_min, global_max);
    }

    // Print max time 
    double max_time;
    MPI_Reduce(&time, &max_time, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);
    if (rank == 0)
    {
        printf("Max time: %fs \n", max_time);
    }

    /*
    if (rank == 0 && print)
    {   
        printf("Timesplits: \n");
        printf("\n");
        for (int i = 0; i<size; i++)
        {
            printf("P %d: \n", i);
            for (int j = 0; j < 4; j++)
            printf("Split %d: %fs \n", j+1, shared_times[i*4 + j]);
            printf("\n");
        }
        printf("\n");
    }
    
    */
    // print timesplits



    // Write to file
    if (rank == 0 && write_to_file)
    {
        write_file(output_file, bins, global_bin_counts, nr_bins, global_min, global_max, N, max_time);
    }


    // Free memory
    free(local_result);
    free(x);
    free(bins);
    free(bin_counts);
    free(global_bin_counts);


    // Free MPI window 
    //MPI_Win_free(&win);


    // Finalize the MPI environment.
    MPI_Finalize();

    return 0;
}

int gillespie_simulation(int *x0, int *x, double* times, MPI_Win win, int rank, double* w, double* cum_w, int* time_updates)
{
    // Initialize variables
    double t = 0, a0, timestep, u1, u2;
    int reaction_index, time_updates_counter = 0;
    double start = MPI_Wtime();
    double elapsed_time;


    // Copy initial state to x
    memcpy(x, x0, 7 * sizeof(int));

    // Start timer
    start = MPI_Wtime();

    while (t < time_limit)
    {   
        /*
        if (t > time_updates[time_updates_counter])
        {
            elapsed_time = MPI_Wtime() - start;
            MPI_Accumulate(&elapsed_time, 1, MPI_DOUBLE, 0, rank * 4 + time_updates_counter, 1, MPI_DOUBLE , MPI_SUM , win);
            time_updates_counter++;
        }
        
        */



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

    //elapsed_time = MPI_Wtime() - start;
    //MPI_Accumulate(&elapsed_time, 1, MPI_DOUBLE, 0, rank * 4 + time_updates_counter, 1, MPI_DOUBLE , MPI_SUM , win);


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

void print_histogram(int* bins, int* bin_counts, int nr_bins, int min, int max)
{
    printf("Histogram:\n");
    printf("%d-%d: %d\n", min, bins[0], bin_counts[0]);
    for (int i = 1; i < nr_bins; i++)
    {
        printf("%d-%d: %d\n", bins[i - 1], bins[i], bin_counts[i]);
    }
    printf("\n");
}

// Writes to file.
// Uses order N, time, nr_bins, min, max and then the bins and bin_counts
void write_file(char* filename, int* bins, int* bin_counts, int nr_bins, int min, int max, int N, double time)
{
    FILE *fp;
    fp = fopen(filename, "w+");
    fprintf(fp, "N:%d, Time:%f, Nr_Bins:%d, Min:%d, Max:%d \n", N, time, nr_bins, min, max);
    fprintf(fp, "%d %d %d\n", min, bins[0], bin_counts[0]);
    for (int i = 1; i < nr_bins; i++)
    {
        fprintf(fp, "%d %d %d\n", bins[i - 1], bins[i], bin_counts[i]);
    }
    fclose(fp);
}
