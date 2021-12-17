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

void p1synch();
void p2synch();
void p4synch();
void p5synch();
void p6synch();
void p7synch();
void p8synch();
void last();


union semun 
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
}semafor_struct;



int Server1, Server2, D, P1, P2, PR, T, S;

int progress = 0, ended = 0;

int s1_synch = 0;

void finished() {
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

int main(int argc, char * argv[]) {
    if (argc != 3) {
        printf("Wrong number of arguments\n");
        exit(0);
    }

    int serv1, serv2, shmem1, shmem2;
    int ppr1[2], ppr2[2];

    char pipe1_read_char[10];
    char pipe2_read_char[10];
    char pipe2_write_char[10];
    char pipe1_write_char[10];

    char semaphore1_char[10];
    char semaphore2_char[10];

    char sharedmem1_char[10];
    char sharedmem2_char[10];

    char P1_descriptor[10];
    char P2_descriptor[10];

    char server1_char[10];
    char server2_char[10];

    if (pipe(ppr1) == -1)
        printf("Couldn't create pipe r1\n");
    if (pipe(ppr2) == -1)
        printf("Couldn't create pipe r2\n");

    int pipe1_read = dup(ppr1[0]);
    int pipe1_write = dup(ppr1[1]);

    int pipe2_read = dup(ppr2[0]);
    int pipe2_write = dup(ppr2[1]);

    int sem1 = semget(4001, 2, 0666 | IPC_CREAT);
    int sem2 = semget(4010, 2, 0666 | IPC_CREAT);

    if (sem1 == -1) {
        printf("Couldn't create sem1\n");
        fprintf(stderr, "semget failed\n");
		exit(EXIT_FAILURE);
    }
    if (sem2 == -1){
        printf("Couldn't create sem2\n");
        fprintf(stderr, "semget failed\n");
		exit(EXIT_FAILURE);
    }

    semctl(sem1, 0, SETVAL, 0);
    semctl(sem2, 0, SETVAL, 0);
    semctl(sem1, 1, SETVAL, 0);
    semctl(sem2, 1, SETVAL, 0);

	semafor_struct.val = 1;
	semctl (sem1, 0, SETVAL, semafor_struct);
	semafor_struct.val = 0;
	semctl (sem1, 1, SETVAL, semafor_struct);

	semafor_struct.val = 1;
	semctl(sem2, 0, SETVAL, semafor_struct);
	semafor_struct.val = 0;
	semctl(sem2, 1, SETVAL, semafor_struct);

    //shared memory

    if ((shmem1 = shmget(2005, 200*sizeof(char), 0666 | IPC_CREAT)) == -1){
        printf("Couldn't create shared memory 1\n");
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
    }
    if ((shmem2 = shmget(2007, 200*sizeof(char), 0666 | IPC_CREAT)) == -1){
        printf("Couldn't create shared memory 2\n");
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
    }
    

    memset(pipe1_read_char, '\0', sizeof(pipe1_read));
    memset(pipe2_read_char, '\0', sizeof(pipe2_read));
    memset(pipe1_write_char, '\0', sizeof(pipe1_write));
    memset(pipe2_write_char, '\0', sizeof(pipe2_write));
    memset(semaphore1_char, '\0', sizeof(sem1));
    memset(semaphore2_char, '\0', sizeof(sem2));
    memset(sharedmem1_char, '\0', sizeof(shmem1));
    memset(sharedmem2_char, '\0', sizeof(shmem2));

    sprintf(pipe1_read_char, "%d", pipe1_read);
    sprintf(pipe2_read_char, "%d", pipe2_read);
    sprintf(pipe1_write_char, "%d", pipe1_write);
    sprintf(pipe2_write_char, "%d", pipe2_write);
    sprintf(semaphore1_char, "%d", sem1);
    sprintf(semaphore2_char, "%d", sem2);
    sprintf(sharedmem1_char, "%d", shmem1);
    sprintf(sharedmem2_char, "%d", shmem2);

    /////////////////////////////////////////////////////////////////////////

    signal(SIGUSR1, p1synch);
    P1 = fork();

    switch (P1) {
        case 0:
            printf("P1 is starting...\n");
            execl("proc_p1", "proc_p1", pipe1_write_char, pipe1_read_char, (char * ) NULL);
            exit(0);
            break;
        case -1:
            printf("P1 wasn't started\n");
            break;
            exit(1);
    }

    while (progress != 1) {
        printf("Waiting for P1 to start...\n");
        sleep(3);
    }

    /////////////////////////////////////////////////////////////////////////

    signal(SIGUSR1, p2synch);

    P2 = fork();

    switch (P2) {
        case 0:
            printf("P2 was started successfully\n");
            execl("proc_p2", "proc_p2", pipe1_write_char, pipe1_read_char, (char * ) NULL);
            exit(0);
            break;
        case -1:
            printf("P2 wasn't started\n");
            exit(1);
            break;
    }

    while (progress != 3) {
        printf("Waiting for P2 to start...\n");
        sleep(3);
    }

    /////////////////////////////////////////////////////////////////////////

    memset(P1_descriptor, '\0', sizeof(P1));
    memset(P2_descriptor, '\0', sizeof(P2));

    sprintf(P1_descriptor, "%d", P1);
    sprintf(P2_descriptor, "%d", P2);

    /////////////////////////////////////////////////////////////////////////

    // signal(SIGUSR1, p3synch);
    PR = fork();
    switch (PR) {
        case 0:
            printf("Process PR was forked\n");
            execl("proc_pr", "proc_pr", P1_descriptor, P2_descriptor, pipe1_read_char, pipe2_write_char, NULL);
            exit(EXIT_SUCCESS);
            break;
        case -1:
            perror("Process D wasn't forked\n");
            exit(EXIT_FAILURE);
    }

    int state;
	waitpid(PR, &state, WUNTRACED);
	kill(P1,SIGUSR2);
	kill(P2,SIGUSR2);


    /////////////////////////////////////////////////////////////////////////

    signal(SIGUSR1, p4synch);
    signal(SIGUSR2, last);
    Server2 = fork();

    switch (Server2) {
        case 0:
            printf("Server 2 was forked\n");
            execl("proc_serv2", "proc_serv2", argv[2], (char * ) NULL);
            exit(0);
            break;
        case -1:
            perror("Couldn't fork server 2");
            exit(1);
            break;
    }

    while (progress != 4) {
        printf("Waiting for serv2 to start... %d %d\n", progress, s1_synch);
        sleep(3);
    }

    /////////////////////////////////////////////////////////////////////////

    signal(SIGUSR1, p5synch);
    Server1 = fork();

    switch (Server1) {
        case 0:
            printf("Server 1 was forked\n");
            execl("proc_serv1", "proc_serv1", argv[1], argv[2], (char * ) NULL);
            exit(0);
            break;
        case -1:
            perror("Couldn't fork server 1");
            exit(1);
            break;
    }

    while (progress != 5) {
        printf("Waiting for serv1 to start...\n");
        sleep(3);
    }

    /////////////////////////////////////////////////////////////////////////

    signal(SIGUSR1, p6synch);
    T = fork();
    switch (T) {
        case 0:
            printf("Process T was forked\n");
            execl("proc_t", "proc_t", pipe2_read_char, sharedmem1_char, semaphore1_char, NULL);
            exit(EXIT_SUCCESS);
            break;
        case -1:
            perror("Process T wasn't forked\n");
            exit(EXIT_FAILURE);
    }

    while (progress != 6) {
        printf("Waiting for T to start...\n");
        sleep(3);
    }

    /////////////////////////////////////////////////////////////////////////

    signal(SIGUSR1, p7synch);

    D = fork();

    switch (D) {
        case 0:
            printf("Process D was forked\n");
            execl("proc_d", "proc_d", sharedmem2_char, semaphore2_char, argv[1], (char * ) NULL);
            exit(0);
            break;
        case -1:
            perror("Process D wasn't forked");
            exit(0);
            break;
    }

    while (progress != 7) {
        printf("Waiting for D to start...\n");
        sleep(3);
    }

    /////////////////////////////////////////////////////////////////////////

    signal(SIGUSR1, p8synch);
    S = fork();
    switch (S) {
        case 0:
            printf("Process S was forked\n");
            execl("proc_s", "proc_s", sharedmem1_char, semaphore1_char, sharedmem2_char, semaphore2_char, NULL);
            exit(EXIT_SUCCESS);
            break;
        case -1:
            perror("Process S wasn't forked\n");
            exit(EXIT_FAILURE);
    }

    while (progress != 8) {
        printf("Waiting for end...\n");
        sleep(3);
    }

    while (ended == 0 && progress == 8){
        printf("Almost done...\n");
        sleep(3);
    }

    finished();

    close(atoi(argv[1]));
    close(atoi(argv[2]));
    sleep(3);
    printf("\nEnd of zadanie!\n");

    return 0;
}

void last(){
    printf("wrapping up...\n");
    ended = 1;
}

void p1synch() {
    if (progress == 0) {
        printf("Process p1 was done\n");
        progress = 1;
    }
}

void p2synch() {
    if (progress == 1) {
        printf("Process p2 was done\n");
        progress = 3;
    }
}

void p4synch() {
    if (progress == 3) {
        printf("Process serv2 was done\n");
        progress = 4;
    }
}

void p5synch() {
     if (progress == 4) {
        printf("Process serv1 was done\n");
        progress = 5;
    }
}

void p6synch() {
    if (progress == 5) {
        printf("Process T was done\n");
        progress = 6;
    }
}

void p7synch() {
    if (progress == 6) {
        printf("Process D was done\n");
        progress = 7;
    }
}

void p8synch() {
    if (progress == 7) {
        printf("Process S was done\n");
        progress = 8;
    }
}