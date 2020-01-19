#include "shell.h"

typedef struct proc {
  pid_t pid;    /* process identifier */
  int state;    /* RUNNING or STOPPED or FINISHED */
  int exitcode; /* -1 if exit status not yet received */
} proc_t;

typedef struct job {
  pid_t pgid;    /* 0 if slot is free */
  proc_t *proc;  /* array of processes running in as a job */
  int nproc;     /* number of processes */
  int state;     /* changes when live processes have same state */
  char *command; /* textual representation of command line */
} job_t;

static job_t *jobs = NULL; /* array of all jobs */
static int njobmax = 1;    /* number of slots in jobs array */
static int tty_fd = -1;    /* controlling terminal file descriptor */

static void sigchld_handler(int sig) {
  msg("%s\n", __func__);
  int old_errno = errno;
  pid_t pid;
  int status;
  /* TODO: Change state (FINISHED, RUNNING, STOPPED) of processes and jobs.
   * Bury all children that finished saving their status in jobs. */
  while((0 < (pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED))))
  {
    int state;
    for(size_t job_idx = 0; job_idx < njobmax; job_idx++)
    {
      if(jobs[job_idx].pgid == 0)
        continue;
      state = 0;
      bool found_proc = false;
      for(size_t proc_idx = 0; proc_idx < jobs[job_idx].nproc; proc_idx++)
      {
        found_proc = false;
        if(jobs[job_idx].proc[proc_idx].pid != pid)
        {
          continue;
        }
        found_proc = true;
        if(WIFEXITED(status))
        {
          jobs[job_idx].proc[proc_idx].state = FINISHED;
          jobs[job_idx].proc[proc_idx].exitcode = status;
        }
        else if(WIFSIGNALED(status))
        {
          jobs[job_idx].proc[proc_idx].state = FINISHED;
          jobs[job_idx].proc[proc_idx].exitcode = status;
        }
        else if(WIFCONTINUED(status))
        {
          jobs[job_idx].proc[proc_idx].state = RUNNING;
        }
        else if(WIFSTOPPED(status))
        {
          jobs[job_idx].proc[proc_idx].state = STOPPED;
        }
        state |= jobs[job_idx].proc[proc_idx].state;
      }
      if(found_proc)
      {
        jobs[job_idx].state = FINISHED;
        safe_printf("assigning status FINISHED to job no %ld of pgid %d\n", job_idx, jobs[job_idx].pgid);
        if(state & STOPPED)
        {
          jobs[job_idx].state = STOPPED;
          safe_printf("assigning status STOPPED to job no %ld of pgid %d\n", job_idx, jobs[job_idx].pgid);
        }
        if(state & RUNNING)
        {
          jobs[job_idx].state = RUNNING;
          safe_printf("assigning status RUNNING to job no %ld of pgid %d\n", job_idx, jobs[job_idx].pgid);
        }
      }
    }
  }
  errno = old_errno;
}

/* When pipeline is done, its exitcode is fetched from the last process. */
static int exitcode(job_t *job) {
  msg("%s\n", __func__);
  return job->proc[job->nproc - 1].exitcode;
}

static int allocjob(void) {
  msg("%s\n", __func__);
  /* Find empty slot for background job. */
  for (int j = BG; j < njobmax; j++)
    if (jobs[j].pgid == 0)
      return j;

  /* If none found, allocate new one. */
  jobs = realloc(jobs, sizeof(job_t) * (njobmax + 1));
  return njobmax++;
}

static int allocproc(int j) {
  msg("%s\n", __func__);
  job_t *job = &jobs[j];
  job->proc = realloc(job->proc, sizeof(proc_t) * (job->nproc + 1));
  return job->nproc++;
}

int addjob(pid_t pgid, int bg) {
  msg("%s\n", __func__);
  int j = bg ? allocjob() : FG;
  job_t *job = &jobs[j];
  /* Initial state of a job. */
  job->pgid = pgid;
  job->state = RUNNING;
  job->command = NULL;
  job->proc = NULL;
  job->nproc = 0;
  return j;
}

static void deljob(job_t *job) {
  msg("%s\n", __func__);
  assert(job->state == FINISHED);
  free(job->command);
  free(job->proc);
  job->pgid = 0;
  job->command = NULL;
  job->proc = NULL;
  job->nproc = 0;
}

static void movejob(int from, int to) {
  msg("%s\n", __func__);
  assert(jobs[to].pgid == 0);
  memcpy(&jobs[to], &jobs[from], sizeof(job_t));
  memset(&jobs[from], 0, sizeof(job_t));
}

static void mkcommand(char **cmdp, char **argv) {
  msg("%s\n", __func__);
  if (*cmdp)
    strapp(cmdp, " | ");

  for (strapp(cmdp, *argv++); *argv; argv++) {
    strapp(cmdp, " ");
    strapp(cmdp, *argv);
  }
}

void addproc(int j, pid_t pid, char **argv) {
  msg("%s\n", __func__);
  assert(j < njobmax);
  job_t *job = &jobs[j];

  int p = allocproc(j);
  proc_t *proc = &job->proc[p];
  /* Initial state of a process. */
  proc->pid = pid;
  proc->state = RUNNING;
  proc->exitcode = -1;
  mkcommand(&job->command, argv);
}

