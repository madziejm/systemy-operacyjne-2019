#include "csapp.h"

static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}

#define N 100
#define M 100

static struct {
  /* TODO: Put semaphores and shared variables here. */
  int servings;
  sem_t mutex;
  sem_t empty_pot;
  sem_t full_pot;
} *shared = NULL;


static void savage(void) {
  for (;;) {
    /* TODO Take a meal or wait for it to be prepared. */
    /* Sleep and digest. */
    Sem_wait(&(shared->mutex));
    if(shared->servings == 0)
    {
      Sem_post(&(shared->empty_pot));
      Sem_wait(&(shared->full_pot));
    }
    shared->servings--;
    // outc('s');
    Sem_post(&(shared->mutex));
    usleep(rand() % 1000 + 1000);
  }

  exit(EXIT_SUCCESS);
}

static void cook(void) {
  for (;;) {
    /* TODO Cook is asleep as long as there are meals.
     * If woken up they cook exactly M meals. */
    assert(shared->servings == 0);
    Sem_wait(&(shared->empty_pot));
    // outc('c');
    shared->servings += M;
    Sem_post(&(shared->full_pot));
  }
}

/* Do not bother cleaning up after this process. Let's assume that controlling
 * terminal sends SIGINT to the process group on CTRL+C. */
int main(void) {
  shared = Mmap(NULL, getpagesize(), PROT_READ|PROT_WRITE, MAP_ANON|MAP_SHARED,
                -1, 0);
  shared->servings = M;
  Sem_init(&(shared->mutex), 1, 1);
  Sem_init(&(shared->empty_pot), 1, 0);
  Sem_init(&(shared->full_pot), 1, 0);
  /* TODO: Initialize semaphores and other shared state. */

  for (int i = 0; i < N; i++)
    if (Fork() == 0)
      savage();

  cook();

  return EXIT_SUCCESS;
}
