/* -- httpd.c -- */

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define LISTENADDR "0.0.0.0"

/* Structures */
struct sHttpRequest {
  char method[8];
  char url[128];
};
typedef struct sHttpRequest httpreq;

/* GLOBAL */
char *error;

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

/* returns 0 on error, otherwize it returns struct sHttpRequest */
httpreq *parse_http(char *str)
{
  httpreq *req;
  char *p;

  req = malloc(sizeof(struct sHttpRequest));
  if (!req) return NULL;
  for (p = str; *p && *p != ' '; p++);
  if (*p == ' ')
    *p = 0;
  else {
    error = "parse_http() NOSPACE error";
    free(req);
    return 0;
  }
  strncpy(req->method, str, 7);

  for (str=++p; *p && *p != ' '; p++);
  if (*p == ' ')
    *p = 0;
  else {
      error = "parse_http() 2N_NOSPACE error";
      free(req);
      return 0;
  }
  strncpy(req->url, str, 127);
  return req;
}

/* returns 0 in error, otherwize return data */
char *cli_read(int c)
{
    static char buf[512];

    memset(&buf, 0, 512);
    if (read(c, buf, 511) < 0)
    {
        error = "read() error";
        return 0;
    }
    else
        return buf;
}

void cli_conn(int s, int c)
{
    httpreq *req;
    char *p;

    p = cli_read(c);
    if (!p)
    {
        fprintf(stderr, "%s\n", error);
        close(c);
        return ;
    }
    req = parse_http(p);
    if (!req)
    {
        fprintf(stderr, "%s\n", error);
        close(c);
        return ;
    }
    printf("method: '%s'\nUrl: '%s'\n", req->method, req->url);
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
  }
  return -1;
}
