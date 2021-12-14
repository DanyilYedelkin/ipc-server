#include <stdio.h>
#include <stdlib.h>     //for method "atoi"
#include <fcntl.h>      //for open
#include <signal.h>     //for signal

__sighandler_t sendSignal(int pipeFilem, int file);    //can't use type void

int main(int argc, char *argv[]){

    int pipeFile = atoi(argv[1]);   //converts the string argument argv[1] to an integer (pipeFile)
    //if pipeFile is 0, than we will have a mistake (error message)
    if(pipeFile == 0){  //checks, if converting has a mistake
        perror("Error: incorrent input argv[1] !");     //return the mistake message
        exit(EXIT_FAILURE);
    }
    int openFile;   //for creating a file
    if((openFile = open("p1.txt", O_RDONLY)) == -1){  //opens the file and ckecks, if opening has a mistake
        perror("Error: can't open the file p1.txt !");  //return the mistake message
        exit(EXIT_FAILURE);
    }

    // signal() sets the disposition of the signal "SIGUSR1" to "sendSignal(pipeFile, openFile)"
    // SIGUSR1	1	Intended for use by user applications
    // If unsuccessful, signal() returns a value of SIG_ERR and a positive value in errno
    if(signal(SIGUSR1, sendSignal(pipeFile, openFile)) != SIG_ERR){
        printf("p1.txt is successful!\n");  //if signal() != SIG_ERR
    } else{
        perror("Error with signal!");   //if signal() == SIG_ERR, so it returns the mistake message
        exit(EXIT_FAILURE);
    }

    //The kill() function sends a signal to a process or process group specified by pid (getppid()).
    //The kill() function is successful if the process has permission to send the signal sig(SIGUSR1) to any of the processes specified by pid (getppid()). 
    //If kill() is not successful, no signal is sent.
    kill(getppid(), SIGUSR1);

    return 0;
}

__sighandler_t sendSignal(int pipeFile, int file){
    //create a buffer for read and write file's elements
    char buffer[1000];
	int readFile, writeFile;

	while((readFile = read(file, buffer, 1000)) > 0){   //if opening the file is successful
		write(file, buffer, readFile);
	}

    //close the file
	close(file);
	return SIG_IGN;
}
