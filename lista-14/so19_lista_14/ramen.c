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

typedef struct ramen {
  /* TODO: Put internal state & mutexes & condvars here. */
  int count;
  int max;
  int done_eating;
  int at_the_table;
  pthread_mutex_t mutex;
  pthread_cond_t can_sit;
  pthread_cond_t everybody_finished_can_leave;
} ramen_t;

static void ramen_init(ramen_t *r, unsigned seats) {
  /* TODO: Initialize internal state of ramen restaurant. */
  r->count = 0;
  r->max = seats;
  r->done_eating = 0;
  r->at_the_table = 0;
  pthread_mutex_init(&r->mutex, NULL);
  pthread_cond_init(&r->can_sit, NULL);
  pthread_cond_init(&r->everybody_finished_can_leave, NULL);
}

static void ramen_destroy(ramen_t *r) {
  /* TODO: Destroy all synchronization primitives. */
  pthread_cond_destroy(&r->can_sit);
  pthread_cond_destroy(&r->everybody_finished_can_leave);
  pthread_mutex_destroy(&r->mutex);
}

static void ramen_wait(ramen_t *r) {
  /* TODO: Enter the restaurant unless it's full or people haven't left yet. */
  // outc('d');
  // printf("\n(%s) count %d max %d at_the_table %d done_eating %d\n",
  //   __func__, r->count, r->max, r->at_the_table, r->done_eating);
  pthread_mutex_lock(&r->mutex);
    r->count++;
    _Bool all_seats_occupied = r->at_the_table == r->max;
    while(all_seats_occupied)
    {
      pthread_cond_wait(&r->can_sit, &r->mutex);
      all_seats_occupied = r->max < r->count;
    }
    r->at_the_table++;
  outc('0'+r->at_the_table);
  pthread_mutex_unlock(&r->mutex);
}

static void ramen_finish(ramen_t *r) {
  /* TODO: Finished eating, so wait for all other to finish before leaving. */
  // printf("\n(%s1) count %d max %d at_the_table %d done_eating %d\n",
  //   __func__, r->count, r->max, r->at_the_table, r->done_eating);
  pthread_mutex_lock(&r->mutex);
    r->done_eating++;
    // printf("\n(%s2) count %d max %d at_the_table %d done_eating %d\n",
    //   __func__, r->count, r->max, r->at_the_table, r->done_eating);
    _Bool everybody_finished_can_leave = r->done_eating == r->at_the_table;
    if(everybody_finished_can_leave)
    {
      r->done_eating--;
      r->at_the_table--;
      assert(r->done_eating == r->at_the_table);
      pthread_cond_broadcast(&r->everybody_finished_can_leave);
      r->count--;
      if(r->at_the_table == 1)
        pthread_cond_broadcast(&r->can_sit);
    }
    else
    {
      everybody_finished_can_leave = r->done_eating == r->at_the_table;
      while(!everybody_finished_can_leave)
      {
        pthread_cond_wait(&r->everybody_finished_can_leave, &r->mutex);
        everybody_finished_can_leave = r->done_eating == r->at_the_table;
      }
      assert(r->done_eating == r->at_the_table);
      r->done_eating--;
      r->at_the_table--;
      r->count--;
    }
  pthread_mutex_unlock(&r->mutex);
}

void *customer(void *data) {
  ramen_t *r = data;

  seed = (unsigned)pthread_self();

  while (true) {
    /* Wait till you get hungry. */
    usleep(rand_r(&seed) % 5000 + 5000);

    /* Queue for delicious ramen. */
    outc('.');
    ramen_wait(r);

    /* It's so yummy! */
    outc('@');
    usleep(rand_r(&seed) % 1000 + 1000);
    outc('!');

    /* Time to leave, but don't be rude or else... */
    ramen_finish(r);
    outc('_');
  }
}

#define THREADS 10
#define SEATS 5

int main(void) {
  ramen_t r;
  ramen_init(&r, SEATS);

  pthread_t tid[THREADS];

  for (int i = 0; i < THREADS; i++)
    Pthread_create(&tid[i], NULL, customer, &r);

  for (int i = 0; i < THREADS; i++)
    Pthread_join(tid[i], NULL);

  ramen_destroy(&r);
  return 0;
}
