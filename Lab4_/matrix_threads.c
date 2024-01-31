#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
int num_rows1, num_cols1, num_rows2, num_cols2;
 
void *compute_element(void *args) {
  float *index = (float *)args;
  int i = (int)index[0]; //index of arguments
  int j = (int)index[1];
  // int num_rows1 = *(int *)args;
  // int num_cols1 = *(int *)args + 1;
  // int num_rows2 = *(int *)args + 2;
  // int num_cols2 = *(int *)args + 3;
  // float *mat1 = (float *) args +3; //why +3?
  
  //Offsets
  float *mat1 = &index[4];
  float *mat2 = &mat1[num_rows1 * num_cols1];
  float *result_matrix = &mat2[num_rows2 * num_cols2];
  // float *mat2 = (float *)args + 3 + i * num_cols1 * num_cols2;
  // float *result_matrix = (float *)args + 2 + num_rows1 * num_cols1 * num_cols2;
 
  float sum = 0.0f;
  for (int k = 0; k < num_rows1; ++k) {
    sum += mat1[i * num_cols1 + k] * mat2[k * num_cols2 + j];
  }
  result_matrix[i * num_cols2 + j] = sum;

  return NULL;
}

void *compute_row(void *args) {

  //int *index = (int *)args; //why casting has *??
  //int i = index[0];
  float *index = (float *)args;
  int i = (int)index[0]; //index of arguments
  // int num_cols1 = *(int *)args;
  // int num_cols2 = *(int *)args + 1;
  // int num_rows1 = *(int *)args;
  // int num_rows2 = *(int *)args + 2;
  // float *mat1 = (float *) args +2; //why +2?
  // float *mat2 = (float *)args + 2 + i * num_cols1 * num_cols2;
  // float *result_matrix = (float *)args + 2 + num_rows1 * num_cols1 * num_cols2;

   //offsets
   float *mat1 = &index[2];
    float *mat2 = &index[num_rows1 * num_cols1];
    float *result_matrix = &mat2[num_rows2 * num_cols2];


  for (int j = 0; j < num_cols2; ++j) {
    float sum = 0.0f;
    for (int k = 0; k < num_rows1; ++k) {
      sum += mat1[i * num_cols1 + k] * mat2[k * num_cols2 + j];
    }
    result_matrix[i * num_cols2 + j] = sum;
  }

  return NULL;
}

float **read_matrix(char *filename) {
  FILE *fp = fopen(filename, "r");
  if (!fp) {
    perror("fopen");
    exit(1);
  }
  char line [ 328 ];

  // Read dimensions of both matrices
  fscanf(fp, "%d %d", &num_rows1, &num_cols1);
  for (int i = 0; i < num_rows1+1; ++i) {
    fgets(line,sizeof(line),fp);
  }

  fscanf(fp, "%d %d", &num_rows2, &num_cols2);

  if (num_cols1 != num_rows2) {
    fprintf(stderr, "No of col1 = %d , no of row2 =%d\n",num_cols1,num_rows2);
    fprintf(stderr, "Error: Number of columns of the first matrix must equal number of rows of the second matrix.\n");
    exit(1);
  }
  else     
    fprintf(stderr, "No of col1 = %d , no of row2 =%d\n",num_cols1,num_rows2);

    // Allocate memory for both matrices
    float **mat1 = (float **)malloc(num_rows1 * sizeof(float *));
  for (int i = 0; i < num_rows1; ++i) {
    mat1[i] = (float *)malloc(num_cols1 * sizeof(float));
    fgets(line,sizeof(line),fp);
  }

  float **mat2 = (float **)malloc(num_rows2 * sizeof(float *));
  for (int i = 0; i < num_rows2; ++i) {
    mat2[i] = (float *)malloc(num_cols2 * sizeof(float));
  }

  // Read elements of both matrices
  for (int i = 0; i < num_rows1; ++i) {
    for (int j = 0; j < num_cols1; ++j) {
      fscanf(fp, "%f", &mat1[i][j]);
    }
  }

  for (int i = 0; i < num_rows2; ++i) {
    for (int j = 0; j < num_cols2; ++j) {
      fscanf(fp, "%f", &mat2[i][j]);
  }
  }

  fclose(fp);
  fprintf(stderr, "Done reading file\n");

  // Pack arguments for threads
  int num_elements = 3 + num_rows1 * num_cols1 * num_cols2;
  float *args = malloc(num_elements * sizeof(float));
  // float *args = malloc((3 + num_rows1 * num_cols1 * num_cols2) * sizeof(float));
  args[0] = num_rows1;
  args[1] = num_cols1;
  args[2] = num_rows2;
  args[3] = num_cols2;
  fprintf(stderr, "Done allocating memory\n");
  
   // Copy matrix elements into args array
  memcpy(&args[4], mat1, num_rows1 * num_cols1 * sizeof(float));
  memcpy(&args[4 + num_rows1 * num_cols1], mat2, num_rows2 * num_cols2 * sizeof(float));


  // memcpy(args + 3, mat1, num_rows1 * num_cols1 * sizeof(float)); //memcpy???
  // memcpy(args + 3 + num_rows1 * num_cols1 * sizeof(float), mat2, num_rows2 * num_cols2 * sizeof(float));
  
  fprintf(stderr, "Done packing arguments\n");

  return mat1, mat2, args;  // Return matrices and packed arguments
}

