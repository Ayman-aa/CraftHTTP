/* -- httpd.c -- */

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

/* GLOBAL */
char *error;

#define LISTENADDR "0.0.0.0"

/* return -1 if there is an error, otherwise the socket fd */
int srv_init(int port)
{
  int returnValue;
  struct sockaddr_in srv;

  returnValue = socket(AF_INET, SOCK_STREAM, 0);
  if (returnValue < 0)
  {
    error = "Socket creation failed";
    return -1;
  }

  srv.sin_family = AF_INET;
  srv.sin_addr.s_addr = inet_addr(LISTENADDR);
  printf("%f\n", inet_addr(LISTENADDR));
  srv.sin_port = htons(port);

  if (bind(returnValue, (struct sockaddr *)&srv, sizeof(srv)))
  {
    close(returnValue);
    error = "Bind() error";
    return -1;
  }

  if (listen(returnValue, 5))
  {
    close(returnValue);
    error = "Listen() error";
    return -1;
  }
  return returnValue;
}

/* return -1 on failure, otherwise in return accepted socket fd */
int cli_accept(int s)
{
  int c;
  socklen_t addrlen;
  struct sockaddr_in cli;

  addrlen = 0;
  memset(&cli, 0, sizeof(cli));
  c = accept(s, (struct sockaddr *)&cli, &addrlen);
  if (c < 0)
  {
      error = "accept() error";
      return -1;
  }
  return c;
}

void cli_conn(int s, int c)
{
    return ;
}

uint64_t main ( int ac, char *av[])
{
  int s, c;
  char *port;

  if (ac < 2)
  {
    fprintf(stderr, "Usage: %s <listening port>\n", av[0]);
    return -1;
  }
  else port = av[1];

  s = srv_init(atoi(port));
  if (s == -1)
  {
    fprintf(stderr, "%s\n", error);
    return -1;
  }
  printf("Listening on %s:%d\n", LISTENADDR, atoi(port));

  while (1337)
  {
    c = cli_accept(s);
    if (c == -1)
    {
      fprintf(stderr, "%s\n", error);
      continue ;
    }
    printf("Incomming connection\n");
    if (!fork())
      cli_conn(s, c);
    close(c);
  }
  return -1;
}
