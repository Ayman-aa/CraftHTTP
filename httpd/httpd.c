/* -- httpd.c -- */

#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#define LISTENADDR "0.0.0.0"

/* Structures */
struct sHttpRequest {
  char method[8];
  char url[128];
};
typedef struct sHttpRequest httpreq;

struct sFile {
    char filename[64];
    char *fc; /* fc = file content */
    int size;
};
typedef struct sFile File;

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

void http_request(int c, char *contenttype, char *response)
{
    char buf[512];
    int length;
    
    memset(&buf, 0, 512);
    length = strlen(response);
    snprintf(buf, 511,
        "Content-Type: %s\n"
        "Content-Length: %d\n"
        "\n%s\n", contenttype, length, response);
    
    length = strlen(buf);
    write(c, &buf, length);

    return ;
}

void http_headers(int c, int code)
{
    char buf[512];
    int buf_length;

    memset(&buf, 0, 512);
    snprintf(buf, 511,
        "HTTP/1.0 %d OK\n"
        "Server: httpd.c\n"
        "X-Frame-Options: SAMEORIGIN\n", code );
    
    buf_length = strlen(buf);
    write(c, buf, buf_length);
    return ;
}

File *readfile(char *filename)
{
    char buf[512];
    int n, x, fd;
    File *f;

    fd = open(filename, O_RDONLY);
    if (fd < 0)
        return 0;

    f = malloc(sizeof(File));
    if (!f)
    {
        close(fd);
        return 0;
    }
    strncpy(f->filename, filename, 63);
    f->fc = malloc(512);

    x = 0; /* bytes read */
    while (1337)
    {
        memset(buf, 0, 512);
        n = read(fd, buf, 512);
        if (!n)
            break ;
        else if (x == -1)
        {
            close(fd);
            free(f);
            free(f->fc);

            return 0;
        }
        strncpy(buf, (f->fc)+x, n);
        x += n;
        f->fc = realloc(f->fc, (512)+x);
    }
    f->size = x;
    close(fd);

    return f;
}

/* 0 = everything is ok, 1 - error */
int sendfile(int c, char *contenttype, File *file)
{
    char buf[512];
    char *p;
    int n, x;
    
    if (!file)
        return 0;
    memset(&buf, 0, 512);
    snprintf(buf, 511,
        "Content-Type: %s\n"
        "Content-Length: %d\n\n",
             contenttype, file->size);
    
    n = file->size;
    p = file->fc; /* again, fc = file content */
    while (42)
    {
        x = write(c, p, (n < 512)?n:512);
        if (x < 0)
            return 0;

        n -= x;
        if (n < 1)
            break ;
        else
            p += x;
    }
    return 1;
}

void cli_conn(int s, int c)
{
    httpreq *req;
    char *p;
    char *response;
    char str[96];
    File *f;

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
    if (!strcmp(req->method, "GET") && !strncmp(req->url, "/img/", 5))
    {
        memset(str, 0, 96);
        snprintf(str, 96,".%s", req->url);
        f = readfile(str);
        printf("opening %s with size: %d\n", f->filename, f->size);
        if (!f)
        {
            response = "File not found";
            http_headers(c, 404); /* 404 = on file was found */
            http_request(c, "text/plain", response);
        }
        else
        {
            http_headers(c, 200);
            if (!sendfile(c, "image/png", f))
            {
                response = "HTTP server error";
                http_request(c, "text/plain", response);
            }
        }
    }
    else if (!strcmp(req->method, "GET") && !strcmp(req->url, "/app/webpage"))
    {
        response = "<html> Hello World</html>";
        http_headers(c, 200); /* 200 = everything is okay */
        http_request(c, "text/html", response);
    }
    else
    {
        response = "File not found";
        http_headers(c, 404); /* 404 = on file was found */
        http_request(c, "text/plain", response);
    }
    free(req);
    close(c);
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
