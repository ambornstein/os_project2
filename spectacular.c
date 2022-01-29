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
sem_t pref_active;
pthread_t *stage_space[4];
int first_spot = 0;
int first_duet_spot = 0;

void performer_init() {
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
    printf("I am a %s", (class == 0) ? "Juggler" : (class == 1) ? "Dancer" : "Soloist");
    
    sem_wait(&mutex);//Start crit section
        switch (class)
        {
        case 0:
            break;
        case 1:
            break;
        case 2:
            break;
        
        default:
            
        }
    sem_post(&mutex);//End crit section
    if (class == 2) {
        
    }
    
    printf("I am %s number %d");
}


void main () {
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