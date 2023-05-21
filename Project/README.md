# Parallel & Distributed Programming - Project
# Parallel Monte Carlo Stochastic Simulation Algorithm for Malaria Simulation

This repository contains an implementation of a parallel Monte Carlo stochastic simulation algorithm for simulating malaria using the Message Passing Interface (MPI). The algorithm allows for efficient parallelization of the simulation, enabling faster computation of large-scale malaria simulations.

## Usage

To run the simulation, you need to provide the following command-line inputs:

Usage: <total number of iterations> <output file> <write to file 0/1> <print timesplits & histogram 0/1>
  
 - `<total number of iterations>`: The total number of iterations for the simulation. This determines the number of times the simulation algorithm will be executed.
- `<output file>`: The file path where the simulation results will be stored.
- `<write to file 0/1>`: A binary flag (0 or 1) indicating whether the simulation results should be written to the output file. If set to 1, the results will be saved; otherwise, they will only be displayed on the console.
- `<print timesplits & histogram 0/1>`: Another binary flag (0 or 1) indicating whether the timesplits and histogram should be printed. If set to 1, the timesplits and histogram information will be displayed alongside the simulation results.

## About
For the course project in Parallell and Distributed Programming at Uppsala University.
VT 2023
  
## Author
By Ture Hassler
 
 
