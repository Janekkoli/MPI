/**
 * This file is for implementation of common interfaces used in both
 * MIMPI library (mimpi.c) and mimpirun program (mimpirun.c).
 * */

#include "mimpi_common.h"

#include "channel.h" //nwm czy moge

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


int rank=-1;
int size;

_Noreturn void syserr(const char* fmt, ...)
{
    va_list fmt_args;

    fprintf(stderr, "ERROR: ");

    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end(fmt_args);
    fprintf(stderr, " (%d; %s)\n", errno, strerror(errno));
    exit(1);
}

_Noreturn void fatal(const char* fmt, ...)
{
    va_list fmt_args;

    fprintf(stderr, "ERROR: ");

    va_start(fmt_args, fmt);
    vfprintf(stderr, fmt, fmt_args);
    va_end(fmt_args);

    fprintf(stderr, "\n");
    exit(1);
}

/////////////////////////////////////////////////
// Put your implementation here

int from_y_to_x(int from,int to,bool wr)
{
    return 16*from+to+24 + 300*wr;
}


int safe_chrecv(int __fd, void *__buf, size_t __nbytes)
{
    int ret,recv_bytes=0,to_go=__nbytes;
    while(to_go>0){
    ASSERT_SYS_OK(ret=chrecv(__fd,__buf+recv_bytes,to_go));
    if(ret==0)  return 0;
    recv_bytes+=ret;
    to_go-=ret;
    }
    return __nbytes;
}


int safe_chsend(int __fd, const void *__buf, size_t __n)
{
    int ret,send_bytes=0,to_go=__n;
    while(to_go>0){
    ASSERT_SYS_OK(ret=chsend(__fd,__buf+send_bytes,to_go));
    if(ret==0) return 0;
    to_go-=ret;
    send_bytes+=ret;
    }
    return __n;
}


pthread_mutex_t mutexx;
pthread_cond_t waiting;

int wait_count,wait_source,wait_tag;


message *first,*last;

bool finished_preceses[16];

void process_finished(int rank_p)
{
    ASSERT_ZERO(pthread_mutex_lock(&mutexx));
    finished_preceses[rank_p]=true;
    if(rank_p==wait_source && wait_count>0) 
        ASSERT_ZERO(pthread_cond_signal(&waiting));
    ASSERT_ZERO(pthread_mutex_unlock(&mutexx));
}

bool is_process_finished(int rank_p)
{
    ASSERT_ZERO(pthread_mutex_lock(&mutexx));
    bool ret=finished_preceses[rank_p];
    ASSERT_ZERO(pthread_mutex_unlock(&mutexx));
    return ret;
}

void mes_init()
{
    first=malloc(sizeof(message));
    last=first;
    ASSERT_ZERO(pthread_mutex_init(&mutexx, NULL));
    //ASSERT_ZERO(pthread_cond_init(waiting, NULL));
}

void add_mes(message *mes)
{
    ASSERT_ZERO(pthread_mutex_lock(&mutexx));
    last->next=mes;
    last=mes;
    if(wait_count==mes->count && wait_source==mes->source && wait_tag==mes->tag)
        ASSERT_ZERO(pthread_cond_signal(&waiting));
    ASSERT_ZERO(pthread_mutex_unlock(&mutexx));
}




#define LIST_CHECK while(x->next)   \
    {\
        x=x->next;\
        if(x->count==count && x->source==source && x->tag==tag)\
        {\
            memcpy(data, x->data, count);\
            free(x->data); /*Usuniecie elementu listy!!!*/\
            ASSERT_ZERO(pthread_mutex_unlock(&mutexx));\
            return MIMPI_SUCCESS;\
        }\
    }\


MIMPI_Retcode search_for(void *data,int count,int source,int tag)
{
    ASSERT_ZERO(pthread_mutex_lock(&mutexx));
    
    message*x=first;
    LIST_CHECK
    if(finished_preceses[source])  {ASSERT_ZERO(pthread_mutex_unlock(&mutexx)); return MIMPI_ERROR_REMOTE_FINISHED;}
    wait_count=count;wait_source=source;wait_tag=tag;
    ASSERT_ZERO(pthread_cond_wait(&waiting, &mutexx));
    wait_count=0;

    x=first;
    LIST_CHECK
    if(finished_preceses[source])  {ASSERT_ZERO(pthread_mutex_unlock(&mutexx)); return MIMPI_ERROR_REMOTE_FINISHED;}
    ASSERT_ZERO(pthread_mutex_unlock(&mutexx));
    return MIMPI_ERROR_DEADLOCK_DETECTED; //lol xdd
}