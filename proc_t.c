#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

void error(char *error);

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

void error(char *error) {
    fprintf(stderr,"[./proc_serv2] %s", error);
    exit(EXIT_FAILURE);
}