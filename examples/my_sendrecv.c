#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include "../mimpi.h"
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    
    MIMPI_Init(false);
    int const world_rank = MIMPI_World_rank();
    int const world_size = MIMPI_World_size();

    char number = 42;
    int const tag = 1;

    MIMPI_Send(&number, sizeof(number), (1 + world_rank)%world_size, tag);
    number=0;
    MIMPI_Recv(&number, sizeof(number), (world_rank+world_size-1)%world_size, tag);
    printf("df %d\n",number);
    MIMPI_Finalize();
    return 0;
}