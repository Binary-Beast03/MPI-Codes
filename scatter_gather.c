#include<stdio.h>
#include<time.h>
#include<stdlib.h>
#include"mpi.h"
int main(int argc, char **argv)
{
    MPI_Init(&argc,&argv);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    float a[50],average,total;
    float send[20],receive[20],receive1[20];
    if(world_size!=4)
    {
        printf("This program is for 4 processes only \n");
        MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
    }
    if (world_rank==0)
    {
        // srand(time(0));
        for(int i=0;i<48;i++)
        {
            a[i]=(float)rand()/RAND_MAX;
            printf("send %d : %f\n",i,a[i]);
        }
        MPI_Scatter(a,12,MPI_FLOAT,&receive,12,MPI_FLOAT,0,MPI_COMM_WORLD);
    }
    else
        MPI_Scatter(NULL,12,MPI_FLOAT,&receive,12,MPI_FLOAT,0,MPI_COMM_WORLD);


    for(int i=0;i<12;i++)
    {
        total+=receive[i];
        printf("receive world rank = %d %d : %f\n",world_rank,i,receive[i]);
    }
    average=(float)total/12;

    
    if(world_rank==0)
       {
            MPI_Gather(&average,1,MPI_FLOAT,receive1,1,MPI_FLOAT,0,MPI_COMM_WORLD);
            // printf("worked here\n");
            total=0.0;
            for(int i=0;i<4;i++)
                total+=receive1[i];
            printf("average of all 48 elements is %f \n",(float)total/4);
       }

    else
        MPI_Gather(&average,1,MPI_FLOAT,NULL,0,MPI_FLOAT,0,MPI_COMM_WORLD);

    
    MPI_Finalize();



    
    
    return 0;

}