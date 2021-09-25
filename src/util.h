
#ifndef _UTIL_H_
#define _UTIL_H_

#include <stdio.h>
#include <string.h>


#ifndef _UTIL_C_
#define GLOBAL(name, init) extern name;
#else
#define GLOBAL(name, init) name = init;
#endif


FILE *open_conf_file();
int  parse_line(FILE *cfg, char *line_buf, int max, char **key, char **value);
int  open_device(char *dev_path, int*br_fd, int *max_fd);

int  read_atoi(int fd);
void write_itoa(FILE *file, int value);

void set_timer();
void wait_timer(int ms);

int  setup_socket_recv(const char *path);
int  setup_socket_conn(const char* path);
void destroy_socket(const char *path);

int socket_recv();



#define streq(str) !strcmp(streq_val, str)
#define LICHT_ERR(msg, ...) fprintf(stderr, msg, __VA_ARGS__)


GLOBAL(const char err_cant_open[], "Could not open '%s' -- %s\n")
GLOBAL(const char path_bl[], "/sys/class/backlight\n")

#endif /* _UTIL_H_ */
