#include<stdio.h>
#include"mpi.h"
int main(int argc, char **argv)
{
    MPI_Init(&argc,&argv);
    MPI_Status status;
    int p,my_rank, min_size = 0,max_size = 8;
    int incr = 1, size,pass;
    float x[100];
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    if (world_size != 2) 
    {
      printf("World size must be two for %s\n", argv[0]);
      MPI_Abort(MPI_COMM_WORLD, 1);
    }

    if (world_rank == 0) 
    {
      for (size=min_size;size<=max_size; size=size+incr) 
    {
      for (pass = 0; pass < 2; pass++) 
    {
      MPI_Send(x, size, MPI_FLOAT,1,0,MPI_COMM_WORLD);
      MPI_Recv(x, size, MPI_FLOAT,1,0,MPI_COMM_WORLD,&status);
      
    }
    printf("%d Ping Pong complete\n", size+1);
    }
    } 
    else 
  {
  for (size=min_size;size<=max_size; size=size+incr) 
  {
    for (pass = 0; pass < 2; pass++) 
    {
      
      MPI_Recv(x, size, MPI_FLOAT,0,0,MPI_COMM_WORLD,&status);
      MPI_Send(x, size, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
      // printf("%d\n", size);

    }
  }
  }

















    
    // int tag;
    // int limit=0;
    // tag=0;
    // int send,receive;
    // int ping_pong_count=0;
    // int partner_rank = (world_rank + 1) % 2;
    // send=2;
    // while (ping_pong_count < limit) 
    // {
    // if (world_rank == ping_pong_count % 2) 
    // {
    //   ping_pong_count++;
    //   MPI_Send(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD);
    //   printf("%d sent and incremented ping_pong_count %d to %d\n",
    //          world_rank, ping_pong_count, partner_rank);
    // } else 
    // {
    //   MPI_Recv(&ping_pong_count, 1, MPI_INT, partner_rank, 0, MPI_COMM_WORLD,
    //            MPI_STATUS_IGNORE);
    //   printf("%d received ping_pong_count %d from %d\n",
    //          world_rank, ping_pong_count, partner_rank);
    // }
    // }

   
    
    MPI_Finalize();

    return 0;
}