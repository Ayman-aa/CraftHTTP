/* sockets.c */
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define IP "142.250.201.78"  /* www.google.com */
#define PORT 80             /* HTTP */

int main ( void )
{
    int s;  /* Socket Return, File Descriptor */
    int c;  /* Connect Return */
    char *data = "HEAD / HTTP/1.0\n\n";
    char buf[512];

    struct sockaddr_in sock;  /* Server Address */

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) return write(2, "socket() - error\n", 17);
    
    sock.sin_addr.s_addr = inet_addr(IP);  /* Server IP */
    sock.sin_port = htons(PORT);  /* Server Port */
    sock.sin_family = AF_INET;

    c = connect(s, (struct sockaddr *)&sock, sizeof(struct sockaddr_in));
    if (c != 0) return write(2, "connect() - error\n", 18);

    write(s, data, strlen(data));
    read(s, buf, 511);
    close(s);

    printf("\n %s \n", buf);
}
