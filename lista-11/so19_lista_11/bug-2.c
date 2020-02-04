/* WARNING: This code is buggy! */
#include "csapp.h"
#define N 4

#define BUGGY

static void *thread(void *vargp) {
  int myid = *((int *)vargp);
  printf("Hello from thread %d\n", myid);
  return NULL;
}

int main(void) {
  pthread_t tid[N];
  int i;

  // #ifdef BUGGY
  // // initial version
  // for (i = 0; i < N; i++)
  //   Pthread_create(&tid[i], NULL, thread, &i);
  // for (i = 0; i < N; i++)
  //   Pthread_join(tid[i], NULL);
  // #endif

  // #ifndef BUGGY
  // int args[N];
  // for (i = 0; i < N; i++)
  //   args[i] = i;
  

  for (i = 0; i < N; i++)
  {
    // printf("starting %d\n", i);
    Pthread_create(&tid[i], NULL, thread,  &i);

  }
  for (i = 0; i < N; i++)
  {
    printf("joining %d\n", i);
    Pthread_join(tid[i], NULL);
    printf("joined %d\n", i);
  }
  // #endif

  return EXIT_SUCCESS;
}
