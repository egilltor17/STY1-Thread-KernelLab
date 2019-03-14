#include "buffer.h"

#define LIGHTFILE "/sys/light/light"
FILE* light;


// ## Random sleep functions used in work functions.
long rand_sleep(int ms)
{
    long sleep_factor = rand() % 1000;
    long ret = (ms * 1000) + (sleep_factor * 1000);
    usleep( ret );
    return ret;
}

// ## INITILIZATION OF the RINGBUFFER ########################//
void buffer_init(unsigned int buffersize) {
     printf("initializing the circular buffer!\n");
     // ## Set the two thread run consditions 
     producers_run = 1; // while true producers loop.
     consumers_run = 1; // while true consumers loop.
     
     // ## Allocating the ringbuffer and initializing variabls. 
     num_slots = buffersize; // size of the buffer in slots.
     buff = (int*) malloc( num_slots*sizeof(int) );
     first_slot = 0;         // the front of circle
     last_slot = 0;          // the end of the circle
     free_slots = num_slots; // the number of empty slots
     
     // ## Initializing the thread-locking mechanisms 
     /******************************************************
      * MISSING CODE 2/6                                   *
      *                                                    *
      * NOTE!!! YOU MUST FIRST CREATE THE SEMAPHORES       *
      * IN buffer.h                                        *
      ******************************************************/
     Sem_init(&semCons, 0, 0);
     Sem_init(&semProd, 0, buffersize);
     Sem_init(&mutex, 0, 1);

     // ## Try to open the /sys/light/light file.
     if( (light = fopen(LIGHTFILE, "r+")) == NULL) { 
          // failed and thus we open a local directory file instead.
          if ( (light = fopen( "./light", "w+")) == NULL) {
               printf("Failed to open the light file :( \n");
               exit(-1);
          }
     }
     // As the buffer is empty we start with a green light.
     //             "R G B\n".
     fprintf(light, "0 1 0\n");
     fflush(light); // we must not buffer this output.
}

// ## DESTRUCTION OF the RINGBUFFER ########################//
void buffer_exit(void) {
     printf("\n\n\nThis party is over!!\n");
     rand_sleep(2000);
     printf("Turning off the lights.\n");
     fprintf(light, "0 0 0\n");
     fflush(light);
     printf("Goodbye and thanks of all the fish!\n\n");
     fclose(light);
     free(buff);
}

// ## The work function for producers #####################//
int produce(int i) {
    int ret = (int) (rand_sleep(max_sleep_time) / 1000);
    return ret;
}
// ## The work function for consumers #####################//
int consume(int i) {
    int ret = (int) (rand_sleep(max_sleep_time) / 1000);
    return ret;
}

// ## The main function for producer threads #############// 
void* producer( void* vargp ) {
  
  while(consumers_run) {
    if ( !free_slots ) {
          printf("The buffer is full :( \n");
           // As the buffer is full set the red light.
          //             "R G B\n".
          fprintf(light, "1 0 0\n");
          fflush(light);
     } else {
          // Neither full nor empty so we show blue/yellow.
          //             "R G B\n".
          fprintf(light, "0 0 1\n");
          fflush(light);
     }

     /******************************************************
      * MISSING CODE 3/6                                   *
      * HERE YOU MUST ADD THREAD SAFTY TO THE CODE BELOW   *
      ******************************************************/

     // ## if there is a free slot we produce to fill it.
     P(&semProd);
     P(&mutex);
     if( free_slots ) {
          int slot = last_slot;
          last_slot = last_slot + 1;  // filled a slot so move index
          if ( last_slot == num_slots ) {
               last_slot = 0;         // we must not go out-of-bounds.
          }
          free_slots = free_slots - 1; // one less free slots available
          V(&mutex);
          buff[slot] = produce(slot);
          P(&mutex);
          printf("producing for slot %d\n", slot);
     }
     V(&mutex);
     V(&semCons);
     
  } // end while

  return NULL;
}

// ## The main function for consumer threads #############// 
void* consumer( void* vargp ) {

  while (consumers_run) {
    if (num_slots - free_slots == 0){
          printf("The buffer is empty :( \n");
          // As the buffer is empty we start with a green light.
          //             "R G B\n".
          fprintf(light, "0 1 0\n");
          fflush(light);
     } else {
          // Neither full nor empty so we show a blue light.
          //             "R G B\n".
          fprintf(light, "0 0 1\n");
          fflush(light);
     }


     /******************************************************
      * MISSING CODE 4/6                                   *
      * HERE YOU MUST ADD THREAD SAFTY TO THE CODE BELOW   *
      ******************************************************/
     
     P(&semCons);
     P(&mutex);
     if (num_slots - free_slots) {
          int slot = first_slot;
          buff[first_slot] = -1;            // zero the slot consumed.
          first_slot = first_slot + 1;      // update buff index.
          if (first_slot == num_slots ) {
               first_slot = 0;              // we must not go out-of-bounds.
          }
          free_slots = free_slots + 1;      // one more free slots 
          V(&mutex);
          int cons = consume(buff[slot]);
          P(&mutex);
          printf("consuming from slot %d value: %d\n", slot, cons);
     }  
     V(&mutex);
     V(&semProd);
     
  } // end while
  return NULL;
}

pthread_t spawn_producer( thread_info *arg )
{
     printf("Spawning thread %d as a producer \n", arg->thread_nr);
     fflush(stdout);
     /******************************************************
      * MISSING CODE 5/6                                   *
      * HERE YOU MUST CREATE A producer THREAD HERE        *
      ******************************************************/
     pthread_t tid;
     int s;
     Pthread_create(&tid, NULL, producer, &s);
     return tid;
}

pthread_t spawn_consumer( thread_info *arg )
{
     printf("Spawning thread %d as a consumer\n", arg->thread_nr);
     fflush(stdout);
     /******************************************************
      * MISSING CODE 6/6                                   *
      * HERE YOU MUST CREATE A consumer THREAD HERE        *
      ******************************************************/
     pthread_t tid;
     int s;
     Pthread_create(&tid, NULL, consumer, &s);
     return tid;
}
