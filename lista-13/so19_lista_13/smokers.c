#include "csapp.h"

static __unused void outc(char c) {
  Write(STDOUT_FILENO, &c, 1);
}

static __thread unsigned seed;

static sem_t tobacco;
static sem_t matches;
static sem_t paper;
static sem_t doneSmoking;

/* TODO: If you need any extra global variables, then define them here. */
static sem_t mutex;
static sem_t tobacco_sem;
static sem_t matches_sem;
static sem_t paper_sem;
_Bool is_paper;
_Bool is_match;
_Bool is_tobacco;

static void *agent(void *arg) {
  seed = pthread_self();

  while (true) {
    Sem_wait(&doneSmoking);

    int choice = rand_r(&seed) % 3;
    if (choice == 0) {
      Sem_post(&tobacco);
      Sem_post(&paper);
    } else if (choice == 1) {
      Sem_post(&tobacco);
      Sem_post(&matches);
    } else {
      Sem_post(&paper);
      Sem_post(&matches);
    }
  }

  return NULL;
}

/* TODO: If you need extra threads, then define their main procedures here. */

static void* tobacco_pusher(void* arg)
{
  while (true)
  {
    Sem_wait(&tobacco);
    Sem_wait(&mutex);
    if(is_paper)
    {
      is_paper = false;
      Sem_post(&matches_sem);
    }
    else if(is_match)
    {
      is_match = false;
      Sem_post(&paper_sem);
    }
    else
    {
      is_tobacco = true;
    }
    Sem_post(&mutex);
  }
  return NULL;
}

static void* paper_pusher(void* arg)
{
  while (true)
  {
    Sem_wait(&paper);
    Sem_wait(&mutex);
    if(is_tobacco)
    {
      is_tobacco = false;
      Sem_post(&matches_sem);
    }
    else if(is_match)
    {
      is_match = false;
      Sem_post(&tobacco_sem);
    }
    else
    {
      is_paper = true;
    }
    Sem_post(&mutex);
  }
  return NULL;
}

static void* matches_pusher(void* arg)
{
  while (true)
  {
    Sem_wait(&matches);
    Sem_wait(&mutex);
    if(is_paper)
    {
      is_paper = false;
      Sem_post(&tobacco_sem);
    }
    else if(is_tobacco)
    {
      is_tobacco = false;
      Sem_post(&paper_sem);
    }
    else
    {
      is_match = true;
    }
    Sem_post(&mutex);
  }
  return NULL;
}

static void randsleep(void) {
  usleep(rand_r(&seed) % 1000 + 1000);
}

static void make_and_smoke(char smoker) {
  randsleep();
  Sem_post(&doneSmoking);
  outc(smoker);
  randsleep();
}

static void *smokerWithMatches(void *arg) {
  seed = pthread_self();

  while (true) {
    /* TODO: wait for paper and tobacco */
    Sem_wait(&matches_sem);
    make_and_smoke('M');
  }

  return NULL;
}

static void *smokerWithTobacco(void *arg) {
  seed = pthread_self();

  while (true) {
    /* TODO: wait for paper and matches */
    Sem_wait(&tobacco_sem);
    make_and_smoke('T');
  }

  return NULL;
}

static void *smokerWithPaper(void *arg) {
  seed = pthread_self();
 
  while (true) {
    /* TODO: wait for tobacco and matches */
    Sem_wait(&paper_sem);
    make_and_smoke('P');
  }

  return NULL;
}

int main(void) {
  Sem_init(&tobacco, 0, 0);
  Sem_init(&matches, 0, 0);
  Sem_init(&paper, 0, 0);
  Sem_init(&doneSmoking, 0, 1);

  /* TODO: Initialize your global variables here. */
  is_paper = false;
  is_match = false;
  is_tobacco = false;
  Sem_init(&mutex, 0, 1);

  pthread_t agentThread;
  Pthread_create(&agentThread, NULL, agent, NULL);

  pthread_t smokerPaperThread, smokerMatchesThread, smokerTobaccoThread;
  Pthread_create(&smokerPaperThread, NULL, smokerWithPaper, NULL);
  Pthread_create(&smokerMatchesThread, NULL, smokerWithMatches, NULL);
  Pthread_create(&smokerTobaccoThread, NULL, smokerWithTobacco, NULL);

  pthread_t matches_pusher_thread, paper_pusher_thread, tobacco_pusher_thread;
  Pthread_create(&matches_pusher_thread, NULL, matches_pusher, NULL);
  Pthread_create(&paper_pusher_thread, NULL, paper_pusher, NULL);
  Pthread_create(&tobacco_pusher_thread, NULL, tobacco_pusher, NULL);

  Pthread_join(agentThread, NULL);
  Pthread_join(smokerPaperThread, NULL);
  Pthread_join(smokerMatchesThread, NULL);
  Pthread_join(smokerTobaccoThread, NULL);
  Pthread_join(matches_pusher_thread, NULL);
  Pthread_join(paper_pusher_thread, NULL);
  Pthread_join(tobacco_pusher_thread, NULL);

  return 0;
}
