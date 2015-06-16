#ifndef _SOCKET_CLIENT_H
#define _SOCKET_CLIENT_H
extern void write_to_server(int fd, char *message);
extern void init_sockaddr(struct sockaddr_in *name,
      const char *hostname, uint16_t port);
#endif
