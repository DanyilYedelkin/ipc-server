#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/wait.h> 

#include <limits.h>

// int synch = 1;

int Server1, Server2, D, P1, P2, PR, T, S;

void finished(){
    sleep(5);
    kill(P1, SIGKILL);
    kill(P2, SIGKILL);
    kill(PR, SIGKILL);
    kill(T, SIGKILL);
    kill(S, SIGKILL);
    kill(D, SIGKILL);
    kill(Server1, SIGKILL);
    kill(Server2, SIGKILL);
    
}


int main(int argc, char *argv[]){
    if (argc != 3) {
        printf("Wrong number of arguments\n");
        exit(0);
    }

    signal(SIGUSR1, finished);

    int serv1, serv2, shmem1, shmem2;

    int ppr1[2], ppr2[2];


    char pipe1_read_char[10];
    char pipe2_read_char[10];
    char pipe2_write_char[10];
    char pipe1_write_char[10];

    char semaphore1_char[10];
    char semaphore2_char[10];

    char sharedmem1_char[10] ;
    char sharedmem2_char[10];

    char P1_descriptor[10];
    char P2_descriptor[10];


    char server1_char[10];
    char server2_char[10];



    if (pipe(ppr1) == -1) printf("Couldn't create pipe r1\n");
    if (pipe(ppr2) == -1) printf("Couldn't create pipe r2\n");

    int pipe1_read = dup(ppr1[0]);
    int pipe1_write = dup(ppr1[1]);

    int pipe2_read = dup(ppr2[0]);
    int pipe2_write = dup(ppr2[1]);


    int sem1 = semget(IPC_PRIVATE, 2, 0666 | IPC_CREAT);
    int sem2 = semget(IPC_PRIVATE, 2, 0666 | IPC_CREAT);

    if (sem1 == -1) printf("Couldn't create sem1\n");
    if (sem2 == -1) printf("Couldn't create sem2\n");

    semctl(sem1,0,SETVAL,0);
	semctl(sem2,0,SETVAL,0);
	semctl(sem1,1,SETVAL,0);
	semctl(sem2,1,SETVAL,0);

    char *p;
    errno = 0;

    ///serv ports


    serv1 = (int)strtol(argv[1], &p, 10); // converting argv[1] to int

    

    if (*p != '\0' || errno != 0){
        printf("Invalid argv[1]\n");
    }

    errno = 0;

    serv2 = (int)strtol(argv[2], &p, 10); // converting argv[1] to int

    

    if (*p != '\0' || errno != 0){
        printf("Invalid argv[2]\n");
    }

    //shared memory

    shmem1 = shmget (IPC_PRIVATE, 256, 0666 | IPC_CREAT);
    shmem2 = shmget (IPC_PRIVATE, 256, 0666 | IPC_CREAT);
    
    memset(pipe1_read_char, '\0', sizeof (pipe1_read));
    memset(pipe2_read_char, '\0', sizeof (pipe2_read));
    memset(pipe1_write_char, '\0', sizeof (pipe1_write));
    memset(pipe2_write_char, '\0', sizeof (pipe2_write));
    memset(semaphore1_char, '\0', sizeof (sem1));
    memset(semaphore2_char, '\0', sizeof (sem2));
    memset(sharedmem1_char, '\0', sizeof (shmem1));
    memset(sharedmem2_char, '\0', sizeof (shmem2));
    memset(server1_char, '\0', sizeof (serv1));
    memset(server2_char, '\0', sizeof (serv2));

    sprintf(pipe1_read_char, "%d", pipe1_read);
    sprintf(pipe2_read_char, "%d", pipe2_read);
    sprintf(pipe1_write_char, "%d", pipe1_write);
    sprintf(pipe2_write_char, "%d", pipe2_write);
    sprintf(semaphore1_char, "%d", sem1);
    sprintf(semaphore2_char, "%d", sem2);
    sprintf(sharedmem1_char, "%d", shmem1);
    sprintf(sharedmem2_char, "%d", shmem2);
    sprintf(server1_char, "%d", serv1);
    sprintf(server2_char, "%d", serv2);



    /////////////////////////////////////////////////////////////////////////

    printf("\tStarting P1 process\n");
    P1 = fork();

    switch(P1){
        case 0 :
            printf("P1 was started successfully\n");
            execl("proc_p1", "proc_p1", pipe1_write_char, (char*)NULL);
            break;
        case -1 :
            printf("P1 wasn't started\n");
            break;
    }

    // pause();
    signal (SIGUSR1, NULL);

    /////////////////////////////////////////////////////////////////////////

    printf("\tStarting P2 process\n");
    P1 = fork();

    switch(P1){
        case 0 :
            printf("P2 was started successfully\n");
            execl("proc_p2", "proc_p2", pipe1_write_char, (char*)NULL);
            break;
        case -1 :
            printf("P2 wasn't started\n");
            break;
    }

    // pause();



    /////////////////////////////////////////////////////////////////////////

    printf("\tInitializing Server 1\n");
    Server1 = fork();

    switch (Server1) {
        case 0 :
            printf("Server 1 was forked\n");
            execl("proc_serv1", "proc_serv1", server1_char, server2_char, (char*)NULL);
            break;
        case -1 :
            perror("Couldn't fork server 1");
            break;
    }

    // pause();//suspends the process until signal arrives


    /////////////////////////////////////////////////////////////////////////

    printf("\tInitializing Server 2\n");
    Server2 = fork();

    switch (Server2) {
        case 0 :
            printf("Server 2 was forked\n");
            execl("proc_serv2", "proc_serv2", server2_char, (char*)NULL);
            break;
        case -1 :
            perror("Couldn't fork server 2");
            break;
    }

    // pause();//suspends the process until signal arrives

    /////////////////////////////////////////////////////////////////////////

    printf("\tInitializing Process D\n");

    D = fork();

    switch (D){
        case 0 :
            printf("Process D was forked\n");
            execl("proc_d", "proc_d", sharedmem2_char, semaphore2_char, server1_char, (char*) NULL);
            break;
        case -1 :
            perror("Process D wasn't forked");
            break;
    }

    // pause();

    /////////////////////////////////////////////////////////////////////////

    memset(P1_descriptor, '\0', sizeof(P1));
    memset(P2_descriptor, '\0', sizeof(P2));

    sprintf (P1_descriptor, "%d", P1);
    sprintf (P2_descriptor, "%d", P2);

    /////////////////////////////////////////////////////////////////////////

    PR = fork();
        switch (PR) {
            case 0 :
                printf("Process PR was forked\n");
                execl("proc_pr", "proc_pr", P1_descriptor, P2_descriptor, pipe1_read_char, pipe2_write_char, NULL);
                exit(EXIT_SUCCESS);
                break;
            case -1 :
                perror("Process D wasn't forked\n");
                exit(EXIT_FAILURE);
        }


    /////////////////////////////////////////////////////////////////////////

    T = fork();
        switch (T) {
            case 0 :
                printf("Process T was forked\n");
                execl("proc_t", "proc_t", pipe2_read_char, sharedmem1_char, semaphore1_char, NULL);
                exit(EXIT_SUCCESS);
                break;
            case -1 :
                perror("Process T wasn't forked\n");
                exit(EXIT_FAILURE);
        }


    /////////////////////////////////////////////////////////////////////////

    S = fork();
        switch (S) {
            case 0 :
                printf("Process S was forked\n");
                execl("proc_s", "proc_s", sharedmem1_char, semaphore1_char, sharedmem2_char, semaphore2_char, NULL);
                exit(EXIT_SUCCESS);
                break;
            case -1 :
                perror("Process D wasn't forked\n");
                exit(EXIT_FAILURE);
        }


    // finished();
    return 0;
}