
#define _UTIL_C_

#include "util.h"
#include "config.h"
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <pwd.h>
#include <regex.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>


char *acatstr2(const char *str1, const char *str2)
{
    int len = strlen(str1) + strlen(str2) + 1;
    
    char *dest = malloc(len);
    char *d    = dest;
    
    const char *src = str1;
    while(*src) {
        *d = *src;
        d++; src++;
    }
    
    src = str2;
    while(*src) {
        *d = *src;
        d++; src++;
    }
    
    *d = '\0';
    return dest;
}


FILE *get_home_conf()
{
    FILE *fconf = NULL;
    uid_t uid = getuid();
    if(uid != 0) {
        char *path;
        char *xdg = getenv("XDG_CONFIG_HOME");
        
        if(xdg)
            path = acatstr2(xdg, "/"cfg_PKG_NAME".conf");
        else {
            struct passwd *pwd = getpwuid(uid);
            
            path = acatstr2(pwd->pw_dir, "/.config/"cfg_PKG_NAME".conf");
        }
        
        fconf = fopen(path, "r");
        free(path);
    }
    
    return fconf;
}


char *parse_line(FILE *cfg, char *line_buf, int max, char **key, char **value)
{
    char      *c;
    regex_t    preg;
    regmatch_t pmatch[3] = {0};
    const char rexpr[] =
        "^\\s*([[:graph:]]+)\\s*=\\s*([[:graph:]]+)\\s*$";
    
    assert(0 == regcomp(&preg, rexpr, REG_EXTENDED));
    
    c = fgets(line_buf, max, cfg);
    if(c != NULL) {
        regexec(&preg, line_buf, 3, pmatch, 0);
        
        line_buf[pmatch[1].rm_eo] = '\0';
        line_buf[pmatch[2].rm_eo] = '\0';
        
        *key   = line_buf + pmatch[1].rm_so;
        *value = line_buf + pmatch[2].rm_so;
    }
    
    return c;
}


char *get_default_device()
{
    char *r;
    struct dirent *de;
    DIR *dir = opendir(g_path_bl);
    
    while(de = readdir(dir), !strcmp(de->d_name, ".") || !strcmp(de->d_name, ".."));
    r = acatstr2(g_path_bl, de->d_name);
    closedir(dir);
    
    return r;
}


int open_device(char *dev_path, int *br_fd, int *max_fd)
{
    char *path = acatstr2(dev_path, "/brightness");
    *br_fd = open(path, O_RDWR|O_SYNC);
    free(path);
    
    path = acatstr2(dev_path, "/max_brightness");
    *max_fd = open(path, O_RDONLY|O_SYNC);
    free(path);
    
    if(-1 == *br_fd || -1 == *max_fd) {
        LICHT_ERR(err_cant_open, dev_path, strerror(errno));
        return -1;
    }
    
    return 0;
}


int read_atoi(int fd)
{
    int result = 0;
    char c;
    
    while(read(fd, &c, 1) == 1 && isdigit(c)) {
        result *= 10;
        result += c - '0';
    }
    
    return result;
}


void write_itoa(FILE *file, int value)
{
    int v = value / 10;
    
    if(v)
        write_itoa(file, v);
    
    fputc(value%10+'0', file);
}


static struct timespec timer;

void set_timer()
{
    clock_gettime(CLOCK_MONOTONIC, &timer);
}


void wait_timer(int ms)
{
    timer.tv_nsec += ms * 1000000;
    timer.tv_sec  += timer.tv_nsec / 1000000000;
    timer.tv_nsec %= 1000000000;
    clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &timer, NULL);
}


static int socket_fd;
static struct sockaddr_un addr;

static int setup_socket(const char *path)
{
    socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    
    if(-1 == socket_fd) {
        LICHT_ERR("Could not create socket -- %s\n", strerror(errno));
        return -1;
    }
    
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, path, sizeof(addr.sun_path));
    
    return 0;
}


int setup_socket_recv(const char *path)
{
    if(-1 == setup_socket(path))
        return -1;
    
    if(-1 == bind(socket_fd, (struct sockaddr*)&addr, sizeof(addr))) {
        LICHT_ERR("Could not bind socket -- %s\n", strerror(errno));
        return -1;
    }
    
    if(-1 == listen(socket_fd, 1)) {
        LICHT_ERR("Cannot listen at socket -- %s\n", strerror(errno));
        return -1;
    }
    
    return 0;
}

int setup_socket_conn(const char* path)
{
    if(-1 == setup_socket(path))
        return -1;
    
    if(-1 == connect(socket_fd, (struct sockaddr*)&addr, sizeof(addr))) {
        LICHT_ERR("Could not connect to socket -- %s\n", strerror(errno));
        return -1;
    }
    
    return socket_fd;
}


int socket_recv()
{
    return accept(socket_fd, NULL, NULL);
}


void destroy_socket(const char *path)
{
    close(socket_fd);
    unlink(path);
}
    
