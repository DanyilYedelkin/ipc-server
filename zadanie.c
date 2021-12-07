#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
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

int synch = 1;

void finished(){
    switch (synch){
        case 1 :
           printf("P1 has been run \n");
            synch ++; 
            break;
        case 2 :
            printf("P2 has been run \n");
            synch ++;
            break;
    }
    
}




int main(int argc, char *argv[]){
    if (argc != 3) {
        printf("Wrong number of arguments\n");
        exit(0);
    }

    int ppr1[2], ppr2[2];

    if (pipe(ppr1) == -1) printf("Couldn't create pipe r1\n");
    if (pipe(ppr2) == -1) printf("Couldn't create pipe r2\n");

    signal (SIGUSR1, finished); //signal listener

    char read[12], write[12];

    ////////       P1 PROCESS

    pid_t p1_id;

    // switch (p1_id = fork()) {
    //     case 0 :
    //         printf ("Starting process P1\n");
    //         sprintf (read,"%d", ppr1[0]);
    //         sprintf (write,"%d", ppr1[1]);
    //         execl("proc_p1", "proc_p1", read, write);
    //         break;
    //     case -1: 
    //         printf ("P1 wasn't started\n");
    //         break;
    //     default:
    //         printf ("P1 decided\n");
    // }

    p1_id = fork();

    printf("\n%d - p1 id\n", p1_id);

    if (p1_id > 0){
        printf ("Starting process P1\n");
            sprintf (read,"%d", ppr1[0]);
            sprintf (write,"%d", ppr1[1]);
            execl("proc_p1", "proc_p1", read, write);
    }
    else {
            printf ("P1 wasn't started\n");
    }

    while (synch == 1){
        printf ("Waiting for P2\n");
        sleep(3);
    }

    ////////       P2 PROCESS

    if (synch == 2) signal (SIGUSR1, finished); //signal listener


    pid_t p2_id = fork();

    // switch (p2_id = fork()) {
    //     case 0 :
    //         printf ("Starting process P2\n");
    //         sprintf (read,"%d", ppr2[0]);
    //         sprintf (write,"%d", ppr2[1]);
    //         execl("proc_p2", "proc_p2", read, write);
    //         break;
    //     case -1: 
    //         printf ("P2 wasn't started\n");
    //         break;
    //     default:
    //         printf ("P2 decided\n");
    // }

    printf("\n%d - p2 id\n", p2_id);

    if (p2_id > 0){
        printf ("Starting process P2\n");
            sprintf (read,"%d", ppr2[0]);
            sprintf (write,"%d", ppr2[1]);
            execl("proc_p2", "proc_p2", read, write);
    }
    else {
            printf ("P2 wasn't started\n");
    }

    while (synch == 2){
        printf ("Waiting for PR\n");
        sleep(3);
    }

    ///////// PR process

    pid_t pr_id;

    char read_r[12], write_r[12];
    sprintf (read_r, "%d", ppr1[0]);
    sprintf (write_r, "%d", ppr2[1]);

    switch (pr_id = fork()){
        case 0 :
            printf("Starting process PR\n");
            sprintf (read, "%d", p1_id);
            sprintf (write, "%d", p2_id);
            execl("proc_pr", "proc_pr", read, write, read_r, write_r, NULL);
            break;
        case -1 :
            printf ("PR wasn't started\n");
            break;
        default : 
            printf ("PR decided");
    }

    //waiting for pr to finish
    int st;
    waitpid(pr_id, &st, WUNTRACED);

    //turning off p1 and p2
    kill(p1_id,SIGUSR2);
    kill(p2_id,SIGUSR2);

}