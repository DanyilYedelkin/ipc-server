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
	ssize_t readLine = getline(&buffer, &len, fd);
	ssize_t writeLine = write(output, buffer, strlen(buffer));
	if (readLine == -1 || writeLine == -1)
	{
		printf("With read||write pipe proc_p1\n");
		exit(1);
	}
	printf("P1: SUCCESSFUL \n");
	free(buffer);
	printf("\n");
}
void killPipe()
{
	fclose(fd);
	printf("P1: finished!\n");
	exit(EXIT_SUCCESS);
}
int main(int argc, char *argv[])
{
	if (argc < 3)
	{
		printf("Usage %s <ppid> <pipe[0]>\n", argv[0]);
		exit(1);
	}
	kill(getppid(), SIGUSR1);
	output = atoi(argv[1]);
	if ((fd = fopen("p1.txt", "r")) == NULL)
	{
		perror("p1.txt wasn't opened!\n");
		exit(EXIT_FAILURE);
	}
	printf("Signal Send to write \n");
	signal(SIGUSR1, writePipe);
	signal(SIGUSR2, killPipe);
	printf("END proc_p1\n");
	while (1)
	{
		sleep(1);
	}
	if (fd != NULL)
		fclose(fd);
	exit(0);
}
