#include <stdio.h>
#include<stdlib.h>
#include"mpi.h"
int main(int argc, char **argv)
{
    MPI_Init(&argc,&argv);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int x,y,z;
    MPI_Status status;
    switch(world_rank)
    {
        case 0: x=0; y=1; z=2;
                MPI_Bcast(&x,1,MPI_INT,0,MPI_COMM_WORLD);
                MPI_Send(&y,1,MPI_INT,2,43,MPI_COMM_WORLD);
                printf("After Send \n");
                printf("z = %d\n",z);
                MPI_Bcast(&z,1,MPI_INT,1,MPI_COMM_WORLD);
                printf("z = %d\n",z);
                // printf("Inside break\n");
                // printf("world rank = %d x=%d, y=%d, z=%d\n",world_rank,x,y,z);
                break;
        
        case 1: x=3; y=69; z=5;
                MPI_Bcast(&x,1,MPI_INT,0,MPI_COMM_WORLD);
                MPI_Bcast(&y,1,MPI_INT,1,MPI_COMM_WORLD);
                // printf("Inside break\n");
                // printf("world rank = %d x=%d, y=%d, z=%d\n",world_rank,x,y,z);
                break;
        
        case 2: x=6;y=7;z=8;
                MPI_Bcast(&z,1,MPI_INT,0,MPI_COMM_WORLD);
                MPI_Recv(&x,1,MPI_INT,0,43,MPI_COMM_WORLD,&status);
                MPI_Bcast(&y,1,MPI_INT,1,MPI_COMM_WORLD);
                // printf("Inside break\n");
                // printf("world rank = %d x=%d, y=%d, z=%d\n",world_rank,x,y,z);
                break;


    }
    if(world_rank<3)
        printf("world rank = %d x=%d, y=%d, z=%d\n",world_rank,x,y,z);

    MPI_Finalize();
    return 0;
}

