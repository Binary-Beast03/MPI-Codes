
#include <stdio.h>
#include "mpi.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    int       p;         
    MPI_Comm  comm;      
    MPI_Comm  row_comm;  
    MPI_Comm  col_comm;  
    int       q;         
    int       my_row;    
    int       my_col;    
    int       my_rank;   
} GRID_INFO_T;


#define MAX 65536
typedef struct {
    int     n_bar;
#define Order(A) ((A)->n_bar)
    float  entries[MAX];
#define Entry(A,i,j) (*(((A)->entries) + ((A)->n_bar)*(i) + (j)))
} LOCAL_MATRIX_T;


LOCAL_MATRIX_T*  Local_matrix_allocate(int n_bar);
void             Free_local_matrix(LOCAL_MATRIX_T** local_A);
void             Read_matrix(LOCAL_MATRIX_T* local_A, 
                     GRID_INFO_T* grid, int n);
void             Print_matrix(char* title, LOCAL_MATRIX_T* local_A, 
                     GRID_INFO_T* grid, int n);
void             Set_to_zero(LOCAL_MATRIX_T* local_A);
void             Local_matrix_multiply(LOCAL_MATRIX_T* local_A,
                     LOCAL_MATRIX_T* local_B, LOCAL_MATRIX_T* local_C);
void             Build_matrix_type(LOCAL_MATRIX_T* local_A);
MPI_Datatype     local_matrix_mpi_t;

LOCAL_MATRIX_T*  temp_mat;
void             Print_local_matrices(char* title, LOCAL_MATRIX_T* local_A, 
                     GRID_INFO_T* grid);
double           largest(double arr[], int n);

void             Setup_grid(GRID_INFO_T*  grid);
void             Fox(int n, GRID_INFO_T* grid, LOCAL_MATRIX_T* local_A,
                    LOCAL_MATRIX_T* local_B, LOCAL_MATRIX_T* local_C);

/*********************************************************/
int main(int argc, char* argv[]) {
    int              p;
    int              world_rank;
    int              world_size;
    GRID_INFO_T      grid;
    LOCAL_MATRIX_T*  local_A;
    LOCAL_MATRIX_T*  local_B;
    LOCAL_MATRIX_T*  local_C;
    int              n;
    int              n_bar;
    int              u;     //size controller
    clock_t          t;    //clocking time
    double           start;
    double           end;
    
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);
    FILE *fp;
    char buffer[100];
    snprintf(buffer, sizeof(char) * 100, "matrix_multi_fox (nodes = %i).csv", world_size);
    fp=fopen(buffer,"w");
    n=0;
    u=2;
    while(u!=23)
    {
    n=u*u;
    t = clock();            //time start
    MPI_Barrier(MPI_COMM_WORLD);
    Setup_grid(&grid);
    start = MPI_Wtime();
    // if (my_rank == 0) {
    //     printf("What's the order of the matrices?\n");
    //     scanf("%d", &n);
    // }

    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    n_bar = n/grid.q    ;

    local_A = Local_matrix_allocate(n_bar);
    Order(local_A) = n_bar;
    Read_matrix(local_A, &grid, n);
    // Print_matrix("We read A =", local_A, &grid, n);

    local_B = Local_matrix_allocate(n_bar);
    Order(local_B) = n_bar;
    Read_matrix( local_B, &grid, n);
    // Print_matrix("We read B =", local_B, &grid, n);

    Build_matrix_type(local_A);
    temp_mat = Local_matrix_allocate(n_bar);

    local_C = Local_matrix_allocate(n_bar);
    Order(local_C) = n_bar;
    Fox(n, &grid, local_A, local_B, local_C);
    // Print_matrix("The product is", local_C, &grid, n);
    // t = clock() - t;            //time_end
    MPI_Barrier(MPI_COMM_WORLD);
    end = MPI_Wtime();
    double t_store[world_size];
    // double time_taken = ((double)t)/CLOCKS_PER_SEC;
    double time_taken = end - start;
    if (world_rank == 0)
    {
        MPI_Gather(&time_taken,1,MPI_DOUBLE,t_store,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
        int m = sizeof(t_store)/sizeof(t_store[0]);
        printf("size = %d Time taken = %f \n",n,largest(t_store,m));
        fprintf(fp,"%d\t%f",n,largest(t_store,m));
        fprintf(fp,"\n");
    }
    else
    {
        MPI_Gather(&time_taken,1,MPI_DOUBLE,NULL,0,MPI_DOUBLE,0,MPI_COMM_WORLD);
    }

    Free_local_matrix(&local_A);
    Free_local_matrix(&local_B);
    Free_local_matrix(&local_C);
    u+=1;
    }
    MPI_Finalize();
    return 0;
} 


