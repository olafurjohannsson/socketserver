#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "socket_client.h"

void write_to_server(int fd, char *message)
{
   int nbytes;

   nbytes = write(fd, message, strlen(message) + 1);
   if (nbytes < 0)
   {
      perror("write");
      exit(EXIT_FAILURE);
   }
}

void init_sockaddr(struct sockaddr_in *name, const char *hostname, uint16_t port)
{
   struct hostent *hostinfo;

   name->sin_family = AF_INET;
   name->sin_port = htons(port);
   hostinfo = gethostbyname(hostname);

   if (hostinfo == NULL)
   {
      printf("Unknown host %s.\n", hostname);
      exit(EXIT_FAILURE);
   }
   name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
}

int main(int argc, char* argv[])
{
   int sock;
   unsigned int port;
   struct sockaddr_in servername;

   sock = socket(PF_INET, SOCK_STREAM, 0);

   if (argc != 2)
   {
      printf("Usage <port>\n");
      exit(0);
   }

   port = atoi(argv[1]);
   if (sock < 0)
   {
      perror("socket (client)");
      exit(EXIT_FAILURE);
   }

   init_sockaddr(&servername, "127.0.0.1", port);

   if (0 > connect(sock,
            (struct sockaddr *)&servername,
            sizeof(servername)))
   {
      perror("connect (client)");
      exit(EXIT_FAILURE);
   }

   write_to_server(sock, "asdf");
   close(sock);
   exit(EXIT_SUCCESS);
}
