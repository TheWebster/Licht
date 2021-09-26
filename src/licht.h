
#ifndef _LICHT_H_
#define _LICHT_H_

#include <pthread.h>
#include <stdio.h>


typedef enum {
    LICHT_OP_GET,
    LICHT_OP_SET,
    LICHT_OP_ADD,
    LICHT_OP_SUB,
    LICHT_OP_MUL
} licht_op_e;

typedef struct {
    licht_op_e op;
    float value;
    int   smooth_duration;
    int   smooth_interval;
    
    float range_max;
    float range_min;
} licht_cmd_s;

typedef struct {
    char *device;
    int br_fd;
    int max_fd;
    
    char reset;
    pthread_t worker;
    pthread_t listen;
    
    int target;
    int max;
    int current;
} licht_context_s;


#endif /* _LICHT_H_ */
