#include<stdio.h>
#include"mpi.h"
#include<stdlib.h>

void mergesort(int* a, int* b, int l, int r);
void merge(int* a, int* b, int l, int m, int r);
double largest(double arr[], int n);

int main(int argc, char** argv )
{
    MPI_Init(&argc,&argv);
    FILE *fp;
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD,&world_size);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD,&world_rank);

    int N = 40;      //size of array
    int i,lower,upper,c;
    lower=1;
    upper=1000;
    double start;
    double end;
    char buffer[100];
    snprintf(buffer, sizeof(char) * 100, "merge_sort_parallel (nodes = %i).csv", world_size);
    fp=fopen(buffer,"w");
    while(N!=3004)
    {
    MPI_Barrier(MPI_COMM_WORLD);
    start = MPI_Wtime();
    int *arr;
    arr = (int*)malloc(N * sizeof(int));    //array which store values
    if(world_rank == 0)
    {
    for(i=0;i<N;i++)
        arr[i] = (rand() % (upper - lower + 1)) + lower;
    }
    
    int sub_array_size = N/world_size;
    int *sub_array;
    sub_array = (int*)malloc(sub_array_size * sizeof(int));
    if (world_rank == 0)
        MPI_Scatter(arr,sub_array_size,MPI_INT,sub_array,sub_array_size,MPI_INT,0,MPI_COMM_WORLD);

    else
        MPI_Scatter(NULL,sub_array_size,MPI_INT,sub_array,sub_array_size,MPI_INT,0,MPI_COMM_WORLD);

    int *temp_array = malloc(sub_array_size * sizeof(int));
    mergesort(sub_array,temp_array,0,(sub_array_size-1));
    int *sorted = NULL;
	if(world_rank == 0) {
		
		sorted = malloc(N* sizeof(int));
		
		}
	
	MPI_Gather(sub_array, sub_array_size, MPI_INT, sorted, sub_array_size, MPI_INT, 0, MPI_COMM_WORLD);
	
	if(world_rank == 0) 
    {
		
		int *other_array = malloc(N * sizeof(int));
		mergesort(sorted, other_array, 0, (N - 1));
		// MPI_Barrier(MPI_COMM_WORLD);
        end = MPI_Wtime();
        double time_taken = end - start;
        printf("size = %d Time taken = %f \n",N,time_taken);
        fprintf(fp,"%d\t%f",N,time_taken);
        fprintf(fp,"\n");


		// printf("This is the sorted array: "); // To print final sorted array
		// for(c = 0; c < N; c++) {
			
		// 	printf("%d ", sorted[c]);
			
		// 	}
			
		// printf("\n");
		// printf("\n");
			
		
		free(sorted);
		free(other_array);
			
	}
   
    // if (world_rank == 0)
    // {
    //      double t_store[world_size];
    
    //     double time_taken = end - start;
    //     MPI_Gather(&time_taken,1,MPI_DOUBLE,t_store,1,MPI_DOUBLE,0,MPI_COMM_WORLD);
    //     int n = sizeof(t_store)/sizeof(t_store[0]);
    //     printf("size = %d Time taken = %f \n",N,largest(t_store,n));
    //     fprintf(fp,"%d\t%f",N,largest(t_store,n));
    //     fprintf(fp,"\n");
    // }
    // else
    // {    double t_store[world_size];
    
    //     double time_taken = end - start;
    //     MPI_Gather(&time_taken,1,MPI_DOUBLE,NULL,0,MPI_DOUBLE,0,MPI_COMM_WORLD);
    // }
	
	
	free(arr);
	free(sub_array);
	free(temp_array);
	

	N+=world_size;
    }
	MPI_Finalize();
	
    

}

void merge(int* a, int* b, int l, int m, int r)
{
    int h,i,j,k;
    h=l;
    i=l;
    j=m+1;
    while((h<=m)&&(j<=r))
    {
        if(a[h]<=a[j])
        {
            b[i] = a[h];
            h++;
        }

        else
        {
            b[i] = a[j];
            j++;
        }

        i++;
    }
    if(m<h)
    {
        for(k=j;k<=r;k++)
        {
            b[i] = a[k];
            i++;
        }
    }
    else
    {
        for(k=h;k<=m;k++)
        {
            b[i] = a[k];
            i++;
        }
    }
    for(k=l;k<=r;k++)
    {
        a[k] = b[k];
    }
}

void mergesort(int* a, int* b, int l, int r)
{
    int m;

    if(l<r)
    {
        m = (l+r)/2;
        mergesort(a,b,l,m);
        mergesort(a,b,(m+1),r);
        merge(a,b,l,m,r);
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