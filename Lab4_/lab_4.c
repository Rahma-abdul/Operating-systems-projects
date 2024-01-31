// #include <pthread.h>
// #include <stdio.h>

// void *myturn(void *arg){
//     while(1){
//         sleep(1);
//         printf("My turn!\n");
//     }
//     return NULL;
// }


// void yourturn(){
//     while(1){
//         sleep(2);
//         printf("Your turn!\n");
//     }
// }

// void main(){

//     pthread_t newthread ;
//     pthread_create(&newthread,NULL,myturn,NULL);
//     //myturn();
//     yourturn();
// }

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


void *print_message_function( void *ptr ) //the thread 
{
     char *message;
     message = (char *) ptr;
     printf("%s hiii\n", message);
}

main()
{
     pthread_t thread1, thread2; //creating 2 threads
     //^ thread identifier
     char *message1 = "Thread 1";
     char *message2 = "Thread 2";
     int  iret1, iret2;

    /* Create independent threads each of which will execute function */

     iret1 = pthread_create( &thread1, NULL, print_message_function, (void*) message1);
     iret2 = pthread_create( &thread2, NULL, print_message_function, (void*) message2);

     //Wait till threads are complete before main continues. 
     //Unless we wait we run the risk of executing an exit 
     //which will terminate the process and all threads 
     //before the threads have completed.

     pthread_join( thread1, NULL); //waits for thread1 to exit
     pthread_join( thread2, NULL); 

     printf("Thread 1 returns: %d\n",iret1);
     printf("Thread 2 returns: %d\n",iret2);
     exit(0);
}

