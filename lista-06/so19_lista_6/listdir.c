#include "csapp.h"

#define DIRBUFSZ 256

static void print_mode(mode_t m) {
  char t;

  if (S_ISDIR(m))
    t = 'd';
  else if (S_ISCHR(m))
    t = 'c';
  else if (S_ISBLK(m))
    t = 'b';
  else if (S_ISREG(m))
    t = '-';
  else if (S_ISFIFO(m))
    t = 'f';
  else if (S_ISLNK(m))
    t = 'l';
  else if (S_ISSOCK(m))
    t = 's';
  else
    t = '?';

  char ur = (m & S_IRUSR) ? 'r' : '-';
  char uw = (m & S_IWUSR) ? 'w' : '-';
  char ux = (m & S_IXUSR) ? 'x' : '-';
  char gr = (m & S_IRGRP) ? 'r' : '-';
  char gw = (m & S_IWGRP) ? 'w' : '-';
  char gx = (m & S_IXGRP) ? 'x' : '-';
  char or = (m & S_IROTH) ? 'r' : '-';
  char ow = (m & S_IWOTH) ? 'w' : '-';
  char ox = (m & S_IXOTH) ? 'x' : '-';

  /* TODO: Fix code to report set-uid/set-gid/sticky bit as 'ls' does. */
  ux = (m & S_ISUID) ? (S_IXUSR & m ? 'S' : 's') : ux;
  gx = (m & S_ISGID) ? (S_IXUSR & m ? 'S' : 's') : gx;
  ox = (m & S_ISVTX) ? (m & S_IXUSR ? 't' : 'T') : ox;

  printf("%c%c%c%c%c%c%c%c%c%c", t, ur, uw, ux, gr, gw, gx, or, ow, ox);
}

static void print_uid(uid_t uid) {
  struct passwd *pw = getpwuid(uid);
  if (pw)
    printf(" %10s", pw->pw_name);
  else
    printf(" %10d", uid);
}

static void print_gid(gid_t gid) {
  struct group *gr = getgrgid(gid);
  if (gr)
    printf(" %10s", gr->gr_name);
  else
    printf(" %10d", gid);
}

static void file_info(int dirfd, const char *name) {
  struct stat sb[1]; // why, why are you doing this?

  /* TODO: Read file metadata. */
  fstatat(dirfd, name, sb, AT_SYMLINK_NOFOLLOW);

  print_mode(sb->st_mode);
  printf("%4ld", sb->st_nlink);
  print_uid(sb->st_uid);
  print_gid(sb->st_gid);

  /* TODO: For devices: print major/minor pair; for other files: size. */
  if((sb->st_mode & S_IFMT) == S_IFBLK || (sb->st_mode & S_IFMT) == S_IFCHR){
    dev_t dev_id = sb->st_rdev;
    printf("%4d %5d ", major(dev_id), minor(dev_id));
  }
  else{
    off_t file_size = sb->st_size;
    printf("%10ld ", file_size);
  }

  char *now = ctime(&sb->st_mtime);
  now[strlen(now) - 1] = '\0';
  printf("%26s", now);

  printf("  %s", name);

  if (S_ISLNK(sb->st_mode)) {
  /* TODO: Read where symlink points to and print '-> destination' string. */
    char dest_filepath[256];
    readlinkat(dirfd, name, dest_filepath, sizeof dest_filepath);
    printf(" -> %s", dest_filepath);
  }

  putchar('\n');
}

int main(int argc, char *argv[]) {
  if (!argv[1])
    argv[1] = ".";

  int dirfd = Open(argv[1], O_RDONLY | O_DIRECTORY, 0);
  char buf[DIRBUFSZ];
  int n;

  while ((n = Getdents(dirfd, (void *)buf, DIRBUFSZ))) {
    /* TODO: Iterate over directory entries and call file_info on them. */
    // These  are  not  the interfaces you are interested in.  Look at readdir(3) for the
    //   POSIX-conforming C library interface.  This page documents the bare kernel  system
    //   call interfaces.
    for(size_t byte_position = 0; byte_position < n; )
    {
      struct linux_dirent *dir = (struct linux_dirent*) (buf + byte_position);
      char* dir_entry_filename = dir->d_name;
      file_info(dirfd, dir_entry_filename);

      byte_position += dir->d_reclen;
    }
  }

  Close(dirfd);
  return EXIT_SUCCESS;
}
