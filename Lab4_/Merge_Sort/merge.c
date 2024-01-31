#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>


//Global Variables --> Threads arguments
int n;
int *array;


void merge_fun( int L , int M , int R){
    int L_size = M-L+1; //size of left array
    int R_size = R -M; //size of right array
    //Create Left and right array
    int L_array[L_size] ,R_array[R_size];
   
    //printf("Merging....\n");

    for (int i = 0 ; i<L_size;i++){
        int x = L+i;
        L_array[i] = array[x];
        //printf("%d ",L_array[i]);
    }
    //printf(" And ");
    for (int i = 0 ; i<R_size;i++){
        int x  = M+1+i;
        R_array[i] = array[x];
        //printf("%d ",R_array[i]);
    }
    
    //Merge both of them by comparing 
    int i=0 , j =0,k=L;
    for (k; i<L_size && j<R_size ;k++){
        if(L_array[i] <= R_array[j]){
            array[k] = L_array[i];
            // printf("1addinggg in array %d in index = %d\n", array[k],k);
            i++;
        }
        else {
            array[k] = R_array[j];
            // printf("2addinggg in array %d in index = %d\n", array[k],k);
            j++;
        }

    }

    //Rest of the elements are merged directly without comapring
    while (i < L_size) { //if left array didn't finish
    array[k] = L_array[i];
    // printf("3addinggg in array %d in index = %d\n", array[k],k);
    i++;
    k++;
  }
  while (j < R_size ) { //if right array didn't finish
    array[k] = R_array[j];
    // printf("4addinggg in array %d in index = %d\n", array[k],k);
    j++;
    k++;
  } 

}

void *sort_thread(void *args){
    
    //printf("In sort thread\n");
    int left = *(int *)args;
    int right = *((int *)args +1);

    //threads of each level of dividing of array
    if (left <right){ //right path in array
        int mid = (left +right) /2;
        //each thread is split into 2
        pthread_t L_thread , R_thread ;

        //Threads arguments for each split
        int args_left[2] = {left,mid};
        int args_right[2] ={mid+1,right};

        //create both threads and calling thread function recusrively 
        pthread_create(&L_thread, NULL, sort_thread, (void *)args_left);
        pthread_create(&R_thread, NULL, sort_thread, (void *)args_right);


        // wait at each level for both threads to finish sorting
        pthread_join(L_thread,NULL); 
        pthread_join(R_thread,NULL); 

        //Now merge both threads 
        //merge_fun(new_left , mid ,right);
        merge_fun(left , mid ,right);
    }
    //free(args);
    return NULL; //base case if 1 element -->already sorted
}



void merge_sort(){
    // printf("\nIn merge sort\n");

    //Iniital thread
    pthread_t thread;
    int args[2] = {0,n-1};
    pthread_create(&thread, NULL, sort_thread, (void *)args);
    pthread_join(thread,NULL); //-->wait for thread to finish to move on
   
}


int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <array_file>\n", argv[0]);
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

    //Read size of array
    fscanf(fp,"%d", &n );
    //printf("%d\n",n);
    
    // 2) Allocating array
    array = malloc(n*sizeof(int));
    if (!array) { //checking no error in allocation
    perror("malloc");
    exit(1);
  }

    printf("\nGiven Array: \n");
    for (int i = 0; i < n; ++i) {
            fscanf(fp, "%d", &array[i]);
            printf("%d  ",array[i]);
        }
    printf("\nDone reading File...\n");
    printf("----------------------------------------------------\n");
    fclose(fp);
    //left and right initially
    
    // 3) Calling Merge_sort function
    merge_sort();
    fprintf(stderr,"\nSorting result:\n");
    for (int i = 0; i < n; ++i) {
            printf("%d  ", array[i]);
    }

    printf("\n----------------------------------------------------\n");
    
    //free allocated memory 
    free(array);
    return 0;
}