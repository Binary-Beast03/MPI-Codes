#include<stdio.h>
#include<stdlib.h>
#include"mpi.h"
#include <time.h>

double largest(double arr[], int n)
{
    int i;
    
    // Initialize maximum element
    double max = arr[0];
 
    // Traverse array elements from second and
    // compare every element with current max 
    for (i = 1; i < n; i++)
        if (arr[i] > max)
            max = arr[i];
 
    return max;
}

int main(int argc, char **argv )
{
    clock_t t;
    t = clock();
    MPI_Init(&argc,&argv);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    int size=4;
    int max_size=1000;
    FILE *fp;
    fp=fopen("matrix_multi_paralle.csv","w");
    // while(size<max_size)
    // {
    int r,c,i,j,k,count=0,upper,lower;
    // double a[size][size], b[size][size];
    // double** a =(double**)malloc(r*sizeof(double*));
    // for (i = 0; i < size; i++)
    //     a[i] = (double*)malloc(size * sizeof(double));

    // double** b =(double**)malloc(r*sizeof(double*));
    // for (i = 0; i < size; i++)
    //     b[i] = (double*)malloc(size * sizeof(double));
    // int a[4][4]={{1,2,3,4},{5,6,7,8},{9,1,2,3},{4,5,6,7}};
    // int b[4][4]={{1,2,3,4},{5,6,7,8},{9,1,2,3},{4,5,6,7}};
    int mul[4][4];
    int sum=0;
    double* a1;
    double* b1;
    double* mul1;
    double* receive;
    a1 = (double*)malloc(size*size*sizeof(double));
    b1 = (double*)malloc(size*size*sizeof(double));
    mul1 = (double*)malloc(size*size*sizeof(double));
    receive = (double*)malloc(size*size*sizeof(double));
    r=size;
    c=size;
    lower=1;
    upper=1000;

    if(world_size!=4)
    {
        printf("This program is for 4 processes only \n");
        MPI_Abort(MPI_COMM_WORLD,EXIT_FAILURE);
    }

    if(world_rank==0)
    {
        // printf("enter the number of row (should be multiple of 4) = ");    
        // fscanf("%d",&r);
        // printf("enter the number of column (should be multiple of 4) =");    
        // fscanf("%d",&c);
        // printf("enter the first matrix element=\n");    
        for(i=0;i<r;i++)
        {    
            for(j=0;j<c;j++)    
            {
                // scanf("%d",&a[i][j]);
                // a[i][j]=(rand() % (upper - lower + 1)) + lower;
                a1[count]=a[i][j];
                // printf("count= %d  ,in a1= %d ,in array a= %d \n",count,a1[count],a[i][j]);
                // printf("%d ",a[i][j]);
                count++;
            }
            // printf("\n");
            // count++;

        }
        

        
        // printf("enter the second matrix element=\n");    
    //     for(i=0;i<r;i++)
    //     {    
    //         for(j=0;j<c;j++)    
    //         {
    //             // scanf("%d",&b[i][j]);
    //             b[i][j]=(rand() % (upper - lower + 1)) + lower;
    //             // printf("%d ",b[i][j]);
                
    //         }
    //         // printf("\n");
            
    //     }
    
    }

    MPI_Bcast(b,r*c,MPI_INT,0,MPI_COMM_WORLD);

    if(world_rank==0)
        MPI_Scatter(a1,r,MPI_INT,receive,r,MPI_INT,0,MPI_COMM_WORLD);

    else
         MPI_Scatter(NULL,r,MPI_INT,receive,r,MPI_INT,0,MPI_COMM_WORLD);
    count=0;
    for(i=0;i<r;i++)
    {
        for(j=0;j<c;j++)    
        {        
            
            sum = sum + a1[j] * b[j][i];
            
            
            // for(k=0;k<c;k++)    
            // {    
            //     mul1[j]+=receive[k]*b[k][j];
            //     // printf(" world rank = %d received = %d mul[%d] = %d b= %d \n",world_rank,receive[k],j,mul1[j],b[k][j]);
                    
            // }
            // count++;
            // printf("world rank = %d row = %d col = %d k = %d value = %d \n",world_rank,i,j,k,mul1[count]);
            // printf("\n");       
        }
        mul1[i]=sum;
        sum=0;
         
    }
    for(i=0;i<c;i++)
        printf("%f \t",mul1[i]);
    printf("\n");
    MPI_Gather(mul1,size,MPI_INT,mul,size,MPI_INT,0,MPI_COMM_WORLD);
    // for(i=0;i<r;i++)
    //     printf("world rank = %d receive = %d ",world_rank,receive[i]);
    if(world_rank==0)
    for(i=0;i<r;i++)
    {   for(j=0;j<c;j++)
        printf("%d \t",mul[i][j]);
    printf("\n");
    }
    t = clock() - t;
    double t_store[4];
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    if (world_rank == 0)
    {
        MPI_Gather(&time_taken,1,MPI_DOUBLE,t_store,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
        int n = sizeof(t_store)/sizeof(t_store[0]);
        printf("size = %d Time taken = %f \n",size,largest(t_store,n));
    }
    else
    {
        MPI_Gather(&time_taken,1,MPI_DOUBLE,NULL,0,MPI_DOUBLE,0,MPI_COMM_WORLD);
    }
    free(mul1);
    free(a1);
    free(b1);
    free(receive);
    // free(a);
    // free(b);
    // size=size+1;
    // }
    MPI_Finalize();
    // printf("time taken by world = %d is %f \n",world_rank,time_taken);
    }