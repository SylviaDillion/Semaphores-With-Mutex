#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// Constant declaration.
#define PLANE_NUMBER 3
#define SAFETY_INSPECTION_DURATION 20
#define PASSENGER_BOARDING_DURATION 25
#define TAXI_OUT_DURATION 20
#define PLANE_NAME_LENGTH 50

sem_t runway_semaphore;

typedef struct {
    char name[PLANE_NAME_LENGTH];
    int taking_off_time;
    pthread_t thread;
} Airplane;

void initialize_airplane(Airplane * plane, int id);
void * preparation(void * args);
void * request_and_takeoff(void * args);
int random_number_in_range(int min, int max);

int main() {
    sem_init(&runway_semaphore, 0, 1); // Initialize semaphore
    srand((unsigned int)time(NULL));
    setbuf(stdout, NULL);

    // Initialize planes.
    Airplane plane[PLANE_NUMBER];
    for (int i = 0; i < PLANE_NUMBER; i++) {
        initialize_airplane(&plane[i], i + 1);
    }

    // Preparation stage.
    for (int i = 0; i < PLANE_NUMBER; i++) {
        pthread_create(&plane[i].thread, NULL, &preparation, &plane[i]);
    }

    for (int i = 0; i < PLANE_NUMBER; i++) {
        pthread_join(plane[i].thread, NULL);
    }

    printf("\nAirplanes finish preparation simultaneously,"
           "they will all simultaneously send a request to"
           "the control tower to use runway\n\n");

    // Planes are taking off.
    for (int i = 0; i < PLANE_NUMBER; i++) {
        pthread_create(&plane[i].thread, NULL, &request_and_takeoff, &plane[i]);
    }

    for (int i = 0; i < PLANE_NUMBER; i++) {
        pthread_join(plane[i].thread, NULL);
    }

    // Free resources associated with semaphore.
    sem_destroy(&runway_semaphore);
    return 0;
}

/**
 * @brief Initilize an `Airplane` instance.
 *
 * Each `taking_off_time` is generated randomly between [5, 10].
 *
 * @param plane the `Airplane` to be initialized.
 * @param id a unique ID used to identify the plane.
 */
void initialize_airplane(Airplane * plane, int id) {
    sprintf(plane->name, "airplane%d", id);
    plane->taking_off_time = random_number_in_range(5, 10);
}

/**
 * @brief Handle preparation stage of an `Airplane`, including three phrases:
 *
 * - Safety inspection
 * - Passenger boarding
 * - Taxi-out
 *
 * @param args can be converted into `Airplane` type.
 */
void * preparation(void * args) {
    Airplane * plane = (Airplane *)args;

    printf("%s starts safety inspection.\n", plane->name);
    sleep(SAFETY_INSPECTION_DURATION);
    printf("%s finishes safety inspection.\n", plane->name);

    printf("%s starts passengers boarding.\n", plane->name);
    sleep(PASSENGER_BOARDING_DURATION);
    printf("%s finishes passengers boarding.\n", plane->name);

    printf("%s starts taxi-out.\n", plane->name);
    sleep(TAXI_OUT_DURATION);
    printf("%s finishes taxi-out.\n", plane->name);

    return NULL;
}

/**
 * @brief Plane requests and uses the runway to take off.
 *
 * @param args can be converted into `Airplane` type.
 */
void * request_and_takeoff(void * args) {
    Airplane * plane = (Airplane *)args;

    // The plane sends a takeoff request to the control tower after
    // finishing preparation.
    // Since all planes have the same preparation time, this step reflects
    // planes sending requests simultaneously.
    printf("%s sends a request to the control tower for takeoff.\n",
           plane->name);

    // Wait for the runway resource. The semaphore acts like a mutex lock here.
    // Only the plane that acquires hte semaphore can proceed, while the others
    // are blocked here util the current plane release it.
    sem_wait(&runway_semaphore);

    // Successfully acquiring the semaphore is treated as the tower granting
    // takeoff permission to this plane.
    printf("Control tower grants takeoff permission to %s\n", plane->name);

    // The plane is taking off.
    printf("%s is taking off.\n", plane->name);
    sleep(plane->taking_off_time);
    printf("%s finished takeoff. Time taken %d mins\n", plane->name,
           plane->taking_off_time);

    // Takeoff complete, and release the runway resource.
    sem_post(&runway_semaphore);

    return NULL;
}

/**
 * @brief Generate a random number between `min` and `max` (inclusive).
 *
 * @param min lower boundary
 * @param max upper boundary
 * @return a random number between `min` and `max` (inclusive).
 */
int random_number_in_range(int min, int max) {
    return min + rand() % (max - min + 1);
}