/*********************************************************/
void Setup_grid(
         GRID_INFO_T*  grid  /* out */) {
    int old_rank;
    int dimensions[2];
    int wrap_around[2];
    int coordinates[2];
    int free_coords[2];

    
    MPI_Comm_size(MPI_COMM_WORLD, &(grid->p));
    MPI_Comm_rank(MPI_COMM_WORLD, &old_rank);

    
    grid->q = (int) sqrt((double) grid->p);
    dimensions[0] = dimensions[1] = grid->q;

    wrap_around[0] = wrap_around[1] = 1;
    MPI_Cart_create(MPI_COMM_WORLD, 2, dimensions, 
        wrap_around, 1, &(grid->comm));
    MPI_Comm_rank(grid->comm, &(grid->my_rank));
    MPI_Cart_coords(grid->comm, grid->my_rank, 2, 
        coordinates);
    grid->my_row = coordinates[0];
    grid->my_col = coordinates[1];

    free_coords[0] = 0; 
    free_coords[1] = 1;
    MPI_Cart_sub(grid->comm, free_coords, 
        &(grid->row_comm));

    free_coords[0] = 1; 
    free_coords[1] = 0;
    MPI_Cart_sub(grid->comm, free_coords, 
        &(grid->col_comm));
} 


/*********************************************************/
void Fox(
        int              n         , 
        GRID_INFO_T*     grid      , 
        LOCAL_MATRIX_T*  local_A   ,
        LOCAL_MATRIX_T*  local_B   ,
        LOCAL_MATRIX_T*  local_C   ) {

    LOCAL_MATRIX_T*  temp_A;
                             
                            
    int              stage;
    int              bcast_root;
    int              n_bar;  
    int              source;
    int              dest;
    MPI_Status       status;

    n_bar = n/grid->q;
    Set_to_zero(local_C);

     
    source = (grid->my_row + 1) % grid->q;
    dest = (grid->my_row + grid->q - 1) % grid->q;

    
    temp_A = Local_matrix_allocate(n_bar);

    for (stage = 0; stage < grid->q; stage++) {
        bcast_root = (grid->my_row + stage) % grid->q;
        if (bcast_root == grid->my_col) {
            MPI_Bcast(local_A, 1, local_matrix_mpi_t,
                bcast_root, grid->row_comm);
            Local_matrix_multiply(local_A, local_B, 
                local_C);
        } else {
            MPI_Bcast(temp_A, 1, local_matrix_mpi_t,
                bcast_root, grid->row_comm);
            Local_matrix_multiply(temp_A, local_B, 
                local_C);
        }
        MPI_Sendrecv_replace(local_B, 1, local_matrix_mpi_t,
            dest, 0, source, 0, grid->col_comm, &status);
    } 
    
} 


/*********************************************************/
LOCAL_MATRIX_T* Local_matrix_allocate(int local_order) {
    LOCAL_MATRIX_T* temp;
  
    temp = (LOCAL_MATRIX_T*) malloc(sizeof(LOCAL_MATRIX_T));
    return temp;
}  


/*********************************************************/
void Free_local_matrix(
         LOCAL_MATRIX_T** local_A_ptr  /* in/out */) {
    free(*local_A_ptr);
}  


/*********************************************************/

void Read_matrix( 
     LOCAL_MATRIX_T*  local_A  ,
     GRID_INFO_T*     grid     ,
    int              n        ) {

    int        mat_row, mat_col;
    int        grid_row, grid_col;
    int        dest;
    int        coords[2];
    float*     temp;
    int        upper,lower;

    lower=1;
    upper=10000;
    MPI_Status status;
    
    if (grid->my_rank == 0) {
        temp = (float*) malloc(Order(local_A)*sizeof(float));
        // printf("%s\n", prompt); 
        // fflush(stdout);
        for (mat_row = 0;  mat_row < n; mat_row++) {
            grid_row = mat_row/Order(local_A);
            coords[0] = grid_row;
            for (grid_col = 0; grid_col < grid->q; grid_col++) {
                coords[1] = grid_col;
                MPI_Cart_rank(grid->comm, coords, &dest);
                if (dest == 0) {
                    for (mat_col = 0; mat_col < Order(local_A); mat_col++){
                        // printf("Inside dest = 0 col = %d\n",mat_col);
                        // scanf("%f", 
                        //   (local_A->entries)+mat_row*Order(local_A)+mat_col);
                              *((local_A->entries)+mat_row*Order(local_A)+mat_col)=(rand() % (upper - lower + 1)) + lower;}
                        //   printf("%f\n",*(local_A->entries)+mat_row*Order(local_A)+mat_col);}
                } else {
                    for(mat_col = 0; mat_col < Order(local_A); mat_col++){
                        // printf("Inside else dest = %d\n col = %d",dest,mat_col);
                        // scanf("%f", temp + mat_col);
                           *(temp + mat_col)=(rand() % (upper - lower + 1)) + lower;}
                        // printf("%f\n",*temp + mat_col);}
                    MPI_Send(temp, Order(local_A), MPI_FLOAT, dest, 0,
                        grid->comm);
                }
            }
        }
        free(temp);
    } else {
        for (mat_row = 0; mat_row < Order(local_A); mat_row++) 
            MPI_Recv(&Entry(local_A, mat_row, 0), Order(local_A), 
                MPI_FLOAT, 0, 0, grid->comm, &status);
    }
                     
} 


