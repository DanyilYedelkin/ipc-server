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
#include <fcntl.h>

void killPipe()
{
    printf("proc_p2 Turn OFF");
}
int main(int argc, char *argv[])
{

  if(argc < 3)
    {
    printf("Usage %s <ppid> <pipe[0]>\n" ,argv[0]);
    exit(1);
  }

  int fd = open("p2.txt", O_RDONLY);
  if(fd < 0)
    {
    perror("Problem with open file");
    exit(1);
  }
    int input = atoi(argv[1]);  
    int output = atoi(argv[2]);
    
    kill(getppid(), SIGUSR1);
 
    char buffer[200];

    ssize_t readLine = getline(buffer, 200, fd);
    ssize_t writeLine = write(output, 200, strlen(buffer));
    if(readLine == -1 || writeLine == -1) 
    {
        printf("With read||write Pipe_p2 problem \n");
        exit(1);
    }
    else
    {
        printf("P2: SUCCESSFUL\n");
    }
    signal(SIGUSR1, (void*)0);
    close(fd);
    signal(SIGUSR2, killPipe);
    while(1)
    {
        sleep(1);
    }
    return 0;
}
