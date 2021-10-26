#include<stdio.h>
#include"mpi.h"
/**
*We have 3 variable a,b,c stored in non-contigious way
*using MPI_Pack and MPI_Unpack we transfer data form process 0
* to process 1
**/

int main(int argc , char** agrv)
{
    MPI_Init(&argc,&agrv);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

    if (world_size!=2)
    {
        printf("This process is meant to run on 2 cores /n");
        MPI_Abort(MPI_COMM_WORLD,69);
    }

    switch(world_rank)
    {
        case 0: {int a,b;
                float c;
                int position;
                char buffer[20];
                a=1;b=2;c=3.14;
                position=0; //now pack the data into buffer at position 0
                MPI_Pack(&a,1,MPI_INT,buffer,20,&position,MPI_COMM_WORLD);
                MPI_Pack(&b,1,MPI_INT,buffer,20,&position,MPI_COMM_WORLD);
                MPI_Pack(&c,1,MPI_FLOAT,buffer,20,&position,MPI_COMM_WORLD);
                MPI_Bcast(buffer,20,MPI_PACKED,0,MPI_COMM_WORLD);
                break;}//special datatype called MPI_packed which tells mpi that buffer using pack function

        case 1: {char buffer[20];
                int a_rev,b_rev;
                float c_rev;
                int position;
                MPI_Bcast(buffer,20,MPI_PACKED,0,MPI_COMM_WORLD);
                position=0;
                MPI_Unpack(buffer,20,&position,&a_rev,1,MPI_INT,MPI_COMM_WORLD);
                MPI_Unpack(buffer,20,&position,&b_rev,1,MPI_INT,MPI_COMM_WORLD);
                MPI_Unpack(buffer,20,&position,&c_rev,1,MPI_FLOAT,MPI_COMM_WORLD);
                printf("data received \n");
                printf("a = %d b = %d c = %f \n",a_rev,b_rev,c_rev);
                break;}


    }

    MPI_Finalize();
    return 0;
    

}