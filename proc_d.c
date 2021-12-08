#include <stdio.h>
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX, INT_MIN
#include <stdlib.h>  // for strtol
#include<stdbool.h>  
#include <signal.h>

bool checkMemory(long memory, int errno);
bool errorPrint(int errno, char *p, long memory); //check argv
//unsigned short sem_num;  /* semaphore number */
//short          sem_op;   /* semaphore operation */
//short          sem_flg;  /* operation flags */ 
int main(int argc, char *argv[])
{
    long memory, port, semafor;

    char *p;
    errno = 0;
    
    memory = strtol(argv[1], &p, 10); //https://stackoverflow.com/questions/9748393/how-can-i-get-argv-as-int/38669018
    errorPrint(errno, p, memory);
    
    //check memory
    checkMemory(memory, errno);
    

    semafor = strtol(argv[2], &p, 10);
    errorPrint(errno, p, semafor);

    port = strtol(argv[3], &p, 10);
    errorPrint(errno, p, semafor);      

    signalRemoveTake();       

    
       
}
void signalRemoveTake()
{
        kill(getppid(), SIGUSR1); //read more directly about that
        signal(SIGUSR2, killMe); 
}
bool errorPrint(int errno, char *p, long memory)
{
    if(errno != 0 || *p != '\0' || memory > INT_MAX || memory < INT_MIN)
    {
        printf("Problem in proc_d Yehor code - problem with argv[] %s\n", strerror(errno));
        exit(EXIT_FAILURE);
        return false;
    }   
    return true;
}
bool checkMemory(long memory, int errno) //https://man7.org/linux/man-pages/man2/shmat.2.html
{
    char *check;
    if(((check) = shmat(memory, NULL, 0)) == NULL) //problem with memory -1
    {
        printf("Error: memory in Code Yegor  %s\n", strerror(errno));
        fprintf(stderr, "shmat failed\n");
        exit(EXIT_FAILURE);
        return false;
    }
    return true;
}
