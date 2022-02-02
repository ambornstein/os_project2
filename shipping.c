#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <pthread.h>

#define TEAMSPERGROUP 4
#define MEMBERSPERTEAM 10

// mutex and cond vars
pthread_mutex_t mutex;
pthread_cond_t next_red; //0
pthread_cond_t next_blue; //1
pthread_cond_t next_yellow; //2
pthread_cond_t next_green; //3

// structs
typedef struct {
    int team;
    int number;
    int sleep_time;
} worker_t;

typedef struct {
    int id;
    int instructions[4];
} package_t;

// global vars
int active_red = -1;
int active_blue = -1;
int active_yellow = -1;
int active_green = -1;
package_t* PPP;

void * start_work(void* input) {
    // get worker info
    worker_t* info = (worker_t *) input;
    // random sleep
    sleep(info->sleep_time);

    // get team name
    char* team_name;
    pthread_cond_t* team_queue;
    int* active;
    switch(info->team) {
        case 0:
            team_name = "Red";
            team_queue = &next_red;
            active = &active_red;
            break;
        case 1:
            team_name = "Blue";
            team_queue = &next_blue;
            active = &active_blue;
            break;
        case 2:
            team_name = "Yellow";
            team_queue = &next_yellow;
            active = &active_yellow;
            break;
        default:
            team_name = "Green";
            team_queue = &next_green;
            active = &active_green;
            break;
    }

    // introduction
    //printf("Hello! I am %s %d\n", team_name, info->number);


    // check if first
    pthread_mutex_lock(&mutex);
    // critical region starts
    if (*active == -1) {
       *active = info->number;
        pthread_mutex_unlock(&mutex);
        printf("I'm first! (%s #%d)\n", team_name, info->number);
    }
    else {
        pthread_cond_wait(team_queue, &mutex);

        // woken as new active thread
        *active = info->number;
        pthread_mutex_unlock(&mutex);
        printf("I'm awake! (%s #%d)\n", team_name, info->number);
    }
    // critical region ends

    // sleep to make sure all threads have loaded
    sleep(1);

    // wake next thread in line
    pthread_mutex_lock(&mutex);
    pthread_cond_signal(team_queue);
    pthread_mutex_unlock(&mutex);

    printf("(%s #%d) through the critical section\n", team_name, info->number);

    return NULL;
}

void team_init() {
    pthread_t tid[TEAMSPERGROUP * MEMBERSPERTEAM];
    worker_t *members = (worker_t *) malloc(TEAMSPERGROUP * MEMBERSPERTEAM * sizeof(worker_t));

    for (int team = 0; team < TEAMSPERGROUP; team++) {
        for (int number = 0; number < MEMBERSPERTEAM; number++) {
            int workernum = team * MEMBERSPERTEAM + number;
            worker_t* w = members + workernum;
            w->team = team;
            w->number = number;
            w->sleep_time = (rand() % 5) + 1;
            pthread_create(&tid[workernum], NULL, &start_work, w);
        }
    }

    for (int i = 0; i < TEAMSPERGROUP * MEMBERSPERTEAM; i++) {
        pthread_join(tid[i], NULL);
    }
    printf("red: %d, blue: %d, yellow: %d, green: %d\n", active_red, active_blue, active_yellow, active_green);
}

void ppp_init() {
    // 5 more packages than workers
    int num_packages = TEAMSPERGROUP * MEMBERSPERTEAM + 5;

    PPP = (package_t *) malloc(num_packages * sizeof(package_t));

    for (int package = 0; package < num_packages; package++) {
        package_t* p = PPP + package;
        p->id = package;
        p->instructions;
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

    // Initializing vars
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&next_red, NULL);
    pthread_cond_init(&next_blue, NULL);
    pthread_cond_init(&next_yellow, NULL);
    pthread_cond_init(&next_green, NULL);

    team_init();
}