#include "csapp.h"

static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}

typedef struct {
  /* TODO: Use this structure to store barrier internal state. */
  int n;
  size_t count;
  sem_t bramka;
  sem_t bramka2;
  sem_t mutex;
} barrier_t;

static barrier_t *barrier_init(int n) {
  if (n < 1)
    app_error("barrier_init requires n > 0");

  barrier_t *b = Mmap(NULL, sizeof(barrier_t), PROT_READ|PROT_WRITE,
                      MAP_ANON|MAP_SHARED, -1, 0);

  /* TODO: Initialize barrier internal state. */
  b->n = n;
  b->count = 0;
  Sem_init(&b->bramka, 1, n);
  Sem_init(&b->bramka2, 1, 0);
  Sem_init(&b->mutex, 1, 1);

  return b;
}

static void barrier_wait(barrier_t *b) {
  /* TODO: Provide wait procedure implementation here. */
  Sem_wait(&b->bramka);
  Sem_wait(&b->mutex);
  b->count += 1;
  // printf("[%d] count: %ld\n", getpid(), b->count);
  if (b->count == b->n) {
    // outc('\n');
    for(size_t k = 0; k < b->n - 1; k++)
      Sem_post(&b->bramka2);
    b->count = 0;
    Sem_post(&b->mutex);
    for(size_t k = 0; k < b->n; k++)
      Sem_post(&b->bramka);
  }
  else
  {
  Sem_post(&b->mutex);
  Sem_wait(&b->bramka2);
  }
}

static void barrier_destroy(barrier_t *b) {
  /* TODO: Provide destroy procedure implementation here. */
  Sem_destroy(&b->mutex);
  Sem_destroy(&b->bramka);
  Sem_destroy(&b->bramka2);
  Munmap(b, sizeof(barrier_t));
}

#define K 100
#define N 50
#define P 100

static noreturn void horse(barrier_t *b) {
  int n = rand() % K + K;

  outc('+');

  for (int i = 0; i < n; i++) {
    barrier_wait(b);
    usleep(rand() % 2000 + 1000);
  }

  outc('-');

  exit(EXIT_SUCCESS);
}

/* Do not bother cleaning up after this process. Let's assume that controlling
 * terminal sends SIGINT to the process group on CTRL+C. */
int main(void) {
  barrier_t *b = barrier_init(N);

  int horses = 0;

  for (;;) {
    do {
      if (Fork() == 0)
        horse(b);
      horses++;
    } while (horses < P);

    Wait(NULL);
    horses--;
  }

  barrier_destroy(b);

  return EXIT_SUCCESS;
}
