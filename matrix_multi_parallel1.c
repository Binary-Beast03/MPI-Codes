#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"
#include <time.h>
#define N 4

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


int main(int argc, char **argv)
{   
    clock_t t;    
    int i, j, k, rank, size, tag = 99,sum = 0,r,c;
    r=c=size=4;
    int upper,lower;

    lower=1;
    upper=10000;
    
    MPI_Init(&argc, &argv);
    FILE *fp;
    fp=fopen("matrix_multi_parallel.csv","w");
    while(size!=3004)
    {
    t = clock();


    int (*a)[size] = malloc(size * sizeof(*a));
    int (*b)[size] = malloc(size * sizeof(*b));
    int (*mul)[size] = malloc(size * sizeof(*mul));
    int *aa,*cc;
    aa=(int*)malloc(size*sizeof(int));
    cc=(int*)malloc(size*sizeof(int));

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    if(world_rank==0)
    {
            for(i=0; i<r; i++)
            {
                    for(j=0; j<c; j++)
                    {
                        a[i][j]=(rand() % (upper - lower + 1)) + lower;
                        
                       
                    }
            }

            for(i=0; i<r; i++)
            {
                    for(j=0; j<c; j++)
                    {
                        b[i][j]=(rand() % (upper - lower + 1)) + lower;
                        
                        
                    }
            }
    }

         
    MPI_Scatter(a, N*N/size, MPI_INT, aa, N*N/size, MPI_INT,0,MPI_COMM_WORLD);


    MPI_Bcast(b, N*N, MPI_INT, 0, MPI_COMM_WORLD);

          
          for (i = 0; i < N; i++)
            {
                    for (j = 0; j < N; j++)
                    {
                            sum = sum + aa[j] * b[j][i];           
                    }
                    cc[i] = sum;
                    sum = 0;
            }

    MPI_Gather(cc, N*N/size, MPI_INT, mul, N*N/size, MPI_INT, 0, MPI_COMM_WORLD);
    t = clock() - t;
    double t_store[world_size];
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
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
    

    size+=4;
    // free(aa);
    // free(cc);
    free(a);
    free(b);
    free(mul);
    fclose(fp);
    }      
    MPI_Finalize();


}

