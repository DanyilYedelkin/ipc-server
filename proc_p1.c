#include <stdio.h>
#include <stdlib.h>     //for method "atoi"
#include <fcntl.h>      //for open
#include <signal.h>     //for signal
 
__sighandler_t sendSignal(int pipeFilem, int file);    //can't use type void

int main(int argc, char *argv[]){

    int fd1 = atoi(argv[1]);   //converts the string argument argv[1] to an integer (fd1)
    // int fd2 = atoi(argv[2]);   
    //if fd1 is 0, than we will have a mistake (error message)
    if(fd1 == 0){  //checks, if converting has a mistake
        perror("Error: incorrent input argv[1] !");     //return the mistake message
        exit(EXIT_FAILURE);
    }
	// if(fd2 == 0){  //checks, if converting has a mistake
    //     perror("Error: incorrent input argv[2] !");     //return the mistake message
    //     exit(EXIT_FAILURE);
    // }

    int openFile;   //for creating a file
    if((openFile = open("p1.txt", O_RDONLY)) == -1){  //opens the file and ckecks, if opening has a mistake
        perror("Error: can't open the file p1.txt !");  //return the mistake message
        exit(EXIT_FAILURE);
    }

    // signal() sets the disposition of the signal "SIGUSR1" to "sendSignal(fd1, openFile)"
    // SIGUSR1	1	Intended for use by user applications
    // If unsuccessful, signal() returns a value of SIG_ERR and a positive value in errno
    kill(getppid(), SIGUSR1);

	signal(SIGUSR1, (void*)0);
    
	sendSignal(fd1, openFile);

	signal(SIGUSR2, (void*)0);

	printf("P1 зrocess is turning off...\n";)

	while (1){
		sleep(10);
	}


    return 0;
}


__sighandler_t sendSignal(int fd1, int file){
    //create a buffer for read and write file's elements
    char buffer[200];
	int readFile, writeFile;

	// while((readFile = read(file, buffer, 1000)) > 0){   //if opening the file is successful
	// 	write(fd1, buffer, readFile);
	// }

	if ((readFile = read(file, buffer, 1000)) == -1){
		printf("Couldn't read p1.txt\n");
	}

	if ((writeFile = write(fd1, buffer, readFile) == -1)){
		printf("Couldn't write to pipe P1\n");
	}

    //close the file
	close(file);
	return SIG_IGN;
}
