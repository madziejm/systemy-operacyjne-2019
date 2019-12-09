#include "shell.h"

typedef int (*func_t)(char **argv);

typedef struct {
  const char *name;
  func_t func;
} command_t;

static int do_quit(char **argv) {
  exit(EXIT_SUCCESS);
}

/*
 * 'cd' - change directory to $HOME
 * 'cd path' - change directory to path
 */
static int do_chdir(char **argv) {
  char *path = argv[0];
  if (path == NULL)
    path = getenv("HOME");
  int rc = chdir(path);
  if (rc < 0) {
    msg("cd: %s: %s\n", strerror(errno), path);
    return 1;
  }
  return 0;
}

static command_t builtins[] = {
  {"quit", do_quit},
  {"cd", do_chdir},
  {NULL, NULL},
};

int builtin_command(char **argv) {
  for (command_t *cmd = builtins; cmd->name; cmd++) {
    if (strcmp(argv[0], cmd->name))
      continue;
    return cmd->func(&argv[1]);
  }

  errno = ENOENT;
  return -1;
}

noreturn void external_command(char **argv) {
  const char *path = getenv("PATH");

  if (!index(argv[0], '/') && path) {
    /* TODO: For all paths in PATH construct an absolute path and execve it. */
    const char* const path_end = path + strlen(path); // get rid of this and rethink the conditions carefully
    size_t path_delimiter_position = -1;
    while(path < path_end && (path_delimiter_position = strcspn(path, ":")) > 0)
    {
      char *executable_path = strndup(path, path_delimiter_position);
      strapp(&executable_path, "/");
      strapp(&executable_path, argv[0]);
      (void)execve(executable_path, argv, environ);
      free(executable_path);
      if(path_delimiter_position > 0) // recheck the condition(s)
      {
        path += path_delimiter_position + 1;
      }
    }
  } else {
    (void)execve(argv[0], argv, environ);
  }

  msg("%s: %s\n", argv[0], strerror(errno));
  exit(EXIT_FAILURE);
}
