#ifndef _UTILS_H
#define _UTILS_H
extern void set_time(time_t mtime, unsigned int buflen, char *buffer);
extern int create_socket(uint16_t port);
extern ssize_t writen(int fd, char *buffer, size_t count);
extern ssize_t readn(int fd, char *buffer, size_t count);
extern ssize_t nprintf(char *format, ...);
extern void lprintf(char *format, ...);
#endif
