#include <pthread.h>
#include "caltrain.h"

void 
station_init(struct station *station) {
    station->numberOfEmptySeats = 0;
    station->numberOfWaitingPassengers = 0;
    station->numberOfPassengersWalkingOnTrain = 0;
    
    pthread_mutex_init(&station->lock, NULL);
    pthread_cond_init(&station->trainArrived, NULL);
    pthread_cond_init(&station->passengersSeated, NULL);
}

void 
station_load_train(struct station *station, int count) {
    pthread_mutex_lock(&station->lock);
    
    station->numberOfEmptySeats = count;
    
    // Wake up waiting passengers
    if (station->numberOfWaitingPassengers > 0 && count > 0) {
        pthread_cond_broadcast(&station->trainArrived);
    }
    
    while ((station->numberOfEmptySeats > 0 && station->numberOfWaitingPassengers > 0) || 
           (station->numberOfPassengersWalkingOnTrain > 0)) {
        pthread_cond_wait(&station->passengersSeated, &station->lock);
    }
    
    station->numberOfEmptySeats = 0;
    pthread_mutex_unlock(&station->lock);
}

void 
station_wait_for_train(struct station *station) {
    pthread_mutex_lock(&station->lock);
    
    station->numberOfWaitingPassengers++;
    
    // Wait until a train arrives with empty seats
    while (station->numberOfEmptySeats == 0) {
        pthread_cond_wait(&station->trainArrived, &station->lock);
    }

    station->numberOfWaitingPassengers--;
    station->numberOfPassengersWalkingOnTrain++;
    station->numberOfEmptySeats--; 
    
    pthread_mutex_unlock(&station->lock);
}

void 
station_on_board(struct station *station) {
    pthread_mutex_lock(&station->lock);

    station->numberOfPassengersWalkingOnTrain--;
    
    if (station->numberOfPassengersWalkingOnTrain == 0 && 
        (station->numberOfEmptySeats == 0 || station->numberOfWaitingPassengers == 0)) {
        pthread_cond_signal(&station->passengersSeated);
    }
    
    pthread_mutex_unlock(&station->lock);
}