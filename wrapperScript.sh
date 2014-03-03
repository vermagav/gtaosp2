#PBS -q cs6210
#PBS -l nodes=3:fourcore
#PBS -l walltime=00:10:00
#PBS -N MyJobName
OMPI_MCA_mpi_yield_when_idle=0
/opt/openmpi-1.4.3-gcc44/bin/mpirun --hostfile $PBS_NODEFILE -np 48 myMPIprog