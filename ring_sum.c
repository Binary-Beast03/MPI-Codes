#include<stdio.h>
#include"mpi.h"
int main(int argc , char **argv)
{
    MPI_Init(&argc , &argv);
    MPI_Status status;
    MPI_Request request;
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    int send=0,receive=0;
    int right,left,sum;
    left=world_rank-1;
    right=world_rank+1;
    // printf("current world rank=%d\n",world_rank);
    if(world_rank-1<0)
    {
        left=world_size-1;
    }

    if(world_rank+1==world_size)
    {
        right=0;
    }

    send = world_rank;
    sum = 0;
    // do{
    printf("world rank = %d",world_rank);
    MPI_Isend(&send,1,MPI_INT,right,0, MPI_COMM_WORLD,&request); 
    MPI_Recv(&receive,1, MPI_INT, left, 0, MPI_COMM_WORLD, &status);
    // printf("right = %d left = %d ,receive= %d \n",right,left,receive);
    // MPI_Wait(&request,&status);
    printf("right = %d left = %d ,send= %d,receive= %d \n",right,left,send,receive);
    // send=receive;
    // if (right==world_rank)
    //     sum=receive+world_rank;
    sum=send+receive;
    // }while(send!=world_rank);
    // if (send==world_rank+1)
    // {
    //     sum=send+world_rank;
    // }

    printf("process %d sum = %d \n", world_rank, sum);

  MPI_Finalize();


}