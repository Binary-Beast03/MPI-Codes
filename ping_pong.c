#include<stdio.h>
#include"mpi.h"

double largest(double arr[], int n);// to find out maximum time

int main(int argc, char **argv)
{
    MPI_Init(&argc,&argv);
    FILE *fp;
    MPI_Status status;
    int min_size = 0,max_size = 8;
    int incr = 1, size,pass;
    float x[100];
    int world_rank;
    double start;
    double end;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    char buffer[100];
    snprintf(buffer, sizeof(char) * 100, "ping pong (nodes = %i).csv", world_size);
    fp=fopen(buffer,"w");
    
    if (world_size != 2) 
    {
      printf("World size must be two for %s\n", argv[0]);
      MPI_Abort(MPI_COMM_WORLD, 1);
    }
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
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

    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    double t_store[world_size];
    double time_taken = end - start;
    if (world_rank == 0)
    {
        MPI_Gather(&time_taken,1,MPI_DOUBLE,t_store,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
        int n = sizeof(t_store)/sizeof(t_store[0]);
        printf("size = %d Time taken = %f \n",size,largest(t_store,n));
        fprintf(fp,"%d\t%f",size,largest(t_store,n));
        fprintf(fp,"\n");
    }
    else
    {
        MPI_Gather(&time_taken,1,MPI_DOUBLE,NULL,0,MPI_DOUBLE,0,MPI_COMM_WORLD);
    }

















    
    

   
    
    MPI_Finalize();

    return 0;
}


double largest(double arr[], int n)
{
    int i;
    
    // Initialize maximum element
    double max = arr[0];
 

    for (i = 1; i < n; i++)
        if (arr[i] > max)
            max = arr[i];
 
    return max;
}

