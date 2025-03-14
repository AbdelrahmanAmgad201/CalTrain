#include <pthread.h>

struct station {
    int numberOfEmptySeats;
    int numberOfWaitingPassengers;
    int numberOfPassengersWalkingOnTrain;
    
    pthread_mutex_t lock;
    pthread_cond_t trainArrived;
    pthread_cond_t passengersSeated;
};

void station_init(struct station *station);

void station_load_train(struct station *station, int count);

void station_wait_for_train(struct station *station);

void station_on_board(struct station *station);