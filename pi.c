#include<stdio.h>
#include"mpi.h"
int main(int argc,char *argv)
{
    

    MPI_Init(&argc,&argv);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    

    int total_slice=20,i,lower=0,upper=1;
    float slice_width;
    // printf("Enter no of slices = ");
    // scanf("%d",&slice_no);
    if(total_slice%world_size!=0)
    {
        printf("slice no should be multiple of world size");
        MPI_Abort(MPI_COMM_WORLD, 1);
    }
    int root_rank=0;
    float x,integral;
    double sum=0.0,sum_all_process=0.0,pi=0.0,temp=0.0;
    double del_x=0.0;
    del_x=(double)1/total_slice;
    for (i=world_rank+1;i<=total_slice;i=i+world_size)
    {
        x=(double)(i-0.5)*(del_x);
        sum+=(double)(4.0/(1.0+x*x));
        // printf("world rank = %d sum = %lf i= %d x= %lf del_x=%f\n",world_rank,sum,i,x,del_x);
    }
    temp=del_x*sum;
    // printf("sum = %lf",sum);
    MPI_Reduce(&temp,&sum_all_process,1,MPI_DOUBLE,MPI_SUM,root_rank,MPI_COMM_WORLD);
    pi=sum_all_process;
    // pi=4*pi*del_x;
    // sum_all_process=sum_all_process+0.5+0.25;
    // sum_all_process=4*sum_all_process*del_x;
    if(world_rank==0)
    {

        printf("world rank = %d pi = %.16f\n",world_rank,pi);
    }
    MPI_Finalize();
    return 0;

}