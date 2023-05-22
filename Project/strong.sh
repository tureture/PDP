#!/bin/bash -l

#SBATCH -M snowy
#SBATCH -A uppmax2023-2-13
#SBATCH -t 00:30:00
#SBATCH -p node
#SBATCH -n 64
#SBATCH -o strong_scaling_%j.txt

echo "Running strong scaling test"

module load gcc openmpi
export OMPI_MCA_btl_openib_allow_ib=1

mpirun --bind-to none -n 1 ./mc 100000 out.txt 0 0 
mpirun --bind-to none -n 2 ./mc 100000 out.txt 0 0 
mpirun --bind-to none -n 4 ./mc 100000 out.txt 0 0 
mpirun --bind-to none -n 8 ./mc 100000 out.txt 0 0
mpirun --bind-to none -n 16 ./mc 100000 out.txt 0 0
mpirun --bind-to none -n 32 ./mc 100000 out.txt 0 0
mpirun --bind-to none -n 64 ./mc 100000 out.txt 0 0



echo "All done!"