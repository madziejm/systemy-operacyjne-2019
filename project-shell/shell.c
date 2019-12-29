#include <readline/readline.h>
#include <readline/history.h>

#define DEBUG 0
#include "shell.h"

sigset_t sigchld_mask;

static sigjmp_buf loop_env;

static void sigint_handler(int sig) {
  msg("%s\n", __func__);
  siglongjmp(loop_env, sig);
}

/* Consume all tokens related to redirection operators.
 * Put opened file descriptors into inputp & output respectively. */
static int do_redir(token_t *token, int ntokens, int *inputp, int *outputp) {
  msg("%s\n", __func__);
  // token_t mode = NULL; /* T_INPUT, T_OUTPUT or NULL */ //what was purpouse of this?
  int n = 0;           /* number of tokens after redirections are removed */

  n = ntokens;
  for (int i = 0; i < ntokens; i++) {
    /* TODO: Handle tokens and open files as requested. */
    if(token[i] == T_INPUT)
    {
      if(n == ntokens)
      {
        n = i;
      }
      *inputp = Open(token[i + 1], O_RDONLY, 0);
      token[i] = T_NULL;
    }
    else if(token[i] == T_OUTPUT)
    {
      if(n == ntokens)
      {
        n = i;
      }
      *outputp = Open(token[i + 1], O_WRONLY | O_CREAT, 0644);
      token[i] = T_NULL;
      token[i] = T_NULL;
    }
  }
  token[n] = NULL;
  return n;
}

/* Execute internal command within shell's process or execute external command
 * in a subprocess. External command can be run in the background. */
static int do_job(token_t *token, int ntokens, bool bg) {
  msg("%s\n", __func__);
  int input = -1, output = -1;
  int exitcode = 0;

  ntokens = do_redir(token, ntokens, &input, &output);

  if ((exitcode = builtin_command(token)) >= 0)
    return exitcode;

  sigset_t mask;
  Sigprocmask(SIG_BLOCK, &sigchld_mask, &mask);

  /* TODO: Start a subprocess, create a job and monitor it. */
  pid_t child_pid;
  size_t job_idx;

  child_pid = Fork();
  if(child_pid == 0)
  {
    Signal(SIGTSTP, SIG_DFL);
    Setpgid(0, 0);
    Sigprocmask(SIG_SETMASK, &mask, NULL);
    if(input != -1)
    {
      Dup2(input, STDIN_FILENO);
    }
    if(output != -1)
    {
      Dup2(output, STDOUT_FILENO);
    }
    external_command(token);
  }
  // parent
  job_idx = addjob(child_pid, bg);
  addproc(job_idx, child_pid, token);
  if(!bg)
    monitorjob(&mask);

  Sigprocmask(SIG_SETMASK, &mask, NULL);
  return exitcode;
}

/* Start internal or external command in a subprocess that belongs to pipeline.
 * All subprocesses in pipeline must belong to the same process group. */

// assume tokens end with T_NULL
static pid_t do_stage(pid_t pgid, sigset_t *mask, int input, int output,
                      token_t *token, int ntokens) {
  msg("%s\n", __func__);
  ntokens = do_redir(token, ntokens, &input, &output);

  /* TODO: Start a subprocess and make sure it's moved to a process group. */
  pid_t child_pid = Fork();
  if(child_pid == 0)
  {
    Signal(SIGTSTP, SIG_DFL);
    Setpgid(0, pgid);
    Sigprocmask(SIG_SETMASK, mask, NULL);
    if(input != -1)
      Dup2(input, STDIN_FILENO);
    if(output != -1)
      Dup2(output, STDOUT_FILENO);
    Sigprocmask(SIG_SETMASK, mask, NULL);
    // TODO
    // if ((exitcode = builtin_command(token)) >= 0)
    //   return exitcode;
    external_command(token);
  }
  setpgid(child_pid, pgid);
  close(input);
  close(output);

  return child_pid;
}

