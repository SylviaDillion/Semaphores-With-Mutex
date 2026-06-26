#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

sem_t semaphore;
pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct {
    char name[50];
    bool safty_inspection;
    bool passengers_boarding;
    bool taxi_out; // The taxiing process to the runway
    pthread_t take_off;
} Airplane;

void initializeAirplne(Airplane * plane, int i) {
    sprintf(plane->name, "airplane%d", i);
    plane->safty_inspection = false;
    plane->passengers_boarding = false;
    plane->taxi_out = false;
}

void * preparation(void * args) {
    Airplane * p = (Airplane *)args;
    pthread_mutex_lock(&print_mutex);
    printf("%s finishes safety inspection.\n", p->name);
    pthread_mutex_unlock(&print_mutex);
    usleep(200);

    pthread_mutex_lock(&print_mutex);
    printf("%s finishes passengers boarding.\n", p->name);
    pthread_mutex_unlock(&print_mutex);
    usleep(250);

    pthread_mutex_lock(&print_mutex);
    printf("%s finishes taxi-out.\n", p->name);
    pthread_mutex_unlock(&print_mutex);
    usleep(200);

    return NULL;
}
void * runway(void * args) {

    sem_wait(&semaphore);
    printf("-----------------------------------\n");
    Airplane * p = (Airplane *)args;
    printf("%s are taking off.\n", p->name);

    int random_num = (rand() % (10 - 5 + 1)) + 5;
    sleep(random_num);

    printf("%s finished. Time taken %d mins\n", p->name, random_num);
    printf("-----------------------------------\n");
    sem_post(&semaphore);

    return NULL;
}

int main() {
    sem_init(&semaphore, 0, 1); // Initialize semaphore
    srand((unsigned int)time(NULL));

    Airplane plane[3];
    for (int i = 0; i < 3; i++)
        initializeAirplne(&plane[i], i + 1);

    for (int i = 0; i < 3; i++)
        pthread_create(&plane[i].take_off, NULL, &preparation, &plane[i]);

    for (int i = 0; i < 3; i++)
        pthread_join(plane[i].take_off, NULL);

    printf("Airplanes finish preparation simultaneously,"
           "they will all simultaneously send a request to"
           "the control tower to use runway\n");

    for (int i = 0; i < 3; i++)
        pthread_create(&plane[i].take_off, NULL, &runway, &plane[i]);
    for (int i = 0; i < 3; i++)
        pthread_join(plane[i].take_off, NULL);

    sem_destroy(&semaphore);
    pthread_mutex_destroy(&print_mutex);
    return 0;
}
