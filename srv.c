/* srv.c */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define PORT 8181

int main ( void )
{
	int s, c;
	socklen_t addrlen = 0;
	struct sockaddr_in srv, cli;
	char buf[512];
	char *data;

	memset(&srv, 0, sizeof(srv));
	memset(&cli, 0, sizeof(cli));

	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s < 0) return write(2, "socket() - error\n", 17);

	srv.sin_family = AF_INET;
	srv.sin_addr.s_addr = 0;
	srv.sin_port = htons(PORT);

	int bind_result = bind(s, (struct sockaddr *)&srv, sizeof(srv));
	if (bind_result != 0)
	{
    fprintf(stderr, "bind() error: %d\n", bind_result);
    close(s);
    return -1;
	}
	printf("Binding successful\n");

	if (listen(s, 5) != 0)
	{
		fprintf(stderr, "listen() - error\n");
		close(s);
		return -1;
	}
	printf("Listening On 0.0.0.0:%d\n", PORT);

	c = accept(s, (struct sockaddr *)&cli, &addrlen);
	if (c < 0)
	{
		fprintf(stderr, "accept() - error\n");
		close(s);
		return -1;
	}
	printf("Client connected");

	read(c, buf, 511);
	data = "httpd v1.0/n";
	write(c, data, strlen(data));
	printf("\n %s \n", buf);
	close(c);
	close(s);

	return 0;
}
