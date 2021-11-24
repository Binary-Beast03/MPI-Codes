#include <stdio.h>
#include"mpi.h"
#include<stdlib.h>

void initialize_mat         (int world_size, int world_rank, int N, float **local_tab,float **local_tab1,int nb_rows);
void laplace                (int nb_rows,int N,float epsilon,float **local_tab,float **local_tab1,int world_size,int world_rank);
void update_matrix          (float **local_tab, int nb_rows, int N, int world_size, int world_rank);
void save_file_final_matrix (char filename[], int world_rank, float **local_tab, int N , int world_size, int nb_rows);
void bounday_cond           (float **local_tab, int world_rank, int world_size, int N, int nb_rows,float slope);
void print_matrix           (int world_rank, int N, float **local_tab, int nb_rows);
/*-------------------MAIN-------------------------*/

int main(int argc, char **argv)
{
    MPI_Init(&argc,&argv);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);
    MPI_Barrier(MPI_COMM_WORLD);

    int N = 8; //size of square matrices
    int nb_rows = (N/world_size)+2;
    int max_iterations = 2000;
    float upper_T = 100.0; // For linear temp scaling
    float lower_T = 0.0;
    float slope = (upper_T - lower_T) /N ;
    float epsilon = 0.001;
    int i,j;
    // float (*local_tab)[N] = malloc(N * sizeof(*local_tab));
    float* local_tab[N];
    for (i = 0; i < N; i++)
        local_tab[i] = (float*)malloc(N * sizeof(float));

    // float (*local_tab1)[N] = malloc(N * sizeof(*local_tab1));

    float* local_tab1[N];
    for (i = 0; i < N; i++)
        local_tab1[i] = (float*)malloc(N * sizeof(float));

    initialize_mat(world_size,world_rank,N,local_tab,local_tab1,nb_rows);
    
    bounday_cond(local_tab,world_rank,world_size,N,nb_rows,slope);
    
    update_matrix(local_tab,nb_rows,N,world_size,world_rank);
    // print_matrix(world_rank,N,local_tab,nb_rows);
    laplace(nb_rows,N,epsilon,local_tab,local_tab,world_size,world_rank);
    // print_matrix(world_rank,N,local_tab,nb_rows);
    save_file_final_matrix("laplace_parallel.csv",world_rank,local_tab,N,world_size,nb_rows);
    MPI_Finalize();
    return 0;
}

/*-------------------MAIN-------------------------*/

void initialize_mat(int world_size, int world_rank, int N, float **local_tab,float **local_tab1,int nb_rows)
{
    int i;//i for row
    int j;//j for column 
    for(i = 0; i<nb_rows; i++)
    {
        for(j = 0; j<N; j++)
        {
            if ((i == 0) || (i == nb_rows-1))
                {local_tab[i][j] = -1;
                local_tab1[i][j] = -1;}
            else
                {local_tab[i][j] = 0;
                local_tab1[i][j] = 0;}
        }
    }

    // for(i = 0; i<nb_rows; i++)
    // {
    //     for(j = 0; j<N; j++)
    //     {
    //        printf("%2.f\t",*(local_tab+j+i*N));
    //     }
    //     printf("\n");
    // }
}

void laplace(int nb_rows,int N,float epsilon,float **local_tab,float **local_tab1,int world_size,int world_rank)
{
    int i,j,k;
    int max_iterations = 2000;
    int x = 1;
    double U = 0.0;
    for (int k = 0; k < max_iterations; k++){
        if(world_rank==0)
            x=2;
        for (int i = x; i < nb_rows-1; i++ ){
            for(int j = 0; j < N; j++)
                    {
                        local_tab[i][j]  = 0.25*(local_tab[i + 1][j] + local_tab[i][j + 1] +
                        local_tab[i-1][j] + local_tab[i][j - 1]);   

                    }

        }
    update_matrix(local_tab,nb_rows,N,world_size,world_rank);
    }
    
    // for(i = 0; i<nb_rows; i++)
    // {
    //     for(j = 0; j<N; j++)
    //     {
    //        printf("%2.f\t",local_tab[i][j]);
    //     }
    //     printf("\n");
    // }

}