/*********************************************************/
void Print_matrix(
         char*            title    ,  
         LOCAL_MATRIX_T*  local_A  ,
         GRID_INFO_T*     grid     ,
         int              n        ) {
    int        mat_row, mat_col;
    int        grid_row, grid_col;
    int        source;
    int        coords[2];
    float*     temp;
    MPI_Status status;

    if (grid->my_rank == 0) {
        temp = (float*) malloc(Order(local_A)*sizeof(float));
        printf("%s\n", title);
        for (mat_row = 0;  mat_row < n; mat_row++) {
            grid_row = mat_row/Order(local_A);
            coords[0] = grid_row;
            for (grid_col = 0; grid_col < grid->q; grid_col++) {
                coords[1] = grid_col;
                MPI_Cart_rank(grid->comm, coords, &source);
                if (source == 0) {
                    for(mat_col = 0; mat_col < Order(local_A); mat_col++)
                        printf("%4.1f ", Entry(local_A, mat_row, mat_col));
                } else {
                    MPI_Recv(temp, Order(local_A), MPI_FLOAT, source, 0,
                        grid->comm, &status);
                    for(mat_col = 0; mat_col < Order(local_A); mat_col++)
                        printf("%4.1f ", temp[mat_col]);
                }
            }
            printf("\n");
        }
        free(temp);
    } else {
        for (mat_row = 0; mat_row < Order(local_A); mat_row++) 
            MPI_Send(&Entry(local_A, mat_row, 0), Order(local_A), 
                MPI_FLOAT, 0, 0, grid->comm);
    }
                     
}  


/*********************************************************/
void Set_to_zero(
         LOCAL_MATRIX_T*  local_A  /* out */) {

    int i, j;

    for (i = 0; i < Order(local_A); i++)
        for (j = 0; j < Order(local_A); j++)
            Entry(local_A,i,j) = 0.0;

}


/*********************************************************/
void Build_matrix_type(
         LOCAL_MATRIX_T*  local_A  /* in */) {
    MPI_Datatype  temp_mpi_t;
    int           block_lengths[2];
    MPI_Aint      displacements[2];
    MPI_Datatype  typelist[2];
    MPI_Aint      start_address;
    MPI_Aint      address;

    MPI_Type_contiguous(Order(local_A)*Order(local_A), 
        MPI_FLOAT, &temp_mpi_t);

    block_lengths[0] = block_lengths[1] = 1;
   
    typelist[0] = MPI_INT;
    typelist[1] = temp_mpi_t;

    MPI_Get_address(local_A, &start_address);
    MPI_Get_address(&(local_A->n_bar), &address);
    displacements[0] = address - start_address;
    
    MPI_Get_address(local_A->entries, &address);
    displacements[1] = address - start_address;

    MPI_Type_create_struct(2, block_lengths, displacements,
        typelist, &local_matrix_mpi_t);
    MPI_Type_commit(&local_matrix_mpi_t); 
} 


/*********************************************************/
void Local_matrix_multiply(
         LOCAL_MATRIX_T*  local_A  ,
         LOCAL_MATRIX_T*  local_B  , 
         LOCAL_MATRIX_T*  local_C  ) {
    int i, j, k;

    for (i = 0; i < Order(local_A); i++)
        for (j = 0; j < Order(local_A); j++)
            for (k = 0; k < Order(local_B); k++)
                Entry(local_C,i,j) = Entry(local_C,i,j) 
                    + Entry(local_A,i,k)*Entry(local_B,k,j);

}


/*********************************************************/
void Print_local_matrices(
         char*            title    /* in */,
         LOCAL_MATRIX_T*  local_A  /* in */, 
         GRID_INFO_T*     grid     /* in */) {

    int         coords[2];
    int         i, j;
    int         source;
    MPI_Status  status;

    if (grid->my_rank == 0) {
        printf("%s\n", title);
        printf("Process %d > grid_row = %d, grid_col = %d\n",
            grid->my_rank, grid->my_row, grid->my_col);
        for (i = 0; i < Order(local_A); i++) {
            for (j = 0; j < Order(local_A); j++)
                printf("%4.1f ", Entry(local_A,i,j));
            printf("\n");
        }
        for (source = 1; source < grid->p; source++) {
            MPI_Recv(temp_mat, 1, local_matrix_mpi_t, source, 0,
                grid->comm, &status);
            MPI_Cart_coords(grid->comm, source, 2, coords);
            printf("Process %d > grid_row = %d, grid_col = %d\n",
                source, coords[0], coords[1]);
            for (i = 0; i < Order(temp_mat); i++) {
                for (j = 0; j < Order(temp_mat); j++)
                    printf("%4.1f ", Entry(temp_mat,i,j));
                printf("\n");
            }
        }
        fflush(stdout);
    } else {
        MPI_Send(local_A, 1, local_matrix_mpi_t, 0, 0, grid->comm);
    }
        
}

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