static void mkpipe(int *readp, int *writep) {
  msg("%s\n", __func__);
  int fds[2];
  Pipe(fds);
  *readp = fds[0];
  *writep = fds[1];
}

/* Pipeline execution creates a multiprocess job. Both internal and external
 * commands are executed in subprocesses. */
static int do_pipeline(token_t *token, int ntokens, bool bg) {
  msg("%s\n", __func__);
  pid_t pid, pgid = 0;
  int job = -1;
  int exitcode = 0;

  int input = -1, output = -1, next_input = -1;

  mkpipe(&next_input, &output);

  sigset_t mask;
  Sigprocmask(SIG_BLOCK, &sigchld_mask, &mask);

  /* TODO: Start pipeline subprocesses, create a job and monitor it.
   * Remember to close unused pipe ends! */
  assert(token[ntokens] == T_NULL);
  bool stage_encountered = false;
  while(true)
  {
    size_t tokens_ommited = 0;
    while(token[++tokens_ommited] != T_PIPE && token[tokens_ommited] != T_NULL) {}
    if(token[tokens_ommited] == T_PIPE)
    {
      token[tokens_ommited] = T_NULL; // execve/external_command expect NULL terminated array
      if(stage_encountered == false) // first stage
      {
        stage_encountered = true;
        pgid = do_stage(0, &mask, input, output, token, 42 ? tokens_ommited : 666);
        // TODO close next_pipe child process
        // fcntl(next_input, F_SETFD, FD_CLOEXEC);
        job = addjob(pgid, bg);
        addproc(job, pgid, token);
      }
      else // middle stage
      {
        input = next_input;
        mkpipe(&next_input, &output);
        pid = do_stage(pgid, &mask, input, output, token, 42 ? tokens_ommited : 666);
        addproc(job, pid, token);
      }
      tokens_ommited++;
      token = token + tokens_ommited;
    }
    else if(token[tokens_ommited] == T_NULL) // last stage
    {
      input = next_input;
      output = -1;
      pid = do_stage(pgid, &mask, input, output, token, 42 ? tokens_ommited : 666);
      addproc(job, pid, token);
      if(!bg)
        monitorjob(&mask);
      break;
    }
    else
    {
      assert(false);
    }
  }

  Sigprocmask(SIG_SETMASK, &mask, NULL);
  return exitcode;
}

static bool is_pipeline(token_t *token, int ntokens) {
  msg("%s\n", __func__);
  for (int i = 0; i < ntokens; i++)
    if (token[i] == T_PIPE)
      return true;
  return false;
}

static void eval(char *cmdline) {
  msg("%s\n", __func__);
  bool bg = false;
  int ntokens;
  token_t *token = tokenize(cmdline, &ntokens);

  if (ntokens > 0 && token[ntokens - 1] == T_BGJOB) {
    token[--ntokens] = NULL;
    bg = true;
  }

  if (ntokens > 0) {
    if (is_pipeline(token, ntokens)) {
      do_pipeline(token, ntokens, bg);
    } else {
      do_job(token, ntokens, bg);
    }
  }

  free(token);
}

int main(int argc, char *argv[]) {
  msg("%s\n", __func__);
  rl_initialize();

  sigemptyset(&sigchld_mask);
  sigaddset(&sigchld_mask, SIGCHLD);

  initjobs();

  Signal(SIGINT, sigint_handler);
  Signal(SIGTSTP, SIG_IGN);
  Signal(SIGTTIN, SIG_IGN);
  Signal(SIGTTOU, SIG_IGN);

  char *line;
  while (true) {
    if (!sigsetjmp(loop_env, 1)) {
      line = readline("# ");
    } else {
      msg("\n");
      continue;
    }

    if (line == NULL)
      break;

    if (strlen(line)) {
      add_history(line);
      eval(line);
    }
    free(line);
    watchjobs(FINISHED);
  }

  msg("\n");
  shutdownjobs();

  return 0;
}