void update_matrix (float **local_tab, int nb_rows, int N, int world_size, int world_rank)
{
    MPI_Status status;

    float *top_row = (float*)malloc((N)*sizeof(float));
    if (top_row == NULL) { exit(-1); }

    float *bottom_row = (float*)malloc((N)*sizeof(float));
    if (bottom_row == NULL) { exit(-1); }

    float *top_rowR = (float*)malloc((N)*sizeof(float));
    if (top_rowR == NULL) { exit(-1); }

    float *bottom_rowR = (float*)malloc((N)*sizeof(float));
    if (bottom_rowR == NULL) { exit(-1); }

    int i,j;
    // printf("\n\n\n");
    // printf("world rank = %d \n",world_rank);
    // for(i = nb_rows-1; i<nb_rows; i++)
    // {
    //     for(j = 0; j<N; j++)
    //     {
    //        printf("%2.f\t",local_tab[i][j]);
    //     }
    //     printf("\n");
    // }

    
    /* ---- SENDING ---- */
    if (world_rank != 0)
    {
        for(i=0;i<N-1;i++){
            top_row[i] = local_tab[1][i];
            }
        
        MPI_Send(top_row, N-1, MPI_FLOAT, world_rank-1, 1, MPI_COMM_WORLD); //2nd top row send 
    }
    if (world_rank != world_size-1)
    { 
        for(i=0;i<N-1;i++)
            bottom_row[i] = local_tab[nb_rows-2][i];

        MPI_Send(bottom_row, N-1, MPI_FLOAT, world_rank+1, 2, MPI_COMM_WORLD); //last 2nd row send

    }
    /* ---- RECEIVING---- */
    if (world_rank != 0) //swapped rows
    {
        MPI_Recv(top_rowR, N-1, MPI_FLOAT, world_rank-1, 2, MPI_COMM_WORLD, &status); //top row receive
        for(i=0;i<N-1;i++){
            local_tab[0][i] = top_rowR[i];
            // printf("%f ",local_tab[0][i]);
            }
    }
    if (world_rank != world_size-1) 
        MPI_Recv(bottom_rowR, N-1, MPI_FLOAT, world_rank+1, 1, MPI_COMM_WORLD, &status); //bottom row receive 
        for(i=0;i<N-1;i++)
            local_tab[nb_rows-1][i] = bottom_rowR[i];
    // printf("world rank = %d \n",world_rank);
    // for(i = 0; i<nb_rows; i++)
    // {
    //     for(j = 0; j<N; j++)
    //     {
    //             printf("%f \t",local_tab[i][j]);
    //     }
    //     printf("\n");
    // }
    

}


