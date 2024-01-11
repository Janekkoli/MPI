/**
 * This file is for implementation of MIMPI library.
 * */

#include "channel.h"
#include "mimpi.h"
#include "mimpi_common.h"


#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

pthread_t *threads;


void* reciver(void* data)
{
    message *mes;
    int from=*((int*)data);
    int recive_from=from_y_to_x(from,rank,0),count,source,tag;
    free(data);
    //printf("%d \n", from);
    int *init_block;
    ASSERT_NOT_NULL(init_block = malloc(sizeof(int)*3)); //count,source,tag
    while(true)
    {
        ASSERT_SYS_OK(chrecv(recive_from,init_block,sizeof(int)*3));
        count=init_block[0];source=init_block[1];tag=init_block[2];
        printf("transfer count %d from %d to %d tag %d\n",count,source,rank,tag);
        ASSERT_NOT_NULL(data = malloc(count));
        
        //later have to add what happen when not all date is transferd
        ASSERT_SYS_OK(chrecv(recive_from,data,count));
        //int *b = (int*) data;
        //printf("%d date recived\n", *b);
        ASSERT_NOT_NULL(mes = malloc(sizeof(message)));
        mes->data=data;mes->count=count;mes->source=source;mes->tag=tag;
        add_mes(mes);
    }
    return 0;
}




void MIMPI_Init(bool enable_deadlock_detection) {
    channels_init();
    size = atoi(getenv("MIMPI_size"));
    //int x;
    // ASSERT_SYS_OK(chrecv(800,&x,sizeof(int)));
    // ASSERT_SYS_OK(close(800));
    // rank=x;
    char dest[40]="MIMPI_",*x;
    sprintf(dest+6, "%d", getpid());
    ASSERT_NOT_NULL(x=getenv(dest));
    rank=*x-'1';
    ASSERT_SYS_OK( unsetenv(dest));
    //inits
    mes_init();

    //creating threads for every pipe
    pthread_attr_t attr;
    ASSERT_ZERO(pthread_attr_init(&attr));
    int* arg;
    ASSERT_NOT_NULL(threads=malloc(sizeof(pthread_t)*size));
    for (int i = 0; i < size; i++) {
        if(i==rank) continue;
        ASSERT_NOT_NULL(arg = malloc(sizeof(int)));
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

MIMPI_Retcode MIMPI_Send(void const *data, int count, int destination, int tag) {
    //have to add retcodes!!!!
    int init_send[3]={count,rank,tag};
    ASSERT_SYS_OK(chsend(from_y_to_x(rank,destination,1),&init_send,sizeof(int)*3));

    ASSERT_SYS_OK(chsend(from_y_to_x(rank,destination,1),data,count));
    return 0;
}




MIMPI_Retcode MIMPI_Recv(void *data,int count,int source,int tag) {
    //have to add retcodes !!!!!!!!!!!
    search_for(data,count,source,tag);
    return 0;
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