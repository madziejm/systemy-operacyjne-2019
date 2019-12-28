#include "csapp.h"
// #include <stdlib.h>
// #include <stdio.h>

int main(int argc, char **argv) {
  struct addrinfo *p, *listp, hints;
  char buf_ip[MAXLINE];
  char buf_service[MAXLINE];
  int rc, flags;

  if (argc < 2 || 3 < argc)
    app_error("usage: %s <domain name>\n", argv[0]);
  bool service_given = argc == 3;

  /* Get a list of addrinfo records */
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = PF_UNSPEC; /* IPv4 only */
  hints.ai_socktype = SOCK_STREAM;
  /* Connections only */
  if ((rc = getaddrinfo(argv[1], service_given ? argv[2] : NULL, &hints, &listp)) != 0)
    gai_error(rc, "getaddrinfo");

  /* Walk the list and display each IP address */
  flags = NI_NUMERICHOST | NI_NUMERICSERV; /* Display address string instead of domain name */
  for (p = listp; p; p = p->ai_next) {
    Getnameinfo(p->ai_addr, p->ai_addrlen, buf_ip, MAXLINE, service_given ? buf_service : NULL, service_given ? MAXLINE : 0, flags);
    if(p->ai_family == PF_INET6)
    {
      printf("[%s]", buf_ip);
      if(service_given)
        printf(":%s\n", buf_service);
      else
        printf("\n");
    }
    else
    {
      printf("%s", buf_ip);
      if(service_given)
        printf(":%s\n", buf_service);
      else
        printf("\n");
    }
  }

  /* Clean up */
  freeaddrinfo(listp);

  return EXIT_SUCCESS;
}
