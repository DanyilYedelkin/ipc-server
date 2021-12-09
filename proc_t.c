#include <stdio.h>
#include <stdlib.h>     //atoi
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/shm.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>
#include <sys/sem.h>    // struct sembuf, semop

#define SEM_UNDO 0x1000
struct sembuf structSmbuff[1];

// создание дополнительных методов
void error(char *error);
__sighandler_t exitSuccess();
void change0();
void change1();
void writing(int file, char* smbuffer, size_t size);


int main (int argc, char** argv){
    // Егор тут что-то накурлыкал, не всё понял :D 
    // возможная замена atoi на strol, нужно ещё разобраться, почему
    int pipe = atoi(argv[1]), sm = atoi(argv[2]), semphr = atoi(argv[3]); //советую поменять на strtol https://stackoverflow.com/questions/17710018/why-shouldnt-i-use-atoi

    //struct sembuf structSmbuff[1] = NULL; //перенёс в глобальную переменную, для того, чтобы легче было считывать показания в других методах
    char writeString[200]; // массив (хранилище) для хранение слов (размер, которого составляет в 200 единиц символов)
    int side = 0;   // для обозначения местоположение курсора в массиве writeString
    char* smbuffer;

    smbuffer = shmat(sm, NULL, 0);
 
    if (smbuffer == NULL) {
        error ("couldn'd attach shared memory");
    }

    kill(getppid(), SIGUSR1);
    
    // продолжение кода Егора (попытка 1) 

    // создание сигнала SIGUSR2 с последующим удачным выходом
    signal(SIGUSR2, exitSuccess());

	//файл, с помощью которого можно узнать, какое слово получил сервак и отослал 
	int file = open("help.txt", O_CREAT | O_WRONLY | O_TRUNC, 0777); 
    // проверка, если с какой-то причины - файл не открылся 
    if(file == -1){ 
        error("problem with opening the file");
    }

    // цикл
	while(1){
		// вписывание семафору 0
		change0();
		semop(semphr, structSmbuff, 1);

		// чтение с pipeR2
        for(char c; c != '\n'; side++){
            read(pipe, &writeString[side], 1);
            c = writeString[side];
            if(c == '\n'){
                c = '\0';
                writeString[side-1] = '\0';
            }
        }

		// запись с переменной для помощи в память
		strcpy(smbuffer, writeString);  // копирование строки
		writing(file, smbuffer, strlen(smbuffer));  //запись контента в файл с массива, с последующим окончание заполнения, через "\n"
		side = 0;
		
		// вписывание семафору 1
		change1();
		semop(semphr, structSmbuff, 1);
	}

}

// метод для вывода ошибки
void error(char *error) {
    fprintf(stderr,"[./proc_t] %s", error);
    exit(EXIT_FAILURE);
}

//  ПРОДОЛЖЕНИЕ КОДА ЕГОРА
// метод для удачного выхода (причина создания - невозможно поменять void exit() на __sighandler_t exit())
__sighandler_t exitSuccess(){
    exit(EXIT_SUCCESS);
}
// смена семафоры на 0
void change0(){
    structSmbuff[0].sem_num = 0;
	structSmbuff[0].sem_op = -1;
	structSmbuff[0].sem_flg = SEM_UNDO;
}
// смена семафоры на 1
void change1(){
    structSmbuff[0].sem_num = 1;
	structSmbuff[0].sem_op = 1;
	structSmbuff[0].sem_flg = SEM_UNDO;
}
// запись в файл
void writing(int file, char* smbuffer, size_t size){
    write(file, smbuffer, size);
    write(file, "\n", 1);
}
