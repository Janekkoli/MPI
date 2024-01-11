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
    recv_bytes+=ret;
    to_go-=ret;
    }
    return 0;
}


int safe_chsend(int __fd, const void *__buf, size_t __n)
{
    int ret,send_bytes=0,to_go=__n;
    while(to_go>0){
    ASSERT_SYS_OK(ret=chsend(__fd,__buf+send_bytes,to_go));
    to_go-=ret;
    send_bytes+=ret;
    }
    return 0;
}


pthread_mutex_t mutexx; 
pthread_cond_t waiting;

int wait_count,wait_source,wait_tag;


message *first,*last;



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

int search_for(void *data,int count,int source,int tag)
{
    ASSERT_ZERO(pthread_mutex_lock(&mutexx));
    message*x=first;
    while(x->next)
    {
        x=x->next;
        if(x->count==count && x->source==source && x->tag==tag)
        {
            data=x->data;
            ASSERT_ZERO(pthread_mutex_unlock(&mutexx));
            //printf("xxd");
            return 1;
        }
    }
    wait_count=count;wait_source=source;wait_tag=tag;
    ASSERT_ZERO(pthread_cond_wait(&waiting, &mutexx));
    x=first;
     while(x->next)
    {
        x=x->next;
        if(x->count==count && x->source==source && x->tag==tag)
        {
            //data=x->data;
            memcpy(data, x->data, count);
            free(x->data);
            //usuniecie elementu listy !!!!
            ASSERT_ZERO(pthread_mutex_unlock(&mutexx));
            return 1;
        }
    }
    ASSERT_ZERO(pthread_mutex_unlock(&mutexx));
    return 0;
}