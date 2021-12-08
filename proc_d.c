#include <stdio.h>
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX, INT_MIN
#include <stdlib.h>  // for strtol
#include<stdbool.h>  
#include <signal.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/sem.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <arpa/inet.h> //inet_addr
bool checkMemory(long memory, int errno);
bool errorPrint(int errno, char *p, long memory); //check argv
void exit_signal();
void semafor_write(int sockf, char *p, int semafor);
void signalRemoveTake();
//Конечно главный вопрос как передается ерно
int main(int argc, char *argv[])
{
    long memory, port, semafor;

    char *p;
    errno = 0;
    
    memory = strtol(argv[1], &p, 10); //https://stackoverflow.com/questions/9748393/how-can-i-get-argv-as-int/38669018
    errorPrint(errno, p, memory);
    
    //check memory
    if(checkMemory(memory, (int)errno) == true) p = shmat(memory, NULL, 0);
    

    semafor = strtol(argv[2], &p, 10);
    errorPrint((int)errno, p, semafor);

    port = strtol(argv[3], &p, 10);
    errorPrint((int)errno, p, semafor);      

    signalRemoveTake();       


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




    int sockfd = socket(domain, type, protocol)*/  

    int sockf;
    if((sockf = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error Yehor Code, socket time");
    }    

 
    struct sockaddr_in server_adress;  //client information

    /*struct sockaddr_in 
    {
            short            sin_family;    e.g. AF_INET -- AF_INET is an address family that is used to designate the type of addresses. 
            unsigned short   sin_port;      e.g. htons(3490) -- The htons() function converts the unsigned short integer 
                                                 hostshort from host byte order to network byte order.
            struct in_addr   sin_addr;      see struct in_addr, below
            char             sin_zero[8];   zero this if you want to
    }; */

	server_adress.sin_family = AF_INET; 
	server_adress.sin_port = htons(port);
	server_adress.sin_addr.s_addr = inet_addr("127.0.0.1"); //function shall convert the string pointed to by cp, in the standard IPv4 
	bzero(&(server_adress.sin_zero), 8); // erases the data in the n bytes of the memory starting at the location pointed to by server_adress

    
    semafor_write(sockf, p, semafor);

}
void semafor_write(int sockf, char *p, int semafor)
{
    struct sembuf sembuf_struct[1]; // structure semafor

    //unsigned short sem_num;  /* semaphore number */
    //short          sem_op;   /* semaphore operation */
    //short          sem_flg;  /* operation flags *

    //sem_op = positive -> he operation adds this value to the semaphore value (semval).
    //sem_op = zero ->  If sem_op is zero, the process must have read permission on the semaphore set.
    // sem_op < 0 -> If sem_op is less than zero, the process must have alter permission on the semaphore set.

    ///set_num =  Operate on semaphore 0 
    while(1)
    {
		sembuf_struct[0].sem_op = -1;
        sembuf_struct[0].sem_num = 1;
        //If an operation specifies SEM_UNDO, it will be automatically undone  when the process terminates.
		sembuf_struct[0].sem_flg = SEM_UNDO;
        //The semop() function performs semaphore operations atomically on a set of semaphores associated with argument semid.
		if(semop(semafor,sembuf_struct,1) == -1)
        {
            perror("Yehor code problem semafor 1: ");
            exit(EXIT_FAILURE);
        }
        if (write(sockf, p, strlen(p)+1) != (int)strlen(p)+1)
        {
			perror("ERROR: Yehor code, write socket in TCP : ");
			exit(EXIT_FAILURE);
        }
        memset(p,'\0', sizeof(p)); // чтобы поставить в конце символ конца
		sembuf_struct[0].sem_op = 1; 
		sembuf_struct[0].sem_num = 0;
		sembuf_struct[0].sem_flg = SEM_UNDO;
		if(semop(semafor,sembuf_struct,1) == -1)
        {
            perror("Yehor code problem semafor : ");
            exit(EXIT_FAILURE);
        }
        sleep(1);
    }
}
void exit_signal()
{
    printf("Turn off");
    exit(EXIT_SUCCESS);
}
void signalRemoveTake()
{
        kill(getppid(), SIGUSR1); //read more directly about that   update отправляем сигнал родителям 
        signal(SIGUSR2, exit_signal); //	Завершение	Пользовательский сигнал № 2
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
