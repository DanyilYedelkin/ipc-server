#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h> 
#include <sys/wait.h> 
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

FILE *fd;
size_t len = 200;
int output;

void writePipe()
{
	char *buffer = NULL;
	buffer = calloc(len, sizeof(char));
	ssize_t readLine = getline(&buffer, &len, fd);	//read a delimited record from fd (FILE*)
	ssize_t writeLine = write(output, buffer, strlen(buffer));
	if (readLine == -1 || writeLine == -1)
	{
		printf("With read || write pipe proc_p1\n");	//error message
		exit(EXIT_FAILURE);	//exit with failure
	}
	// printf("P1: SUCCESSFUL \n");
	free(buffer);
	// printf("\n");
}
void killPipe()
{
	fclose(fd);
	printf("P1: finished!\n");
	exit(EXIT_SUCCESS);
}
int main(int argc, char *argv[])
{
	if (argc < 3)	//check number of parameters
	{
		printf("Usage %s <ppid> <pipe[0]>\n", argv[0]);	//error message
		exit(EXIT_FAILURE);	//failure exit
	}
	//The kill() function sends a signal to a process or process group specified by pid (getppid()).
    //The kill() function is successful if the process has permission to send the signal sig(SIGUSR1) to any of the processes specified by pid (getppid()). 
    //If kill() is not successful, no signal is sent.
	kill(getppid(), SIGUSR1);
	output = atoi(argv[1]);		//write the pipe, atoi converts to int 
	if ((fd = fopen("p1.txt", "r")) == NULL)	//open the file and checks it
	{
		perror("p1.txt wasn't opened!\n");	//error message
		exit(EXIT_FAILURE);	//failure exit
	}
	printf("Signal Send to write \n");	//write current situation
	signal(SIGUSR1, writePipe); //sets the disposition of the signal SIGUSR1 to writePipe
	signal(SIGUSR2, killPipe); //sets the disposition of the signal SIGUSR2 to killPipe
	printf("END proc_p1\n");	//write current situation
	while (1)
	{
        pause();
	}
	if (fd != NULL)
		fclose(fd);
	exit(0);
}
