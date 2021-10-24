#include<stdio.h>
#include"mpi.h"
int main(int argc, char **argv)
{
    MPI_Init(&argc,&argv);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

    int k_bar,i,j,q;
    int a[2][2]={{1,2},{3,4}};
    int b[2][2]={{1,2},{3,4}};
    int c[2][2]={};
    int temp=0,stage=0;

    q=2;
    while(stage<2)
    {
        for(i=0;i<2;i++)
        {
            k_bar=(i+stage)%q;
            for(j=0;j<2;j++)
            {
                temp+=a[0][i] * b[k_bar][j];

            }
        }
    }
}