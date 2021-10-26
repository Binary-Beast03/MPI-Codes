/* send_col_to_row.c -- send column 1 of a matrix on process 0 to row 1
 *     on process 1.
 *
 * Input: none
 * Output: The row received by process 1.
 *
 * Note:  This program should only be run with 2 processes
 *
 * See Chap 6., pp. 98 & ff in PPMPI
 */
#include <stdio.h>
#include "mpi.h"

int main(int argc, char* argv[]) {
    int p;
    int my_rank;
    float A[10][10];
    MPI_Status status;
    MPI_Datatype column_mpi_t;
    int i, j;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Type_vector(10, 1, 10, MPI_FLOAT, &column_mpi_t);
    MPI_Type_commit(&column_mpi_t);

    if (my_rank == 0) {
        for (i = 0; i < 10; i++)
            for (j = 0; j < 10; j++)
                A[i][j] = (float) j;
        MPI_Send(&(A[0][2]), 1, column_mpi_t, 1, 0,
                 MPI_COMM_WORLD);
    } else { /* my_rank = 1 */
        for (i = 0; i < 10; i++)
            for (j = 0; j < 10; j++)
                A[i][j] = 0.0;
        MPI_Recv(A, 10, MPI_FLOAT, 0, 0,
                 MPI_COMM_WORLD, &status);
        for (j = 0; j < 10; j++)
            printf("%3.1f ", A[0][j]);
        printf("\n");
    }

    MPI_Finalize();
}  /* main */