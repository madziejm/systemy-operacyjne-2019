#include "csapp.h"

static __thread unsigned seed;

#define DEBUG
#ifdef DEBUG
static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}
#else
#define outc(c)
#endif

typedef struct ride {
  /* TODO: Put internal state & mutexes & condvars here. */
} ride_t;

static void ride_init(ride_t *r, unsigned seats) {
}

static void ride_destroy(ride_t *r) {
  /* TODO: Destroy all synchronization primitives. */
}

static void cart_load(ride_t *r) {
  /* TODO: Wait for all seats to be taken and depart. */
}

static void cart_unload(ride_t *r) {
  /* TODO: Wait for all passangers to leave and be ready for next ride. */
}

static void passenger_board(ride_t *r) { 
  /* TODO: Wait for the cart, enter it and wait for the ride to begin. */
}

static void passenger_unboard(ride_t *r) {
  /* TODO: Wait for the ride to end and leave the cart. */
}

static void *cart(void *data) {
  ride_t *r = data;

  seed = (unsigned)pthread_self();

  while (true) {
    outc('L');
    cart_load(r);

    usleep(rand_r(&seed) % 1000 + 1000);

    outc('U');
    cart_unload(r);
  }

  return NULL;
}

static void *passenger(void *data) {
  ride_t *r = data;

  seed = (unsigned)pthread_self();

  while(true) {   
    outc('-');
    passenger_board(r);

    usleep(rand_r(&seed) % 500 + 500);

    outc('!');
    passenger_unboard(r);

    outc('+');
    usleep(rand_r(&seed) % 2000 + 2000);
  }

  return NULL;
}

#define PASSENGERS 50
#define SEATS 10

int main(void) {
  ride_t r;
  ride_init(&r, SEATS);

  pthread_t cartThread;
  pthread_t passengerThread[PASSENGERS];

  pthread_create(&cartThread, NULL, cart, &r);
  for (int i = 0; i < 50; i++)
    pthread_create(&passengerThread[i], NULL, passenger, &r);

  pthread_join(cartThread, NULL);
  for (int i = 0; i < 50; i++)
    pthread_join(passengerThread[i], NULL);

  ride_destroy(&r);
  return 0;
}
