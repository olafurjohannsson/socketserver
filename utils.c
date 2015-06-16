#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "utils.h"

#define DEBUG 0

void lprintf(char *format, ...)
{
   va_list args;
   char buf[1024];

   va_start(args, format);
   vsnprintf(buf, sizeof(buf), format, args);
   va_end(args);
   if (!DEBUG)
   {
      FILE *fp;
      fp = fopen("log.txt", "a+");
      fprintf(fp, "%s\n", buf);
      fclose(fp);
   }
   fprintf(stdout, "%s\n", buf);
}

ssize_t nprintf(char *format, ...)
{
   va_list args;
   char buf[1024];

   va_start(args, format);
   vsnprintf(buf, sizeof(buf), format, args);
   va_end(args);
   return writen(1, buf, strlen(buf));
}

ssize_t readn(int fd, char *buffer, size_t count)
{
   int offset, block;

   offset = 0;
   while (count > 0)
   {
      block = read(fd, &buffer[offset], count);

      if (block < 0) return block;
      if (!block) return offset;

      offset += block;
      count -= block;
   }

   return offset;
}

ssize_t writen(int fd, char *buffer, size_t count)
{
   int offset, block;

   offset = 0;
   while (count > 0)
   {
      block = write(fd, &buffer[offset], count);

      if (block < 0) return block;
      if (!block) return offset;

      offset += block;
      count -= block;
   }

   return offset;
}

void set_time(time_t mtime, unsigned int buflen, char *buffer)
{
   struct tm* timeinfo;

   timeinfo = localtime(&mtime);
   strftime(buffer, buflen, "%d.%-m.%Y %H:%M", timeinfo);
}

int create_socket(uint16_t port)
{
   int sock;
   struct sockaddr_in name;

   sock = socket(PF_INET, SOCK_STREAM, 0);
   if (sock < 0)
   {
      perror("Creating socket fail!");
      exit(EXIT_FAILURE);
   }

   name.sin_family = AF_INET;
   name.sin_port = htons(port);
   name.sin_addr.s_addr = htonl(INADDR_ANY);

   if (bind(sock, (struct sockaddr *) &name, sizeof(name)) < 0)
   {
      perror("Bind fail\n");
      exit(EXIT_FAILURE);
   }
   return sock;
}

