#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


int main(void) {
	printf("tararara %d\n", getpid());
    while(1){
		sleep(3);
    }
}


