#include "csapp.h"
#include "pipeio.h"

static bool ReadNum(pipe_t p, long *valp) {
  return Read(p.read, valp, sizeof(long)) == sizeof(long);
}

static bool WriteNum(pipe_t p, long val) {
  return Write(p.write, &val, sizeof(long)) == sizeof(long);
}

static noreturn void generator(pipe_t out, long maxprime) {
  for (long n = 2; n <= maxprime; n++)
    WriteNum(out, n);
  exit(EXIT_SUCCESS);
}

static void filter(pipe_t in, pipe_t out, long prime) {
  long num;
  while (ReadNum(in, &num)) {
    if (num % prime != 0)
      WriteNum(out, num);
  }
}

static noreturn void filter_chain(pipe_t in) {
  long prime;

  /* TODO: Something is missing here! */

  if(ReadNum(in, &prime))
  {
    printf("%ld\n", prime);
    pipe_t me_to_child_pipe = MakePipe();
    pid_t child_pid = Fork();
    if(child_pid != 0) // parent (Fork() exits on failure)
    {
      CloseReadEnd(me_to_child_pipe);
      filter(in, me_to_child_pipe, prime);
      CloseWriteEnd(me_to_child_pipe);
      int status;
      Waitpid(child_pid, &status, 0);
    }
    else // child
    {
      CloseWriteEnd(me_to_child_pipe);
      filter_chain(me_to_child_pipe);
    }
  }
  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
  if (argc != 2)
    app_error("Usage: %s [MAXPRIME]", argv[0]);

  long maxprime = atol(argv[1]);

  if (maxprime < 2 || maxprime > 10000)
    app_error("Give maximum prime number in range from 2 to 10000!");

  /* Spawn generator. */
  pipe_t gen_pipe = MakePipe();
  if (Fork()) { /* parent */
    CloseWriteEnd(gen_pipe);
  } else { /* child */
    CloseReadEnd(gen_pipe);
    generator(gen_pipe, maxprime);
  }

  /* Spawn filter chain. */
  if (Fork()) { /* parent */
    CloseReadEnd(gen_pipe);
  } else { /* child */
    filter_chain(gen_pipe);
  }

  for (int i = 0; i < 2; i++)
    Wait(NULL);

  return 0;
}
