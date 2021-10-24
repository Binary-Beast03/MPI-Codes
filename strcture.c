#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include"mpi.h"
struct person_info
{
    int age;
    double height;
    char name[10];
};

int main(int argc, char **argv)
{
    MPI_Init(&argc,&argv);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);

    if(world_size!=2)
    {
        printf("This program is design for 2 processes \n");
        MPI_Abort(MPI_COMM_WORLD,1);
    }

    MPI_Datatype person_type;                                           //custom datatype name 
    int length[3]={1,1,10};                                             //array for block length
    MPI_Aint displacement[3];                                           //array to strore displacement
    MPI_Aint base_address;                                              // base address
    struct person_info dummy_person;                                    //object initialize for gathering address and displacement    
    MPI_Get_address(&dummy_person,&base_address);
    MPI_Get_address(&dummy_person.age,&displacement[0]);
    MPI_Get_address(&dummy_person.height,&displacement[1]);
    MPI_Get_address(&dummy_person.name[0],&displacement[2]);
    displacement[0]=MPI_Aint_diff(displacement[0],base_address);
    displacement[1]=MPI_Aint_diff(displacement[1],base_address);
    displacement[2]=MPI_Aint_diff(displacement[2],base_address);

    MPI_Datatype types[3]={MPI_INT,MPI_DOUBLE,MPI_CHAR};                //custom datatype for types to store datatypes

    MPI_Type_create_struct(3,length,displacement,types,&person_type);

    MPI_Type_commit(&person_type);                                      //declare user defined datatype to mpi
    
    if(world_rank==0)
    {
        struct person_info send;
        send.age=21;
        send.height=1.66;
        strcpy(send.name,"Anurag");                                //sprintf() and len+1 for 0\ are used                  
        MPI_Send(&send,1,person_type,1,0,MPI_COMM_WORLD);                

    }
    if(world_rank==1)
    {
        struct person_info receive;
        MPI_Recv(&receive,1,person_type,0,0,MPI_COMM_WORLD,MPI_STATUS_IGNORE);
        printf("data received name = %s , age %d , height =%lf \n" ,receive.name,receive.age,receive.height);
    }

    MPI_Finalize();
    return 0;
}