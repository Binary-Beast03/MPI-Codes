
#include<stdio.h>
#include"mpi.h"

void get_data_bcast(int* a2_ptr, int* b2_ptr, int* c2_ptr,int world_rank,int world_size);
void Get_data(float* a_ptr, float* b_ptr, int* n_ptr, int my_rank, int p);
int main(int argc, char** argv)
{
    
    MPI_Init(&argc,&argv);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    MPI_Status status;
    float a,b;
    int c;
    int a2,b2,c2;
    Get_data(&a,&b,&c,world_rank,world_size);
    get_data_bcast(&a2,&b2,&c2,world_rank,world_size);
    printf("a = %f form rank = %d\n",a,world_rank);
    printf("b = %f form rank = %d\n",b,world_rank);
    printf("c = %d form rank = %d\n",c,world_rank);
    printf("\n");
    printf("a2 = %d from rank = %d\n",a2,world_rank); 
    printf("b2 = %d from rank = %d\n",b2,world_rank);
    printf("c2 = %d from rank = %d\n",c2,world_rank);
    MPI_Finalize();

    
    
    
    return 0;

}

void Get_data(
         float*  a_ptr    /* out */, 
         float*  b_ptr    /* out */, 
         int*    n_ptr    /* out */,
         int     my_rank  /* in  */, 
         int     p        /* in  */) {

    int source = 0;    /* All local variables used by */
    int dest;          /* MPI_Send and MPI_Recv       */
    int tag;
    MPI_Status status;

    if (my_rank == 0){
        printf("Enter a, b, and n\n");
        scanf("%f %f %d", a_ptr, b_ptr, n_ptr);
        for (dest = 1; dest < p; dest++){
            tag = 0;
            MPI_Send(a_ptr, 1, MPI_FLOAT, dest, tag, 
                MPI_COMM_WORLD);
            tag = 1;
            MPI_Send(b_ptr, 1, MPI_FLOAT, dest, tag, 
                MPI_COMM_WORLD);
            tag = 2;
            MPI_Send(n_ptr, 1, MPI_INT, dest, tag, 
                MPI_COMM_WORLD);
        }
    } else {
        tag = 0;
        MPI_Recv(a_ptr, 1, MPI_FLOAT, source, tag, 
            MPI_COMM_WORLD, &status);
        tag = 1;
        MPI_Recv(b_ptr, 1, MPI_FLOAT, source, tag, 
            MPI_COMM_WORLD, &status);
        tag = 2;
        MPI_Recv(n_ptr, 1, MPI_INT, source, tag, 
                MPI_COMM_WORLD, &status);
    }
}

void get_data_bcast(int* a2_ptr, int* b2_ptr, int* c2_ptr,int world_rank,int world_size)
{
  
    int source=0;
    int dest;
    int tag;
    MPI_Status status;
    if (world_rank==0)
    {
        printf("Enter a,b and c \n");
        scanf("%d %d %d",a2_ptr,b2_ptr,c2_ptr);
        
    }
        MPI_Bcast(a2_ptr,1,MPI_INT,source,MPI_COMM_WORLD);
        MPI_Bcast(b2_ptr,1,MPI_INT,source,MPI_COMM_WORLD);
        MPI_Bcast(c2_ptr,1,MPI_INT,source,MPI_COMM_WORLD);
    // else
    // {
    //     MPI_Recv(a2_ptr,1,MPI_INT,source,tag,MPI_COMM_WORLD,&status);
    //     MPI_Recv(b2_ptr,1,MPI_INT,source,tag,MPI_COMM_WORLD,&status);
    //     MPI_Recv(c2_ptr,1,MPI_INT,source,tag,MPI_COMM_WORLD,&status);
    // }
}