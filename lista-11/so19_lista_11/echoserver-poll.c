#include "csapp.h"
#include "rio.h"

#define LISTENQ 10

typedef struct client {
  size_t id;
  rio_t rio;
} client_t;

static size_t clientid = 1;
static client_t **clients;
static struct pollfd *fds;
static nfds_t nfds = 0;
static nfds_t maxfds = 0;

static sig_atomic_t quit = false;
static size_t nbytes = 0; /* Counts total bytes received by server */

static void initclients(int listenfd) {
  nfds = 1;
  maxfds = 1;
  fds = Malloc(sizeof(struct pollfd));
  fds[0].fd = listenfd;
  fds[0].events = POLLIN;
}

static void addclient(int connfd, const char *hostname, const char *port) {
  printf("[%ld] Connected to %s:%s\n", clientid, hostname, port);

  /* Should expand space to accommodate for new client ? */
  if (maxfds == nfds) {
    maxfds++;
    fds = Realloc(fds, sizeof(struct pollfd) * maxfds);
    clients = Realloc(clients, sizeof(client_t *) * maxfds);
    clients[maxfds - 1] = Malloc(sizeof(client_t));
  }

  int i = nfds++;
  fds[i].fd = connfd;
  fds[i].events = POLLIN; /* Wait only for input events! */
  fds[i].revents = 0;

  client_t *c = clients[i];
  c->id = clientid++;
  rio_readinitb(&c->rio, connfd);
}

static void delclient(int i) {
  assert(i > 0);
  Close(fds[i].fd);
  printf("[%ld] Disconnected!\n", clients[i]->id);

  if (i < nfds - 1) {
    fds[i] = fds[nfds - 1];

    /* Swap slots so that prefix of array comprises only active clients. */
    client_t *tmp = clients[i];
    clients[i] = clients[nfds - 1];
    clients[nfds - 1] = tmp;
  }

  nfds--;
}

static int clientread(int i) {
  char buf[MAXLINE];
  int n = Rio_readlineb(&clients[i]->rio, buf, MAXLINE);

  if (n > 0) {
    Rio_writen(fds[i].fd, buf, n);
    nbytes += n;
    printf("[%ld] Received %d (%ld total) from client\n",
           clients[i]->id, n, nbytes);
  }

  return n;
}

static void sigint_handler(int sig) {
  safe_printf("Server received quit request!\n");
  quit = true;
}

int main(int argc, char **argv) {
  if (argc != 2)
    app_error("usage: %s <port>\n", argv[0]);

  Signal(SIGINT, sigint_handler);

  int listenfd = Open_listenfd(argv[1], LISTENQ);
  initclients(listenfd);

  while (!quit) {
    int nready = Poll(fds, nfds, 500);
    if (nready == 0)
      continue;

    /* TODO: If listening descriptor ready, add new client to the pool. */
    if(fds[0].revents & POLLIN)
    {
      struct sockaddr_storage clientaddr;
      socklen_t size = sizeof clientaddr;
      int connfd = Accept(listenfd, (struct sockaddr*)&clientaddr, &size);
      // if(clientaddr.ss_family == AF_INET)
      //   host = inet_ntoa(((struct sockaddr_in*)&clientaddr)->sin_addr);
      //   // host = ((struct sockaddr_in*)&clientaddr)->sin_addr.s_addr;
      //   // printf("port is %d x%dx\n", ((struct sockaddr_in*)&clientaddr)->sin_port,  ((struct sockaddr_in*)&clientaddr)->sin_addr.s_addr);
      // if(clientaddr.ss_family == AF_INET6)
      //   // host = inet_ntoa(((struct sockaddr_in6*)&clientaddr)->sin6_addr);
      //   host = "ipv6 host";

      char* host = malloc(64);
      sa_family_t address_family = clientaddr.ss_family;
      inet_ntop(address_family,
        address_family == AF_INET ?
          (void*)(&((struct sockaddr_in*)&clientaddr)->sin_addr)
          : (void*)(&((struct sockaddr_in6*)&clientaddr)->sin6_addr),
        host,
        size);
      addclient(connfd, host, argv[1]);
      free(host);
    }

    /* TODO: Echo a text line from each ready connected descriptor.
     * Delete a client when end-of-file condition was detected on socket. */
    int i = 1;
    while (nready > 0) {
      while(!(fds[i].revents & POLLIN) && ++i < nfds) {}
      if(i == nfds)
        break;
      size_t read_bytes = clientread(i);
      if(read_bytes == 0)
        delclient(i);
    }
  }

  printf("Server received %ld total bytes.\n", nbytes);
  return EXIT_SUCCESS;
}
