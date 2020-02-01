#include "csapp.h"

static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}

static void randsleep(void) {
  usleep(rand() % 5000 + 5000);
}

#define N 50

static pthread_t td[N];
static sem_t forks[N];
/* TODO: If you need extra shared state, define it here. */
static sem_t free_forks_sem;

void *philosopher(void *id) {
  int right = (intptr_t)id;
  int left = right == 0 ? N - 1 : right - 1;

  for (;;) {
    /* Think */
    randsleep();

    /* TODO: Take forks (without deadlock & starvation) */
    Sem_wait(&free_forks_sem);
    Sem_wait(&forks[right]);
    Sem_wait(&forks[left]);

    /* Eat */
    outc('e');
    randsleep();

    /* TODO: Put forks (without deadlock & starvation) */
    Sem_post(&forks[left]);
    Sem_post(&forks[right]);
    Sem_post(&free_forks_sem);
  }

  return NULL;
}

int main(void) {
  /* TODO: If you need extra shared state, initialize it here. */
  Sem_init(&free_forks_sem, 0, N - 1);

  for (int i = 0; i < N; i++)
    Sem_init(&forks[i], 0, 1);

  for (int i = 0; i < N; i++)
    Pthread_create(&td[i], NULL, philosopher, (void *)(intptr_t)i);

  for (int i = 0; i < N; i++)
    Pthread_join(td[i], NULL);
  
  return EXIT_SUCCESS;
}
