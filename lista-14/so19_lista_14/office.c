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

typedef struct office {
  /* TODO: Put internal state & mutexes & condvars here. */
} office_t;

static void office_init(office_t *o, unsigned seats) {
  /* TODO: Initialize internal state of post office. */
}

static void office_destroy(office_t *o) {
  /* TODO: Destroy all synchronization primitives. */
}

static bool customer_walk_in(office_t *o) {
  /* TODO: No seats then leave, otherwise wait for a clerk call. */
}

static void clerk_wait(office_t *o) {
  /* TODO: Wait for a customer or call one from a seat. */
}

static void clerk_done(office_t *o) {
  /* TODO: Tell the customer that the job is done. */
}

static void *customer(void *data) {
  office_t *b = data;

  seed = (unsigned)pthread_self();

  while (true) {
    if (customer_walk_in(b)) {
      /* Yay! I sent my registered mail :) */
      outc('+');
      /* I guess they'll force me to go there again... */
      usleep(rand_r(&seed) % 5000 + 5000);
    } else {
      /* Heck! No empty seats :( */
      outc('-');
      /* Try again in a while... */
      usleep(rand_r(&seed) % 500 + 500);
    }
  }

  return NULL;
}

static void *clerk(void *data) {
  office_t *b = data;

  seed = (unsigned)pthread_self();

  while (true) {
    /* Wait for customer to walk in or grab one that is seated. */
    clerk_wait(b);
    /* Do the paperwork! */
    usleep(rand_r(&seed) % 500 + 500);
    /* Another customer leaving happy? */
    clerk_done(b);
  }

  return NULL;
}

#define SEATS 4
#define CUSTOMERS 12

int main(void) {
  office_t o;
  office_init(&o, SEATS);

  pthread_t clerkThread;
  pthread_t customerThread[CUSTOMERS];

  Pthread_create(&clerkThread, NULL, clerk, &o);
  for (int i = 0; i < CUSTOMERS; i++)
    Pthread_create(&customerThread[i], NULL, customer, &o);

  pthread_join(clerkThread, NULL);
  for (int i = 0; i < CUSTOMERS; i++)
    Pthread_join(customerThread[i], NULL);

  office_destroy(&o);
  return 0;
}