void save_file_final_matrix (char filename[], int world_rank, float **local_tab, int N , int world_size, int nb_rows)
{
    FILE *f;
    int i,j;
    int r =N/world_size;
    int test=16;
    
    float* temp_matrix[r];
        for (i = 0; i < r; i++)
            temp_matrix[i] = (float*)malloc(N * sizeof(float)); // Check if the memory has been well allocated

    float* final_matrix[N];
    for (i = 0; i < N; i++)
        final_matrix[i] = (float*)malloc(N * sizeof(float));
    

    int root_id = 0 ; // rank of the processor responsible of gathering the data
    // printf("\n \n Matrix printed by me: %d \n\n", world_rank);
    for(i = 1; i<nb_rows-1; i++)
    {
        for(j = 0; j<N; j++)
        {
            // printf(" %.2f", local_tab[i][j]); // Change ".2f" to "%d" for more clarity (test mode, without laplace calculation)
            temp_matrix[i-1][j] = local_tab[i][j];
            // printf(" %.2f ", temp_matrix[i-1][j]);
            // printf("i = %d j = %d ",i-1,j);
            //printf(" %d",(int) *(local_tab+j+i*N));
        }
        // printf("\n");
    }
    if(world_rank == 0)
    {
        float final_matrix1[N][N];
        MPI_Gather(temp_matrix, test, MPI_FLOAT, final_matrix1, test, MPI_FLOAT, root_id, MPI_COMM_WORLD );
        for(i=0;i<N;i++)
        {
            for(j=0;j<N;j++)
            {
                printf("%2.f ",final_matrix1[i][j]);
            }
        }
    }

    else
        MPI_Gather(temp_matrix, N*(N/world_size), MPI_FLOAT, NULL, 0, MPI_FLOAT, root_id, MPI_COMM_WORLD );

  
    if (world_rank == 0)
    {
        if ((f = fopen (filename, "w")) == NULL) { perror ("matrix_save: fopen "); }
        for (i = 0; i<N; i++)
        {
            for (j=0; j<N; j++)
            {
                fprintf (f, "%f ", final_matrix[i][j]); // Change "%f" to "%d" for more clarity (test mode, without laplace calculation)
                //fprintf (f, "%d ", (int) *(final_matrix + j + i*N) );
            }
            fprintf (f, "\n");
        }
        fclose (f);
    }
    // for(i = 0; i<N/world_size;i++)
    // {
    //     free(temp_matrix[i]);
    // }
}

void bounday_cond(float **local_tab, int world_rank, int world_size, int N, int nb_rows,float slope)
{
    int i,j;
    int t = nb_rows-2;

    float *boundry_y_strip = (float*)malloc((t)*sizeof(float));
    if (boundry_y_strip == NULL) { exit(-1); }
    float *boundry_y = (float*)malloc(N*sizeof(float));
    if (boundry_y == NULL) { exit(-1); }

    for(j=0;j<N;j++)
        boundry_y[j] = 0.0; //initialize boundary_y

    if (world_rank == 0)     //allocate row wise
    {
            i=1;
            for(j = 0; j<N; j++)
            {
                    local_tab[i][j] = local_tab[i][j]+slope*j;
            }
    }
    if (world_rank == 0)
    {
        for(i=0;i<N;i++)
            boundry_y[i] = boundry_y[i]+slope*(N-i);
        
        MPI_Scatter(boundry_y,nb_rows-2,MPI_FLOAT,boundry_y_strip,nb_rows-2,MPI_FLOAT,0,MPI_COMM_WORLD);
    }

    else
    {
        MPI_Scatter(NULL,nb_rows-2,MPI_FLOAT,boundry_y_strip,nb_rows-2,MPI_FLOAT,0,MPI_COMM_WORLD);
    }
    // printf("%f ", local_tab[0][N-1]);   
    for(j=0; j<nb_rows-1;j++){
        local_tab[j][N-1] = boundry_y_strip[j-1];
        }

    /*----------------print local array-----------------*/
    // printf(" world rank = %d \n",world_rank);
    // for(i = 0; i<nb_rows; i++)
    // {
    //     for(j = 0; j<N; j++)
    //     {
    //             printf("%f \t",local_tab[i][j]);
    //     }
    //     printf("\n");
    // }
    /*----------------print local array-----------------*/

    

    free(boundry_y);
    free(boundry_y_strip);

}

void print_matrix(int world_rank, int N, float **local_tab, int nb_rows)
{
    int i,j;
    printf("\n \n Matrix printed by me: %d \n\n", world_rank);
    for(i = 0; i<nb_rows; i++)
    {
        for(j = 0; j<N; j++)
        {
            printf(" %.2f", local_tab[i][j]); // Change ".2f" to "%d" for more clarity (test mode, without laplace calculation)
            //printf(" %d",(int) *(local_tab+j+i*N));
        }
        printf("\n");
    }
}
