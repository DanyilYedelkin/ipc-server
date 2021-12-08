#include <stdio.h>
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX, INT_MIN
#include <stdlib.h>  // for strtol
#include<stdbool.h>  
#include <signal.h>
#include <netinet/in.h>

bool checkMemory(long memory, int errno);
bool errorPrint(int errno, char *p, long memory); //check argv
void exit_signal();

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

    //unsigned short sem_num;  /* semaphore number */
    //short          sem_op;   /* semaphore operation */
    //short          sem_flg;  /* operation flags *
    /* Поскольку сокет является только лишь абстракцией, то связка IP-адрес + номер порта - это уже имплементация в ОС. 
    Верное название этой имплементации - "Интернет сокет". Абстракция используется для того, чтобы операционная система могла работать 
    с любым типом канала передачи данных. Именно поэтому в ОС Linux Интернет сокет - 
    это дескриптор, с которым система работает как с файлом. Типов сокетов, конечно же, намного больше. 
    В ядре ОС Linux сокеты представлены тремя основными структурами:
    
    В ядре ОС Linux сокеты представлены тремя основными структурами:
    struct socket - представление сокета BSD, того вида сокета, который стал основой для современных "Интернет сокетов";
    struct sock - собственная оболочка, которая в Linux называется "INET socket";
    struct sk_buff - "хранилище" данных, которые передает или получает сокет;

    socket - создание сокета;

    bind - действие используется на стороне сервера. В стандартных терминах - это открытие порта на прослушивание, 
    используя указанный интерфейс;

    listen - используется для перевода сокета в прослушивающее состояние. Применяется к серверному сокету;
    connect - используется для инициализации соединения;
    accept - используется сервером, создает новое соединение для клиента;
    send/recv - используется для работы с отправкой/приемом данных;
    close - разрыв соединения, уничтожение сокета.

    struct sockaddr_in 
    {
            short            sin_family;    e.g. AF_INET -- AF_INET is an address family that is used to designate the type of addresses. 
            unsigned short   sin_port;      e.g. htons(3490) -- The htons() function converts the unsigned short integer 
                                                 hostshort from host byte order to network byte order.
            struct in_addr   sin_addr;      see struct in_addr, below
            char             sin_zero[8];   zero this if you want to
    };

    int sockfd = socket(domain, type, protocol)*/  

    int sockf;
    if((sockf = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Error Yehor Code, socket time");
    }     
       
}

void exit_signal()
{
    printf("Turn off");
    exit(EXIT_SUCCESS);
}
void signalRemoveTake()
{
        kill(getppid(), SIGUSR1); //read more directly about that
        signal(SIGUSR2, exit_signal); 
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
