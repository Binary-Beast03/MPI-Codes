#include <stdio.h>
#include "mpi.h"
int main(int argc, char **argv)
{
    MPI_Init(&argc,&argv);
    MPI_Status status;
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    int send,receive;
    if(world_rank==0)
    {
        send=2;
        MPI_Send(&send,1,MPI_INT,1,0,MPI_COMM_WORLD);
    }
    else if(world_rank==1)
    {
        MPI_Recv(&receive,1,MPI_INT,0,0,MPI_COMM_WORLD,&status);
        printf("Process 1 received number %d from process 0 /n",receive);
    }

    MPI_Finalize();

    return 0;

}