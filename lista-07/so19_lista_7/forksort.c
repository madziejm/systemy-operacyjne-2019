#include "csapp.h"

#define FORKSORT

#ifdef __GNUC__
#define VARIABLE_IS_NOT_USED __attribute__ ((unused))
#else
#define VARIABLE_IS_NOT_USED
#endif

static void InsertionSort(long table[], size_t left, size_t right) {
  for (size_t pivot = left + 1; pivot <= right; pivot++) {
    size_t insert = left;

    while ((insert < pivot) && (table[insert] < table[pivot]))
      insert++;

    if (insert < pivot) {
      size_t n = pivot - insert;
      long tmp = table[pivot];
      memmove(&table[insert + 1], &table[insert], n * sizeof(long));
      table[insert] = tmp;
    }
  }
}

static void SwapElem(long table[], size_t i, size_t j) {
  long tmp = table[i];
  table[i] = table[j];
  table[j] = tmp;
}

static size_t Partition(long table[], size_t begin, size_t end, long pivot) {
  size_t left = begin;
  size_t right = end;

  while (left < right) {
    while ((left < right) && (table[left] < pivot))
      left++;

    while ((left < right) && (table[right] >= pivot))
      right--;

    if (left < right)
      SwapElem(table, left, right);
  }

  return left;
}

#define INSERTSORT_MAX 32
#define FORKSORT_MIN (1L << 18)

static int QuickSort(long table[], size_t left, size_t right) {
  // return -1 if not in subprocess and getpid() otherwise
  pid_t VARIABLE_IS_NOT_USED pid_left = -1, VARIABLE_IS_NOT_USED pid_right = -1, pid = -1;

  /* TODO: If there is more to sort than FORKSORT_MIN start a subprocess. */
  if (left < right) {
    if (right - left <= INSERTSORT_MAX) {
      InsertionSort(table, left, right);
    } else {
      #ifdef FORKSORT
      if (FORKSORT_MIN <= right - left)
      {
        pid_t child_pid = Fork();
        if(child_pid == 0) // child
        {
          // 1;
          pid = getpid();
        }
        else // parent (assume Fork succeded)
        {
          return child_pid;
        //   // sort normally you bastard
        }
      }
      #endif
      size_t pivot = left + random() % (right - left + 1);
      size_t split = Partition(table, left, right, table[pivot]);

      if (left == split) {
        SwapElem(table, left, pivot);
        split++;
      } else {
        pid_left = QuickSort(table, left, split - 1);
      }

      pid_right = QuickSort(table, split, right);

      /* TODO: Wait for possible children and exit if created a subprocess. */
      int status;
      if(pid_left != -1)
        Waitpid(pid_left, &status, 0);
      if(pid_right != -1)
        Waitpid(pid_right, &status, 0);
      #ifdef FORKSORT
      if (pid != -1)
      {
        exit(0);
      }
      #endif
    }
  }

  return pid;
}

#define NELEMS (1L << 26)

int main(void) {
  /* Table size in bytes must be divisible by page size. */
  size_t size = NELEMS * sizeof(long);
  assert((size & (getpagesize() - 1)) == 0);

  /* Initialize PRNG seed. */
  struct timeval tv;
  gettimeofday(&tv, NULL);
  srandom(tv.tv_usec);

  /* TODO: Allocate table... */
  long *table = (long*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  perror("mmap error");

  /* ... and fill it in with random elements. */
  for (size_t i = 0; i < NELEMS; i++)
    table[i] = random();

  /* Sort it using hybrid algorithm... */
  if (QuickSort(table, 0, NELEMS - 1) > 0)
    Wait(NULL);

  /* ... and verify if the table is actually sorted. */
  long prev = LONG_MIN;
  for (size_t i = 0; i < NELEMS; i++) {
    assert(prev <= table[i]);
    prev = table[i];
  }
  
  printf("SUCCESS!\n");
  return 0;
}
