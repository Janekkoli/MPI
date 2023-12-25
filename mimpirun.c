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
    {
        ASSERT_SYS_OK(channel(pipefd));
        //printf("%d %d \n",pipefd[0],pipefd[1]); //nie za dobrze :(
        ASSERT_SYS_OK(dup2(pipefd[0],70));
        ASSERT_SYS_OK(close(pipefd[0]));
        ASSERT_SYS_OK(dup2(pipefd[1],71));
        ASSERT_SYS_OK(close(pipefd[1]));
        ASSERT_SYS_OK(dup2(70,MIMPI_recive_point+i));
        ASSERT_SYS_OK(close(70));
        ASSERT_SYS_OK(dup2(71,MIMPI_send_point+i));
        ASSERT_SYS_OK(close(71));
        
    }
    //printf("%d size\n", size);
    pid_t pid;
    for(int i=0;i<size;i++)
    {
        ASSERT_SYS_OK(pid = fork());
        if(!pid) //child
        {


            ASSERT_SYS_OK(dup2(MIMPI_recive_point+i,MIMPI_recive_point));
            for(int i2=1;i2<size;i2++)
                    ASSERT_SYS_OK(close(MIMPI_recive_point+i2));
            
            rank=i;
            ASSERT_SYS_OK(execvp(argv[2],argv+2));
        }
        int x=i;
        ASSERT_SYS_OK(chsend(i+MIMPI_send_point,&x,sizeof(int)));
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
    channels_finalize();
    return 0;
}