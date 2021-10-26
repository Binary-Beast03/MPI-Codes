#include<stdio.h>
#include"mpi.h"
/**
 * brief Illustrates how to create a vector MPI datatype.
 * details This program is meant to be run with 2 processes: a sender and a
 * receiver. These two MPI processes will exchange a message made of three
 * integers. On the sender, that message is in fact the middle column of an
 * array it holds, which will be represented by an MPI vector.
 *
 *
 *     Full array          What we want
 *                            to send
 * +-----+-----+-----+  +-----+-----+-----+
 * |  0  |  1  |  2  |  |  -  |  1  |  -  |
 * +-----+-----+-----+  +-----+-----+-----+
 * |  3  |  4  |  5  |  |  -  |  4  |  -  |
 * +-----+-----+-----+  +-----+-----+-----+
 * |  6  |  7  |  8  |  |  -  |  7  |  -  |
 * +-----+-----+-----+  +-----+-----+-----+
 *
 * How to extract a column with a vector type:
 *
 *                 distance between the
 *            start of each block: 3 elements
 *          <---------------> <--------------->
 *         |                 |                 |
 *      start of          start of          start of
 *      block 1           block 2           block 3
 *         |                 |                 |
 *         V                 V                 V
 *   +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *   | ses  |  -  |  4  |  -  |  -  |  7  |  -  |
 *   +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *          <--->             <--->             <--->
 *         block 1           block 2           block 3
 * 
 * Block length: 1 element
 * Element: MPI_INT
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
        case 0: {int arr[3][3]={0,1,2,3,4,5,6,7,8};
                MPI_Datatype row_send;
                MPI_Type_vector(3,3,3,MPI_INT,&row_send);
                MPI_Type_commit(&row_send);
                MPI_Send(&arr[0][0],1,row_send,1,99,MPI_COMM_WORLD);
                break;}
    
        case 1: {int receive[10];
                MPI_Status status;
                MPI_Recv(&receive,10,MPI_INT,0,99,MPI_COMM_WORLD,&status);
                printf("%d %d %d ",receive[0],receive[1],receive[3]);
                break;}
    
    }
    MPI_Finalize();
    return 0;   
}