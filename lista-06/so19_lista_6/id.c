#include "csapp.h"

static const char *uidname(uid_t uid) {
  /* TODO: Something is missing here! */
  struct passwd *password = getpwuid(uid);
  return password->pw_name;
  // if(passwd != NULL)
  // {
  //   getpwnam
  // }
}

static const char *gidname(gid_t gid) {
  /* TODO: Something is missing here! */
  struct group *group = getgrgid(gid);
  return group->gr_name;
}

static int getid(uid_t *uid_p, gid_t *gid_p, gid_t **gids_p) {
  gid_t *gids = NULL;
  // int ngid = 2; // WHAT IS THIS
  int groups;

  /* TODO: Something is missing here! */
  *uid_p = getuid();
  *gid_p = getgid();

  // // malloc flavour
  // groups = getgroups(0, NULL); // 0;
  // gids = malloc(groups * sizeof(gid_t));
  // getgroups(groups, gids);
  // // malloc flavour end
  
  // realloc flavour
  // in order to fulfill the assignment criteria
  groups = 0;
  while(1)
  {
    gids = realloc(gids, (++groups) * sizeof(gid_t));
    if(!(getgroups(groups, gids) < 0 && errno == EINVAL))
    {
      break;
    }
  }
  // realloc flavour end

  *gids_p = gids;
  return groups;
}

int main(void) {
  uid_t uid;
  gid_t *gids, gid;
  int groups = getid(&uid, &gid, &gids);

  printf("uid=%d(%s) gid=%d(%s) ", uid, uidname(uid), gid, gidname(gid));
  printf("groups=%d(%s)", gids[0], gidname(gids[0]));
  for (int i = 1; i < groups; i++)
    printf(",%d(%s)", gids[i], gidname(gids[i]));
  putchar('\n');

  free(gids);

  return 0;
}
