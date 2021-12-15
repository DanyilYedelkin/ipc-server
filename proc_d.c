#include <stdio.h>
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX, INT_MIN
#include <stdlib.h>  // for strtol
#include<stdbool.h>  //bool function
#include <signal.h>  //use signal
#include <netinet/in.h> //AF_INET. 
#include <string.h>
#include <sys/sem.h> //sembuf
#include <unistd.h>  //I/O primitives fork,pipe
#include <sys/types.h> //pid_t
#include <sys/ipc.h> // All use a common structure type, ipc_perm to pass information used in determining
#include <sys/shm.h>  //schmat
#include <arpa/inet.h> //inet_addr
#include <fcntl.h>


void killSignal()
{
   	printf("Turn OFF\n");
	exit(EXIT_SUCCESS);
}

int main(int argc, char ** argv)
{

    int memory = atoi(argv[1]);
    int semafor = atoi(argv[2]);
    int port = atoi(argv[3]);

    char* shared_memory;
    shared_memory = shmat(memory, NULL, 0);
    if(shared_memory == NULL)
    {
        perror("Problem with shmat");
        exit(1);
    }
    else
    {
        printf("Memory work proc_d\n");
    }
    
    kill(getppid(), SIGUSR1);
    signal(SIGUSR2, killSignal);

    int socked = socket(AF_INET, SOCK_STREAM, 0);
    if(socked < 0)
    {
        perror("Socket proc_d");
        exit(1);
    }
    else
    {
        printf("Socket work proc_d\n");
    }
   
    //char buffer[1024];
	struct sembuf sem_b[1];
    
	struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serv_addr.sin_port = htons(port);
	bzero(&serv_addr.sin_zero, 8);

	if(connect(socked,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
    {
		perror("Connect proc_d");
		exit(1);
	}	
    int fd=open("1D.txt",O_CREAT|O_TRUNC|O_WRONLY,0777);
    while(1)
    {
        sem_b[0].sem_num = 1;
        sem_b[0].sem_op = -1;
        sem_b[0].sem_flg = SEM_UNDO;
        if(semop(semafor, sem_b, 1) < 0)
        {
            perror("Semop proc_d");
            exit(1);
        }
        if (write(socked, shared_memory, strlen(shared_memory)+1) != (int)strlen(shared_memory)+1)
        {
			printf("Problem write to socket ");
			exit(EXIT_FAILURE);
		}
        else
        {
			sleep(1);
			write(fd, shared_memory,strlen(shared_memory));
			write(fd, "\n",1);
		}

        //bzero(shared_memory, 1024*sizeof(char));
        sem_b[0].sem_num = 0;
        sem_b[0].sem_op = 1;
        sem_b[0].sem_flg = SEM_UNDO;
        if(semop(semafor,sem_b,1) < 0)
        {
			perror("Semop2 proc_d");
			exit(1);
		}
        
    }
}
