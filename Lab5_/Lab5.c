#include <stdio.h> 
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h> 
#include <semaphore.h> 
#include <unistd.h> 
#include <signal.h>
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KNRM  "\x1B[0m"


#define N_cnt 5 // n mCounter threads
#define b 10 // buffer size
#define t1 3 // time of mMonitor

int counter = 0;
int buffer[b]; //queue of integers
int head = 0, tail = 0; // 3lshan n3rf n-access el queue


sem_t counter_locked;
sem_t buffer_locked;
sem_t empty;
sem_t full;


void *enqueue(int value) { //returns pointer
    buffer[tail] = value;
    tail = (tail + 1) % b;
}

int dequeue() {
    int value = buffer[head];
    head = (head + 1) % b;
    return value;
}


void *mCounter(void *arg) { 
    //count independent incoming messages
    //int i = (int)arg;
    // pthread_t thread_id = pthread_self(); //kan bygeb el id mesh el number
    // int* threadss = (int*)malloc(sizeof(int) * N_cnt);
    // int thread_num = threadss[(int) arg];
    int thread_num = (intptr_t)arg +1;

    // all msgs received at once --> no delay
    printf(KYEL "Counter thread (%d): received a message\n" KNRM,thread_num );
    while (1) {
        sleep(rand() % 20);

        printf(KYEL "Counter thread (%d): waiting to write\n" KNRM, thread_num);
        
        sem_wait(&counter_locked); //Grants access to counter
        
        //printf(KYEL "Counter thread (%d): now adding to counter, counter value before adding =%d\n" KNRM, thread_num, counter);
        counter++;
        printf(KYEL "Counter thread (%d): now adding to counter, counter value after adding =%d\n" KNRM, thread_num, counter);
        
        sem_post(&counter_locked);
    }
}

void *mMonitor(void *arg) { //Producer for Collector
    //count of threads at time intervals of size t1,
    while (1) {
        sleep(t1+ rand() % 5);
        printf(KGRN "Monitor thread: waiting to read counter\n" KNRM);
        sem_wait(&counter_locked); //Grants access
       
        int msgs = counter; //Saves it for later
        counter = 0;
        printf(KGRN "Monitor thread: reading a count value of %d\n" KNRM, msgs);
        sem_post(&counter_locked); //momken mcounter ylha2ha?

        sem_wait(&empty);
        //printf(KGRN " Waiting for buffer access\n" KNRM);

        sem_wait(&buffer_locked);

        if ((tail + 1) % b == head) { 
            printf(KGRN "Monitor thread: Buffer full!!!!!!!\n" KNRM);
            sem_post(&buffer_locked); // seb el buffer lw malyan
        
        } else {
            int i = tail % b;  //lazem before el enqueue 3lshan abl ma ytghyar
            enqueue(msgs);
            printf(KGRN "Monitor thread: writing to buffer at position %d\n" KNRM, i);
            //printf(KGRN "mMonitor: Added count %d to buffer\n" KNRM, msgs);
            sem_post(&buffer_locked);
            sem_post(&full);
        }
    }
}

void *mCollector(void *arg) { //Consumer for Monitor
    //reads the values from the buffer
    while (1) {
        sleep(rand() % 20);
        sem_wait(&full);
        // printf(KBLU "Waiting for buffer access\n" KNRM);
        
        sem_wait(&buffer_locked);

        if (head == tail) { 
            printf(KBLU "Collector thread: nothing is in the buffer!!!!!\n" KNRM);
            sem_post(&buffer_locked); // seb el buffer lw fady
        } else {
            int i = head % b;  //lazem before el dequeue 3lshan abl ma ytghyar
            int count = dequeue();
            printf(KBLU "Collector thread: reading from the buffer at position %d\n" KNRM, i);
            //printf(KBLU "mCollector: Read count %d from buffer\n" KNRM, count);
            sem_post(&buffer_locked);
            sem_post(&empty);
        }
    }
}


void intHandler(int dummy) {
	// set the noramal color back
    printf("%sExit\n", KNRM);
	// Destroy the semaphore 
	sem_destroy(&counter_locked); 
	sem_destroy(&buffer_locked); 
	sem_destroy(&full); 
	sem_destroy(&empty); 
	exit(0);
}



int main() {
    signal(SIGINT, intHandler);

    sem_init(&counter_locked, 0, 1); //Only 1 can access it
    sem_init(&buffer_locked, 0, 1);
    sem_init(&empty, 0, b);
    sem_init(&full, 0, 0);


    //N counter and 1 monitor and 1 collector 
    pthread_t threads[N_cnt + 2]; 
    for (int i = 0; i < N_cnt; i++) {
        pthread_create(&threads[i], NULL, mCounter, (void *)(intptr_t)i);
    }
    pthread_create(&threads[N_cnt], NULL, mMonitor, NULL);
    pthread_create(&threads[N_cnt + 1], NULL, mCollector, NULL);

    for (int i = 0; i < N_cnt + 2; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&counter_locked);
    sem_destroy(&buffer_locked);
    sem_destroy(&empty);
    sem_destroy(&full);

    printf("%sExit\n", KNRM);

    return 0;
}
