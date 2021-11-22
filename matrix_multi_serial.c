#include<stdio.h>
#include<time.h>
#include<stdlib.h>
int main()
{   
    clock_t t;
    
    int size=4;
    FILE *fp;
    fp=fopen("matrix_multi_serial.csv","w");
    while(size<3000)
    {
    int r,c,i,j,k,lower,upper;
    // int a[size][size], b[size][size],mul[size][size];
    int (*a)[size] = malloc(size * sizeof(*a));
    int (*b)[size] = malloc(size * sizeof(*b));
    int (*mul)[size] = malloc(size * sizeof(*mul));
    // int** a =(int**)malloc(r*sizeof(int*));
    // for (i = 0; i < r; i++)
    //     a[i] = (int*)malloc(c * sizeof(int));

    // int** b =(int**)malloc(r*sizeof(int*));
    // for (i = 0; i < r; i++)
    //     b[i] = (int*)malloc(c * sizeof(int));

    // int** mul =(int**)malloc(r*sizeof(int*));
    // for (i = 0; i < r; i++)
    //     mul[i] = (int*)malloc(c * sizeof(int));
    lower=1;
    upper=10000;
    // printf("enter the number of row (should be multiple of 4) = ");    
    // scanf("%d",&r);
    // printf("enter the number of column (should be multiple of 4) =");    
    // scanf("%d",&c);
    // printf("enter the first matrix element=\n");
    r=size;
    c=size;
    t = clock();    
    for(i=0;i<r;i++)
    {    
        for(j=0;j<c;j++)    
        {
            a[i][j]=(rand() % (upper - lower + 1)) + lower;
        }
    }
    // printf("enter the second matrix element=\n");    
    for(i=0;i<r;i++)
    {    
        for(j=0;j<c;j++)    
        {
            b[i][j]=(rand() % (upper - lower + 1)) + lower;
        }
    } 

    printf("multiply of the matrix=\n");    
    for(i=0;i<r;i++)    
    {    
        for(j=0;j<c;j++)    
        {    
            mul[i][j]=0;    
            for(k=0;k<c;k++)    
            {    
                mul[i][j]+=a[i][k]*b[k][j];    
            }       
        }    
    }

    // for(i=0;i<r;i++)    
    // {    
    //     for(j=0;j<c;j++)    
    //     {    
    //         printf("%d\t",mul[i][j]);    
    //     }    
    //     printf("\n");    
    // }
    t = clock() - t;
    double time_taken = ((double)t)/CLOCKS_PER_SEC;
    printf("size = %d time taken = %f \n",size,time_taken);
    fprintf(fp,"%d\t%f",size,time_taken);
    fprintf(fp,"\n");
    size=size+4;
    free(a);
    free(b);
    free(mul);
    }
fclose(fp);    
return 0;  
}             
