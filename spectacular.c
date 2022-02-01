#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
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
pthread_t stage_space[4];

void join_stage(int input) {
    // cast and dereference input argument
    printf("I am %s number %d\n", ((input == 0) ? "juggler" : (input == 1) ? "dancer" : "soloist"), 1);
    sem_wait(&mutex);//Start crit section
    if (type == -1) type = input;
    switch(input) {
        case 0: sem_post(&jug_lock); break;
        case 1: sem_post(&dance_lock); break;
        case 2: sem_post(&solo_lock); break;
    }
    int r = 0;
    int y;
    sem_getvalue(&slots, &y);
    printf("%d",y);
    if (y != 0) {
        while (r < y) {
            if (stage_space[r] == NULL) {
                stage_space[r] = pthread_self();
                sem_wait(&slots);
                break;
            }
        }
    }
    else {
        printf("On stage:");
        for (int n = 0; n < 4; n++) {
            printf("%ld",stage_space[n]);
        }
    }
    sem_post(&mutex);//End crit section
}

/*void leave_stage(int input) {
    // cast and dereference input argument
    printf("I am %s number %d\n", ((input == 0) ? "juggler" : (input == 1) ? "dancer" : "soloist"), 1);
    sem_wait(&mutex);//Start crit section
    sem_post(&mutex);//End crit section
}*/

void *juggler() {
    join_stage(0);
    sem_wait(&jug_lock);
    sleep(1);
    printf("Juggled\n");
    sem_post(&jug_lock);
}

void *dancer() {
    join_stage(1);
    sem_wait(&dance_lock);
    sleep(1);
    printf("Danced");
    sem_post(&dance_lock);
}

void *soloist() {
    join_stage(2);
    sem_wait(&solo_lock);
    sleep(1);
    printf("Soloed");
    sem_post(&solo_lock);
}

void performer_init() {
    sem_init(&slots, 0, 4);
    sem_init(&jug_lock, 0, 0);
    sem_init(&dance_lock, 0, 0);
    sem_init(&solo_lock, 0, 0);
    for (int i = 0; i < 11; i++) {
        pthread_t jug;
        pthread_create(&jug, NULL, juggler, NULL);
        pthread_join(jug, NULL);
    }
    for (int j = 0; j < 17; j++) {
        pthread_t danc;
        pthread_create(&danc, NULL, dancer, NULL);
        pthread_join(danc, NULL);
    }
    for (int k = 0; k < 6; k++) {
        pthread_t solo;
        pthread_create(&solo, NULL, soloist, NULL);
        pthread_join(solo, NULL);
    }
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
    performer_init();
}