#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>

#define NUM_JUGGLERS 11; //type 0
#define NUM_DANCERS 17; //type 1
#define NUM_SOLO 6; //type 2

sem_t mutex;
sem_t slots; //empty: 2 for soloists, otherwise 4 (should start at 4)
sem_t jug_lock;
sem_t dance_lock;
sem_t solo_lock;
int type = -1;
pthread_t *stage_space[4];

void performer_init() {
    sem_init(&slots, 0, 4);
    sem_init(&jug_lock, 0, 0);
    sem_init(&dance_lock, 0, 0);
    sem_init(&solo_lock, 0, 0);
    for (int i=0; i < NUM_JUGGLERS; i++) {
        pthread_t preformer;
        Pthread_create(&preformer, NULL, join_stage, 0);
        pthread_join(preformer, NULL);
    }
    for (int j=0; j < NUM_DANCERS; j++) {
        pthread_t preformer;
        Pthread_create(&preformer, NULL, join_stage, 1);
        pthread_join(preformer, NULL);
    }
    for (int k=0; k < NUM_SOLO; k++) {
        pthread_t preformer;
        Pthread_create(&preformer, NULL, join_stage, 2);
        pthread_join(preformer, NULL);
    }
}

void join_stage(void *input) {
    // sleep upon creation
    sleep(rand()%8 + 2);

    // cast and dereference input argument
    int class = *(int *) input;
    //sem_wait(&slots);
    sem_wait(&mutex);//Start crit section
    if (type == -1) {
        type = class;
        if (type == 0) sem_post(&jug_lock);
        else if (type = 1) sem_post(&dance_lock);
    }
        switch (class)
        {
        case 0:
            // sem_wait(&jug_lock);
            sem_wait(&slots);
            sleep(rand()%5);
            sem_post(&jug_lock);
            
            break;
        case 1:
            // sem_wait(&dance_lock);
            break;
        case 2:
            // sem_wait(&solo_lock);
            break;
        
        default:
            
        }
    sem_post(&mutex);//End crit section
    if (class == 2) {
        
    }
    
    printf("I am %s number %d", ((class == 0) ? "juggler" : (class == 1) ? "dancer" : "soloist"), 1);
}


int main () {
    // Getting random seed
    char buffer[5];
    int childnum;
    int file = open("seed.txt", O_RDONLY);
    read(file, buffer, 5);
    int seed = atoi(buffer);
    printf("Read seed value: %d\n", seed);
    srand(seed);
    sem_init(&mutex, 0, 1);
    
}