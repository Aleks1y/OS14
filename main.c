#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <semaphore.h>

#define SEM_COUNT 2
#define SUCCESS 0

void destroySemaphores(int count, sem_t* semaphores){
  for(int i = 0; i < count; ++i){
    errno = sem_destroy(&semaphores[i]);
    if(errno != SUCCESS){
    	perror("Destoying semaphore error");
  }
}

void Print(sem_t* semaphore_for_wait, sem_t* semaphore_for_post){
    for(int i = 0;i < 10; ++i){
        errno = sem_wait(semaphore_for_wait);
        if(errno != SUCCESS){
            perror("Semaphore wait error");
	    return;
        }

        printf("Thread № %d: %d\n", thread_number, i);
        
        errno = sem_post(semaphore_for_post);
        if(errno != SUCCESS){
            perror("Semaphore post error");
	    return;
        } 
    }
}

void* secondPrint(void* param){
    sem_t* semaphores = (sem_t*)param;
    Print(&semaphores[2], &semaphores[1]);
    return NULL;
}

int main(void){
    pthread_t thread;
    sem_t semaphores[SEM_COUNT];
    
    errno = initSem(&semaphores[1], 0, 1);
    if(errno != SUCCESS){
	    perror("Can not initialize first semaphore");
    	exit(EXIT_FAILURE); 
    }
    
    errno = initSem(&semaphores[2], 0, 0);
    if(errno != SUCCESS){
	perror("Can not initialize second semaphore");
      	destroySemaphores(1, semaphores);
    	exit(EXIT_FAILURE); 
    }
	
    errno = pthread_create(&thread, NULL, secondPrint, semaphores);
    if(errno != SUCCESS){
	  perror("Creating thread error");
	  destroySemaphores(SEM_COUNT, semaphores);
   	  exit(EXIT_FAILURE); 
    }

    Print(&semaphores[2], &semaphores[1]);

    errno = pthread_join(thread,NULL);
    if(errno != SUCCESS){
	  perror("Thread join error");
   	  destroySemaphores(SEM_COUNT, semaphores);
   	  exit(EXIT_FAILURE); 
    }

    destroySemaphores(SEM_COUNT, semaphores);
    exit(0);
}
