#include <stdio.h>
#include <stdlib.h>     //for method "atoi"
#include <fcntl.h>      //for open
#include <signal.h>     //for signal

__sighandler_t sendSignal(int pipeFilem, int file);    //can't use type void

int main(int argc, char *argv[]){
    /*int pipe = fork();        а я фиг знает, как провнерить id и pipe, для активации кода
    printf("%d\n", pipe);*/

    int pipeFile = atoi(argv[1]);
    if(pipeFile == 0){
        perror("Error: incorrent input argv[1] !");
        exit(1);
    }
    int openFile;
    if((openFile = open("p1.txt", O_RDONLY)) == -1){
        perror("Error: can't open the file p1.txt !");
        exit(-1);
    }

    if(signal(SIGUSR1, sendSignal(pipeFile, openFile)) != SIG_ERR){
        printf("p1.txt is successful!\n");
    } else{
        perror("Error with signal!");
        exit(EXIT_FAILURE);
    }

    kill(getppid(), SIGUSR1);
    return 0;
}

__sighandler_t sendSignal(int pipeFile, int file){
    char buf[1000];
	int readFile, writeFile;

	while((readFile = read(file, buf, 1000)) > 0){
		if((writeFile = write(file, buf, readFile)) == -1){
            perror("Error: Nepodarilo sa zapisat do pipe (P1)!");
            exit(-2);
        }
	}

	close(file);
	return SIG_IGN;
}
