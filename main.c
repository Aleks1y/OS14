#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <semaphore.h>

#define SEM_COUNT 2
#define SUCCESS 0
#define FIRST_THREAD 1
#define SECOND_THREAD 2
#define FIRST_SEMAPHORE 0
#define SECOND_SEMAPHORE 1

void destroySemaphores(int count, sem_t *semaphores){
    for(int i = 0; i < count; ++i){
    	if(sem_destroy(&semaphores[i]) != SUCCESS){
    	    perror("Destoying semaphore error");
    	}
    }
}

void Print(int thread_number, sem_t *semaphore_for_wait, sem_t *semaphore_for_post){
    for(int i = 0; i < 10; ++i){
        if(sem_wait(semaphore_for_wait) != SUCCESS){
            perror("Semaphore wait error");
	    return;
        }

        printf("Thread № %d: %d\n", thread_number, i);
        
        if(sem_post(semaphore_for_post) != SUCCESS){
            perror("Semaphore post error");
	    return;
        } 
    }
}

void* secondPrint(void* param){
    sem_t* semaphores = (sem_t*)param;
    Print(SECOND_THREAD, &semaphores[SECOND_SEMAPHORE], &semaphores[FIRST_SEMAPHORE]);
    return NULL;
}

int main(int argc, char **argv){
    pthread_t thread;
    sem_t semaphores[SEM_COUNT];
    
    if(sem_init(&semaphores[FIRST_SEMAPHORE], 0, 1) != SUCCESS){
	perror("Can not initialize first semaphore");
    	exit(EXIT_FAILURE); 
    }
    
    if(sem_init(&semaphores[SECOND_SEMAPHORE], 0, 0) != SUCCESS){
	perror("Can not initialize second semaphore");
      	destroySemaphores(1, semaphores);
    	exit(EXIT_FAILURE); 
    }
	
    errno = pthread_create(&thread, NULL, secondPrint, semaphores);
    if(errno != SUCCESS){
	perror("Creating thread error");
	destroySemaphores(2, semaphores);
   	exit(EXIT_FAILURE); 
    }

    Print(FIRST_THREAD, &semaphores[FIRST_SEMAPHORE], &semaphores[SECOND_SEMAPHORE]);

    errno = pthread_join(thread,NULL);
    if(errno != SUCCESS){
    	perror("Thread join error");
   	destroySemaphores(2, semaphores);
   	exit(EXIT_FAILURE); 
    }

    destroySemaphores(2, semaphores);
    exit(0);
}