void matrix_mul_element_wise(float *mat1, float *mat2, float *result_matrix, int num_rows1, int num_cols1, int num_rows2, int num_cols2) {
  clock_t start_time = clock();
    fprintf(stderr, "In element wise function\n");

  pthread_t threads[num_rows1 * num_cols2]; //thread is size of result matrix
//   thread_args_t thread_args[num_rows * num_cols];
  fprintf(stderr, "Done making element wise thread\n");

  for (int i = 0; i < num_rows1; ++i) { //matrix 1
    for (int j = 0; j < num_cols2; ++j) { //matrix 2
      int *index = malloc(3 * sizeof(int)); //why 3?
      index[0] = i;
      index[1] = j;
      pthread_create(&threads[i * num_cols2 + j], NULL, compute_element, index);
    }
  }

  for (int i = 0; i < num_rows1 * num_cols2; ++i) {
    pthread_join(threads[i], NULL);
  }

  clock_t end_time = clock();

  printf("Element-wise multiplication time: %f seconds\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);
  free(mat1);
  free(mat2);
  free(result_matrix);
}

void matrix_mul_row_wise(float *mat1, float *mat2, float *result_matrix, int num_rows1, int num_cols1, int num_rows2, int num_cols2) {
  clock_t start_time = clock();

  pthread_t threads[num_rows1];
//   thread_args_t thread_args[num_rows];

  for (int i = 0; i < num_rows1; ++i) {
    int *index = malloc(2* sizeof(int)); //why 2?
    index[0] = i;
    pthread_create(&threads[i], NULL, compute_row, index);
  }

  for (int i = 0; i < num_rows1; ++i) {
    pthread_join(threads[i], NULL);
  }

  clock_t end_time = clock();

  printf("Row-wise multiplication time: %f seconds\n", (double)(end_time - start_time) / CLOCKS_PER_SEC);
  free(mat1);
  free(mat2);
  free(result_matrix);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <matrix_file>\n", argv[0]);
    exit(1);
  }
  float **mat1, **mat2; //why 2 **?
  float *args;
  int num_rows1, num_cols1;
  int num_rows2, num_cols2;

  mat1, mat2, args = read_matrix(argv[1]);
  num_rows1 = args[0];
  num_cols1 = args[1];
  num_rows2 = args[2];
  num_cols2 = args[3];  
    fprintf(stderr, "Done reading matrices\n");

    // Allocate memory for result matrix
  float *result_matrix = (float *)malloc(num_rows1 * num_cols2 * sizeof(float));

    fprintf(stderr, "Done allocating result matrix\n");

   // Perform element-wise and row-wise multiplication
  matrix_mul_element_wise(mat1[0], mat2[0], result_matrix, num_rows1, num_cols1, num_rows2, num_cols2);
  matrix_mul_row_wise(mat1[0], mat2[0], result_matrix, num_rows1, num_cols1, num_rows2, num_cols2);

  // Free remaining allocated memory
  free(args);

  return 0;
}
