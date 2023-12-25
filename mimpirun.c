/**
 * This file is for implementation of mimpirun program.
 * */

#include "mimpi_common.h"
#include "channel.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char* argv[]) {
    //moge dodac zmienne srodowoskowe zaczynajace sie od MIMPI :)
    channels_init();
    size = atoi(argv[1]);
    char dest[14]="MIMPI_size=";
    strncat(dest,argv[1],2);
    putenv(dest);
    //setting chanels
    int pipefd[2];
    for(int i=0;i<size;i++)
        for(int i2=0;i2<size;i2++)
        {
            if(i2==i) continue;
            ASSERT_SYS_OK(channel(pipefd)); //0 - read , 1 - write
            ASSERT_SYS_OK(dup2(pipefd[0],from_y_to_x(i,i2,0)));
            ASSERT_SYS_OK(close(pipefd[0]));
            ASSERT_SYS_OK(dup2(pipefd[1],from_y_to_x(i,i2,1)));
            ASSERT_SYS_OK(close(pipefd[1]));
        }



    //printf("%d size\n", size);
    pid_t pid;
    for(int i=0;i<size;i++)
    {
        ASSERT_SYS_OK(pid = fork());
        if(!pid) //child
        {
            for(int i2=0;i2<size;i2++) //closing unused descriptors
                for(int i3=0;i3<size;i3++)
                    {if(i3==i2) continue;
                    if(i3!=i) ASSERT_SYS_OK(close(from_y_to_x(i2,i3,0)));
                    if(i2!=i) ASSERT_SYS_OK(close(from_y_to_x(i2,i3,1)));}
            ASSERT_SYS_OK(dup2(from_y_to_x((i+1)%size,i,0),800));
            ASSERT_SYS_OK(execvp(argv[2],argv+2));
        }
        ASSERT_SYS_OK(chsend(from_y_to_x((i+1)%size,i,1),&i,sizeof(int)));
    }



    //wiating for all proceses to end
    int status; //exit status
    pid_t child_pid;
    for (int i = 0; i < size; ++i) {
        ASSERT_SYS_OK(child_pid = waitpid(-1, &status, 0));

        // printf("Child %d (PID: %d) terminated.\n", i + 1, child_pid);

        // if (WIFEXITED(status)) {
        //     printf("Child %d exited with status %d.\n", i + 1, WEXITSTATUS(status));
        // } else {
        //     printf("Child %d exited abnormally.\n", i + 1);
        // }
    }

    for(int i=0;i<size;i++)
        for(int i2=0;i2<size;i2++)
            if(i==i2) continue;
            else {ASSERT_SYS_OK(close(from_y_to_x(i,i2,0)));ASSERT_SYS_OK(close(from_y_to_x(i,i2,1)));}
    channels_finalize();
    return 0;
    //valgrind --tool=memcheck --leak-check=yes --track-origins=yes --track-fds=yes  ./mimpirun 16 examples_build/hello
}