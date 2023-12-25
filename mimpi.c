/**
 * This file is for implementation of MIMPI library.
 * */

#include "channel.h"
#include "mimpi.h"
#include "mimpi_common.h"

#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void* reciver(void* data)
{
    int from=*((int*)data);
    free(data);
    printf("%d \n", from);
    return 0;
}

pthread_t *threads;


void MIMPI_Init(bool enable_deadlock_detection) {
    channels_init();
    size = atoi(getenv("MIMPI_size"));
    int x;
    ASSERT_SYS_OK(chrecv(800,&x,sizeof(int)));
    ASSERT_SYS_OK(close(800));
    rank=x;


    //creating threads for every pipe
    pthread_attr_t attr;
    ASSERT_ZERO(pthread_attr_init(&attr));

    threads=malloc(sizeof(pthread_t)*size);
    for (int i = 0; i < size; i++) {
        if(i==rank) continue;
        int* arg = malloc(sizeof(int));
        *arg = i;
        ASSERT_ZERO(pthread_create(&threads[i], &attr, reciver, arg));
    }

}

void MIMPI_Finalize() {
    //TODO


    //killing threads not sure if it is nessecary 
    //for(int i=0;i<size;i++)
    //    if(i!=rank) pthread_cancel(threads[i]); 
    free(threads);
    channels_finalize();
}

int MIMPI_World_size() {
    return size;
}

int MIMPI_World_rank() {
    return rank;
}

MIMPI_Retcode MIMPI_Send(
    void const *data,
    int count,
    int destination,
    int tag
) {
    TODO
}

MIMPI_Retcode MIMPI_Recv(
    void *data,
    int count,
    int source,
    int tag
) {
    TODO
}

MIMPI_Retcode MIMPI_Barrier() {
    TODO
}

MIMPI_Retcode MIMPI_Bcast(
    void *data,
    int count,
    int root
) {
    TODO
}

MIMPI_Retcode MIMPI_Reduce(
    void const *send_data,
    void *recv_data,
    int count,
    MIMPI_Op op,
    int root
) {
    TODO
}