/* Returns job's state.
 * If it's finished, delete it and return exitcode through statusp. */
int jobstate(int j, int *statusp) {
  msg("%s\n", __func__);
  assert(j < njobmax);
  job_t *job = &jobs[j];
  int state = job->state;

  /* TODO: Handle case where job has finished. */
  if(job->state == FINISHED)
  {
    *statusp = exitcode(job);
    deljob(job);
  }

  return state;
}

char* jobcmd(int j) {
  msg("%s\n", __func__);
  assert(j < njobmax);
  job_t *job = &jobs[j];
  return job->command;
}

/* Continues a job that has been stopped. If move to foreground was requested,
 * then move the job to foreground and start monitoring it. */
bool resumejob(int j, int bg, sigset_t *mask) {
  msg("%s\n", __func__);
  if (j < 0) {
    for (j = njobmax - 1; j > 0 && jobs[j].state == FINISHED; j--)
      continue;
  }

  if (j >= njobmax || jobs[j].state == FINISHED)
    return false;

  /* TODO: Continue stopped job. Possibly move job to foreground slot. */
  jobs[j].state = RUNNING; // prevent race condition
  killpg(jobs[j].pgid, SIGCONT);
  if(bg == FG)
  {
    assert(j != 0);
    movejob(j, 0);
    monitorjob(mask);
  }

  return true;
}

/* Kill the job by sending it a SIGTERM. */
bool killjob(int j) {
  msg("%s\n", __func__);
  if (j >= njobmax || jobs[j].state == FINISHED)
    return false;
  debug("[%d] killing '%s'\n", j, jobs[j].command);

  /* TODO: I love the smell of napalm in the morning. */
  assert(jobs[j].pgid != 0);
  killpg(jobs[j].pgid, SIGTERM);

  return true;
}

/* Report state of requested background jobs. Clean up finished jobs. */
void watchjobs(int which) {
  msg("%s\n", __func__);
  for (int j = BG; j < njobmax; j++) {
    if (jobs[j].pgid == 0)
      continue;

    /* TODO: Report job number, state, command and exit code or signal. */
    if(which == ALL || which == FINISHED)
    {
      int state = jobs[j].state;
      if(state == FINISHED)
      {
        printf("[%d] '%s' – finished, ", j, jobs[j].command);
        if(WIFEXITED(state))
        {
          printf("exited with code %d\n", WEXITSTATUS(state));
        }
        if(WIFSIGNALED(state))
        {
          printf("signaled by signal %d\n", WTERMSIG(state));
        }
        deljob(&jobs[j]);
      }
    }
    if(which == ALL || which == RUNNING)
    {
      if(jobs[j].state == RUNNING)
      {
        printf("[%d] '%s' – running\n", j, jobs[j].command);
      }
    }
    if(which == ALL || which == STOPPED)
    {
      if(jobs[j].state == STOPPED)
      {
        printf("[%d] '%s' – stopped\n", j, jobs[j].command);
      }
    }
  }
}

/* Monitor job execution. If it gets stopped move it to background.
 * When a job has finished or has been stopped move shell to foreground. */
int monitorjob(sigset_t *mask) {
  msg("%s\n", __func__);
  int _exitcode = 0;

  /* TODO: Following code requires use of Tcsetpgrp of tty_fd. */
  tcsetpgrp(tty_fd, jobs[0].pgid);
  while(jobs[0].state == RUNNING)
  {
    Sigsuspend(mask);
  }
  if(jobs[0].state == FINISHED)
  {
    _exitcode = exitcode(&jobs[0]);
    deljob(&jobs[0]);
  }
  if(jobs[0].state == STOPPED)
  {
    size_t new_idx = allocjob();
    jobs[new_idx].pgid = 0; // mark job as empty
    movejob(0, new_idx);
  }
  tcsetpgrp(tty_fd, getpgrp());
  return _exitcode;
}

/* Called just at the beginning of shell's life. */
void initjobs(void) {
  msg("%s\n", __func__);
  Signal(SIGCHLD, sigchld_handler);
  jobs = calloc(sizeof(job_t), 1);

  /* Assume we're running in interactive mode, so move us to foreground.
   * Duplicate terminal fd, but do not leak it to subprocesses that execve. */
  assert(isatty(STDIN_FILENO));
  tty_fd = Dup(STDIN_FILENO);
  fcntl(tty_fd, F_SETFL, O_CLOEXEC);
  Tcsetpgrp(tty_fd, getpgrp());
}

/* Called just before the shell finishes. */
void shutdownjobs(void) {
  msg("%s\n", __func__);
  sigset_t mask;
  Sigprocmask(SIG_BLOCK, &sigchld_mask, &mask);

  /* TODO: Kill remaining jobs and wait for them to finish. */
  for(size_t job_idx = 0; job_idx < njobmax; job_idx++)
  {
    if(jobs[job_idx].pgid != 0 && jobs[job_idx].state != FINISHED)
    {
      killjob(job_idx);
      while(jobs[job_idx].state != FINISHED)
      {
        Sigsuspend(&mask);
      }
    }
  }

  watchjobs(FINISHED);

  Sigprocmask(SIG_SETMASK, &mask, NULL);

  Close(tty_fd);
}
