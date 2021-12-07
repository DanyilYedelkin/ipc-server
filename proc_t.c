#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

int main (int argc, char* argv[]){
    int pipe = atoi(argv[1]), sm = atoi(argv[2]), semphr = atoi(argv[3]);

    // struct sembuf structSembuf[1] = NULL;

    char * smbuffer;

    smbuffer = shmat(sm, NULL, 0);
 
    if (smbuffer == NULL) {
        error ("couldn'd attach shared memory");
    }

    kill(getppid(), SIGUSR1);


}