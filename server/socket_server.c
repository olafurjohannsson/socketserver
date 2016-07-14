#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#include "../utils/utils.h"

#define MAXMSG 1024


void print_meta();

struct socket_user
{
   unsigned int port;
   char *host;
};

struct socket_server
{
   unsigned int total_users;
   time_t boot_time;
   struct socket_user *users;
};

int read_from_client(int filedes)
{
   char buffer[MAXMSG];
   int nbytes;

   // Clear buffer and read fd
   memset(buffer, 0, MAXMSG);
   nbytes = read(filedes, buffer, MAXMSG);
   if (nbytes < 0)
   {
      perror("Read from client\n");
      exit(EXIT_FAILURE);
   }
   else if (nbytes == 0)
   {
      // EOF
      return -1;
   }
   else
   {
      nprintf("Server: got message: %s", buffer);
      return 0;
   }
}

int main(int argc, char* argv[])
{
   int port, sock;
   struct socket_server server;
   struct sockaddr_in clientname;
   socklen_t size;
   int i;
   fd_set active_fd_set, read_fd_set;

   server.boot_time = time(NULL);
   server.total_users = 0;
   if (argc != 2)
   {
      printf("Usage: %s <port>\n", argv[0]);
      exit(0);
   }
   port = atoi(argv[1]);

   // Create socket and listen for connections
   sock = create_socket(port);
   if (listen (sock, 1) < 0)
   {
      perror("Listen fail\n");
      exit(EXIT_FAILURE);
   }

   lprintf("+OK Socket server started on port %d", port);

   // Init set of active sockets
   FD_ZERO(&active_fd_set);
   FD_SET(sock, &active_fd_set);

   while (1)
   {
      read_fd_set = active_fd_set;

      // Block until input arrives on active socket
      if (select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)
      {
         perror("Select fail\n");
         exit(EXIT_FAILURE);
      }

      // service all sockets with input pending
      for (i = 0; i < FD_SETSIZE; i++)
      {
         if (FD_ISSET(i, &read_fd_set))
         {
            if (i == sock)
            {
               // Connection request on original socket
               int new;
               size = sizeof(clientname);
               new = accept(sock, (struct sockaddr*)&clientname, &size);

               if (new < 0)
               {
                  perror("Accept fail\n");
                  exit(EXIT_FAILURE);
               }

               server.total_users++;
               char *host = inet_ntoa(clientname.sin_addr);
               unsigned int port = ntohs(clientname.sin_port);

               lprintf("+OK Connect from host %s, port %d, total users: %d.", host, port, server.total_users);
               /*
               struct socket_user *user = malloc(sizeof(struct socket_user*));
               user->host = host;
               user->port = port;
               server.users[server.total_users] = *user;
               */
               // Set fd
               FD_SET(new, &active_fd_set);
            }
            else
            {
               // Data arriving on an already connected socket
               if (read_from_client(i) < 0)
               {
                  //free(server.users[server.total_users]);
                  close(i);
                  FD_CLR(i, &active_fd_set);
               }
            }
         }
      }
   }

   return 0;
}

unsigned rawsock_get_adapter_ip(const char *ifname)
{
    int fd;
    struct ifreq ifr;
    struct sockaddr_in *sin;
    struct sockaddr *sa;
    int x;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;
    strcpy_s(ifr.ifr_name, IFNAMSIZ, ifname);

    x = ioctl(fd, SIOCGIFADDR, &ifr);
    if (x < 0) {
        fprintf(stderr, "ERROR:'%s': %s\n", ifname, strerror(errno));
        //fprintf(stderr, "ERROR:'%s': couldn't discover IP address of network interface\n", ifname);
        close(fd);
        return 0;
    }

    close(fd);

    sa = &ifr.ifr_addr;
    sin = (struct sockaddr_in *)sa;
    return ntohl(sin->sin_addr.s_addr);
}

void print_meta(struct socket_server server)
{
   char buff[32];
   set_time(server.boot_time, 32, buff);
   lprintf("Start time of socket server: %s", buff);
   lprintf("Process ID of socket server: %u", getpid());
}
