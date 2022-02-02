#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <semaphore.h>

#define NUM_JUGGLERS 11 //type 0
#define NUM_DANCERS 17 //type 1
#define NUM_SOLO 6 //type 2

// semaphores
sem_t mutex;
sem_t jug_lock;
sem_t dance_lock;
sem_t solo_lock;

// globals
int active_type = -1;
int slots; //empty: 2 for soloists, otherwise 4 (should start at 4)
pthread_t stage_space[4];

typedef struct {
    int type;
    int number;
    int ready_time;
} performer_t;

void * join_stage(void *input) {
    // cast and dereference input argument
    performer_t *info = (performer_t *) input;

    // getting ready
    sleep(info->ready_time);

    printf("I am %s #%d\n", ((info->type == 0) ? "Juggler" : (info->type == 1) ? "Dancer" : "Soloist"), info->number);
    sem_wait(&mutex);//Start crit section
    if (active_type == -1) {
        active_type = info->type;
        switch(info->type) {
            case 0: sem_post(&jug_lock); break;
            case 1: sem_post(&dance_lock); break;
            case 2: sem_post(&solo_lock); break;
        }
    }
    int r = 0;
    if (slots != 0) {
        while (r < slots) {
            if (stage_space[r] == NULL) {
                stage_space[r] = pthread_self();
                break;
            }
            else r++;
        }
    }
    else {
        printf("On stage: ");
        for (int n = 0; n < 4; n++) {
            printf("%ld",stage_space[n]);
        }
        printf("\n");
    }
    sem_post(&mutex);//End crit section
}

/*void leave_stage(int input) {
    // cast and dereference input argument
    printf("I am %s number %d\n", ((input == 0) ? "juggler" : (input == 1) ? "dancer" : "soloist"), 1);
    sem_wait(&mutex);//Start crit section
    sem_post(&mutex);//End crit section
}*/

/*void *juggler() {
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
}*/

void performer_init() {
    int num_performers = NUM_JUGGLERS + NUM_DANCERS + NUM_SOLO;
    pthread_t tid[num_performers];

    performer_t *performers = (performer_t *) malloc(num_performers * sizeof(performer_t));

    sem_init(&jug_lock, 0, 0);
    sem_init(&dance_lock, 0, 0);
    sem_init(&solo_lock, 0, 0);
    for (int i = 0; i < NUM_JUGGLERS; i++) {
        performer_t* p = performers + i;
        p->type = 0;
        p->number = i;
    }
    for (int j = 0; j < NUM_DANCERS; j++) {
        performer_t* p = performers + NUM_JUGGLERS + j;
        p->type = 1;
        p->number = j;
    }
    for (int k = 0; k < NUM_SOLO; k++) {
        performer_t* p = performers + NUM_JUGGLERS + NUM_DANCERS + k;
        p->type = 2;
        p->number = k;
    }

    // creating threads
    for (int thread = 0; thread < num_performers; thread++) {
        performer_t* p = performers + thread;
        p->ready_time = (rand() % 5) + 1;
        pthread_create(&tid[thread], NULL, &join_stage, p);
    }

    // joining threads
    for (int thread = 0; thread < num_performers; thread++) {
        pthread_join(tid[thread], NULL);
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