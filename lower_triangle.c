#include<stdio.h>
#include"mpi.h"
/**
 * brief Illustrates how to create an indexed MPI datatype.
 * details This program is meant to be run with 2 processes: a sender and a
 * receiver. These two MPI processes will exchange a message made of six
 * integers. On the sender, that message is in fact the lower triangle of an
 * array it holds, which will be represented by an MPI indexed type.
 *
 *
 *     Full array          What we want
 *                            to send
 * +-----+-----+-----+  +-----+-----+-----+
 * |  0  |  1  |  2  |  |  0  |  -  |  -  |
 * +-----+-----+-----+  +-----+-----+-----+
 * |  3  |  4  |  5  |  |  3  |  4  |  -  |
 * +-----+-----+-----+  +-----+-----+-----+
 * |  6  |  7  |  8  |  |  6  |  7  |  8  |
 * +-----+-----+-----+  +-----+-----+-----+
 *
 * How to extract the lower triangle with an indexed type:
 *
 *   
 *        +---------------------------- displacement for
 *        |                            block 2: 6 elements
 *        |                                   |
 *        +---------- displacement for        |
 *        |          block 2: 3 elements      |
 *        |                 |                 |
 *  displacement for        |                 |
 * block 1: 0 elements      |                 |
 *        |                 |                 |
 *        V                 V                 V
 *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *        |  0  |  -  |  -  |  3  |  4  |  -  |  6  |  7  |  8  |
 *        +-----+-----+-----+-----+-----+-----+-----+-----+-----+
 *         <--->             <--------->       <--------------->
 *        block 1              block 2              block 3
 *       1 element            2 elements           3 elements
 * 
 * */
int main(int argc, char** argv)
{
    MPI_Init(&argc,&argv);
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
        case 0: {MPI_Datatype lower_triangle;
                int block_lenght[3]={1,2,3};
                int displacement[3]={0,3,6};
                MPI_Type_indexed(3,block_lenght,displacement,MPI_INT,&lower_triangle);
                MPI_Type_commit(&lower_triangle);
                int arr[3][3]={0,1,2,3,4,5,6,7,8};
                MPI_Ssend(arr,1,lower_triangle,1,99,MPI_COMM_WORLD);
                break;}

        case 1: {int receive[6];
                MPI_Status status;
                MPI_Recv(receive,6,MPI_INT,0,99,MPI_COMM_WORLD,&status);
                printf("Received elements are \n" );
                for(int i = 0; i<sizeof(receive)/sizeof(int); i++)
                {
                    printf("%d \n",receive[i]);
                }
                break;}

    
    }
    MPI_Finalize();
    return 0;
}