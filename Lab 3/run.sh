#!/bin/bash -l

#SBATCH -M snowy
#SBATCH -A uppmax2023-2-13
#SBATCH -t 00:25:00
#SBATCH -p node
#SBATCH -n 16

module load gcc openmpi

echo "Run program"
echo "n=16"

mpirun --bind-to none -n 16 ./part_2 100000 200 200




echo "All done!"