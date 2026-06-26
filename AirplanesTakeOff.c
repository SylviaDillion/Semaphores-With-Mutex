#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define PLANE_NUMBER 3
#define SAFETY_INSPECTION_DURATION 20
#define PASSENGER_BOARDING_DURATION 25
#define TAXI_OUT_DURATION 20
#define PLANE_NAME_LENGTH 50

sem_t semaphore;

typedef struct {
    char name[PLANE_NAME_LENGTH];
    int taking_off_time;
    pthread_t take_off;
} Airplane;

void initialize_airplane(Airplane * plane, int id);
void * preparation(void * args);
void * runway(void * args);
int random_number_in_range(int min, int max);

int main() {
    sem_init(&semaphore, 0, 1); // Initialize semaphore
    srand((unsigned int)time(NULL));

    Airplane plane[PLANE_NUMBER];
    for (int i = 0; i < PLANE_NUMBER; i++) {
        initialize_airplane(&plane[i], i + 1);
    }

    for (int i = 0; i < PLANE_NUMBER; i++) {
        pthread_create(&plane[i].take_off, NULL, &preparation, &plane[i]);
    }

    for (int i = 0; i < PLANE_NUMBER; i++) {
        pthread_join(plane[i].take_off, NULL);
    }

    printf("Airplanes finish preparation simultaneously,"
           "they will all simultaneously send a request to"
           "the control tower to use runway\n");

    for (int i = 0; i < PLANE_NUMBER; i++) {
        pthread_create(&plane[i].take_off, NULL, &runway, &plane[i]);
    }

    for (int i = 0; i < PLANE_NUMBER; i++) {
        pthread_join(plane[i].take_off, NULL);
    }

    sem_destroy(&semaphore);
    return 0;
}

void initialize_airplane(Airplane * plane, int id) {
    sprintf(plane->name, "airplane%d", id);
    plane->taking_off_time = random_number_in_range(5, 10);
}

void * preparation(void * args) {
    Airplane * p = (Airplane *)args;

    printf("%s starts safety inspection.\n", p->name);
    sleep(SAFETY_INSPECTION_DURATION);
    printf("%s finishes safety inspection.\n", p->name);

    printf("%s starts passengers boarding.\n", p->name);
    sleep(PASSENGER_BOARDING_DURATION);
    printf("%s finishes passengers boarding.\n", p->name);

    printf("%s starts taxi-out.\n", p->name);
    sleep(TAXI_OUT_DURATION);
    printf("%s finishes taxi-out.\n", p->name);

    return NULL;
}

void * runway(void * args) {
    sem_wait(&semaphore);
    Airplane * p = (Airplane *)args;

    printf("%s are taking off.\n", p->name);
    sleep(p->taking_off_time);
    printf("%s finished. Time taken %d mins\n", p->name, p->taking_off_time);

    sem_post(&semaphore);
    return NULL;
}

int random_number_in_range(int min, int max) {
    return min + rand() % (max - min + 1);
}
