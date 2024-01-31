#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>

//Global Variables --> Threads arguments
int rows1, cols1 , rows2 ,cols2;

float *mat1,*mat2,*res;

void *element_thread(void *args){
    //thread of each element in res

    int *index =(int *)args;
    int i = index[0];
    int j = index[1];
    //printf("%d,%d\n",i,j);
    float sum = 0.0f;
    //printf("here i is %d, and j is %d\n\n",i,j);
    for (int k = 0 ;k < cols1 ; ++k){
        int x = i *cols1+k;
        int y = k*cols2+j;
        sum = sum +(mat1[x]*mat2[y]);
        //printf("here you go 1: %f ,2: %f = %f\n",mat1[x],mat2[y],sum);

    }
    int x = i*cols2+j;
    res[x] = sum;

   
    return NULL;
}


void matrix_element_wise(){
    fprintf(stderr, "In element wise function\n");    

    // 1) Start timing
    clock_t strt = clock();
    // 2) Identifing threads needed --> no. of elements in res matrix
    pthread_t threads[rows1 * cols2];

    // 3) Create the thread for each element
   
    int c =0 ;
    for (int i = 0; i < rows1; i++) { //matrix 1
        for (int j = 0; j < cols2; j++) { //matrix 2
    int *index = malloc(2* sizeof(int)); 
    //   index[c++] = i;
    //   index[c++] = j;
        index[0] =i;
        index[1] =j;
     // int x =i * cols2 + j;
      //printf("Thread number %d, row = %d ,col = %d\n",x,i,j);
      pthread_create(&threads[c], NULL, element_thread,  (void *)index);
      c++;
    }
  }
      //fprintf(stderr, "Threads created\n");    


    // 4) Joining all threads --> wait for them to exit so we can move on
    for (int i = 0; i < rows1 * cols2; i++) {
    pthread_join(threads[i], NULL);

    //fprintf(stderr, "Threads joined\n");    

  }

    clock_t end = clock();  
    printf("Element-wise multiplication time: %f seconds\n", (double)(end - strt) / CLOCKS_PER_SEC);

    //free(index);
}


void *row_thread(void *args){
    //thread of each row in res

    int *index =(int *)args;
    int i = index[0];
    //printf("%d\n",i);
    //printf("here i is %d, and j is %d\n\n",i,j);
    for (int k = 0 ;k < cols2 ; ++k){
        float sum = 0.0f;
        for(int z = 0;z<cols1;++z){
            int x = i *cols1+z;
            int y = z *cols2+k;
            sum = sum +(mat1[x]*mat2[y]);
        }
        int x = i*cols2+k;
        res[x] = sum;

    }
    
   

    return NULL;
}


void matrix_row_wise(){
    fprintf(stderr, "In row wise function\n");    

    // 1) Start timing
    clock_t strt = clock();

    // 2) Identifing threads needed --> no. of rows in res matrix
    pthread_t threads[rows1];

    // 3) Create the thread for each element
    for (int i = 0; i < rows1; ++i) { //matrix 1
      int *index = malloc(rows1* sizeof(int)); 
      index[0] = i;
      //printf("Thread number %d\n",i);
      pthread_create(&threads[i], NULL, row_thread, (void *)index);
  }
      //fprintf(stderr, "Threads created\n");    


    // 4) Joining all threads --> wait for them to exit so we can move on
    for (int i = 0; i < rows1; i++) {
    pthread_join(threads[i], NULL);
  }

    clock_t end = clock();
    printf("Row-wise multiplication time: %f seconds\n", (double)(end - strt) / CLOCKS_PER_SEC);

    //free(index);
}



int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <matrix_file>\n", argv[0]);
    exit(1);
  }
    // 1) Open and read file 
    //char *filename = "matrix.txt";
    char *filename = argv[1];
    FILE *fp = fopen(filename ,"r");
    if (!fp) {
    perror("fopen");
    exit(1);
  }

    // 2) Check Comaptiability

    //Read size of matrix 1
    fscanf(fp, "%d %d", &rows1, &cols1);

    // 3) Allocating matrices
    mat1 = malloc(rows1 *cols1 * sizeof(float));
    printf("Matrix 1: \n");
    for (int i = 0; i < rows1; ++i) {
        for (int j = 0; j < cols1; ++j) {
            int x = i * cols1 + j;
            fscanf(fp, "%f", &mat1[x]);
            printf("%f\t",mat1[x]);
        }
        printf("\n");
    }
    fprintf(stderr,"Done allocating matrix 1 and reading it\n\n");
    printf("-------------------\n");

    fscanf(fp, "%d %d", &rows2, &cols2);
    mat2 = malloc(rows2* cols2 * sizeof(float));
    printf("Matrix 2: \n");
    for (int i = 0; i < rows2; ++i) {
        for (int j = 0; j < cols2; ++j) {
            int x = i * cols2 + j;
            fscanf(fp, "%f", &mat2[x]);
            printf("%f\t",mat2[x]);
            }
            printf("\n");
    }
    fprintf(stderr,"Done allocating matrix 2 and reading it\n\n");
    printf("----------------------------------------------------\n");

    //checking if compatiable sizes for multiplications
    if (cols1 != rows2){
        fprintf(stderr, "No of col1 = %d , no of row2 =%d\n",cols1,rows2);
        fprintf(stderr, "Error: Number of columns of the first matrix must equal number of rows of the second matrix.\n");
        exit(1);   
        }

    fprintf(stderr, "No of col1 = %d , no of row2 =%d\n",cols1,rows2);
    fprintf(stderr, "Comaptiable!! \n\n");

    fclose(fp);
    fprintf(stderr,"Done reading File\n");

    // 4) Allocating memory for final matrix
    res = malloc(rows1*cols2*sizeof(float));
    fprintf(stderr, "Result matrix allocated!\n\n");
    // 5) Calling 2 functions to perform 2 algorithms

    printf("----------------------------------------------------\n");

    matrix_element_wise();
     
    //Res matrix from first algorithm
    fprintf(stderr,"\nElement Wise result\n");
    for (int i = 0; i < rows1; ++i) {
        for (int j = 0; j < cols2; ++j) {
            printf("%f ", res[i * cols2 + j]);
        }
        printf("\n");
    }
    printf("----------------------------------------------------\n");
    sleep(3);
    matrix_row_wise();

    //Res matrix from second algorithm
    fprintf(stderr,"\nRow Wise result\n");
    for (int i = 0; i < rows1; ++i) {
        for (int j = 0; j < cols2; ++j) {
            printf("%f ", res[i * cols2 + j]);
        }
        printf("\n");
    }


    //free allocated memory 
    free(mat1);
    free(mat2);
    free(res);

    return 0;
}