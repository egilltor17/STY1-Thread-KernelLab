#ifndef BUFFER_H
#define BUFFER_H
#include "csapp.h"

#define max_sleep_time 2000

struct thread_info {
    int thread_nr;
} typedef thread_info;

// ## Variables needed for the buffer logic. #################//
int * buff;
int first_slot;
int last_slot;
int free_slots;
int num_slots;

// ## Halting condition for threads ##########################//
int producers_run;
int consumers_run;

// ## Declaration of locking mechanisms (semaphors) ##########//
/******************************************************
 * MISSING CODE 1/6                                   *
 *                                                    *
 * HERE YOU ADD THE DECLARATIONS OF THE LOCKS         *
 * (semaphors) YOU NEED. THEY ARE HOWEVER INITIALIZED * 
 * IN THE IMPL. OF buffer_init() IN buffer.c          *
 ******************************************************/

sem_t semCons;
sem_t semProd;
sem_t mutex;

static long counter = 0;

// ## Random sleep function in milliseconds ##################//
long rand_sleep(int ms);

// ## Initilization and destruction of buffer ################//
void buffer_init(unsigned int);
void buffer_exit(void);

// ## Thread functions #######################################//
void* producer( void* vargp  );
void* consumer( void* vargp  );

// ## Work functions #########################################//
int  produce(int);
int  consume(int);

// ## Functions to create new threads ########################//
pthread_t spawn_producer(thread_info *arg);
pthread_t spawn_consumer(thread_info *arg);

#endif /* BUFFER_H */
