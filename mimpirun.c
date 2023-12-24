/**
 * This file is for implementation of mimpirun program.
 * */

#include "mimpi_common.h"
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char* argv[]) {
    int n = atoi(argv[1]);
    int rank = -1;
    pid_t pid;
    for(int i=0;i<n;i++)
    {
        ASSERT_SYS_OK(pid = fork());
        if(!pid) //child
        {
            rank=i;

            ASSERT_SYS_OK(execvp(argv[2],argv+2));
        }

    }
}