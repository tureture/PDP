#!/bin/bash -l

#SBATCH -M snowy
#SBATCH -A uppmax2023-2-13
#SBATCH -t 00:25:00
#SBATCH -p node
#SBATCH -n 2

echo "Run program"

mpirun --bind-to none -n 2 ./array_sect




echo "All done!"