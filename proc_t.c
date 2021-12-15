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
#include <sys/sem.h>    // struct sembuf, semop

//write define SEM_UNDO..., since I have a problem with decleration in VS Code
#define SEM_UNDO 0x1000
//struct for work with semop() and etc.
struct sembuf structSmbuff[1];

// создание дополнительных методов
// create methods
void error(char *error);    //for print and return error message
__sighandler_t exitSuccess();   //for successful exit
void change0(); 
void change1();
void writing(int file, char* smbuffer, size_t size);


int main (int argc, char** argv){
    // Егор тут что-то накурлыкал, не всё понял :D 
    // возможная замена atoi на strol, нужно ещё разобраться, почему
    //converts the string argument argv[1] to an integer (pipe), 
    //converts the string argument argv[2] to an integer (sm),
    //converts the string argument argv[3] to an integer (semphr);
    int pipe = atoi(argv[1]), sm = atoi(argv[2]), semphr = atoi(argv[3]); //советую поменять на strtol https://stackoverflow.com/questions/17710018/why-shouldnt-i-use-atoi

    //struct sembuf structSmbuff[1] = NULL; //перенёс в глобальную переменную, для того, чтобы легче было считывать показания в других методах
    char writeString[200]; // массив (хранилище) для хранение слов (размер, которого составляет в 200 единиц символов)
    int side = 0;   // для обозначения местоположение курсора в массиве writeString
    char* smbuffer;

    //The shmat() function attaches the shared memory segment associated with the shared memory identifier, sm, to the address space of the calling process.
    smbuffer = shmat(sm, NULL, 0);
 
    //if smbuffer is NULL, than return the error message
    if (smbuffer == NULL) {
        error ("couldn'd attach shared memory");    //prints the error message and returns EXIT_FAILURE
    }

    //The kill() function sends a signal to a process or process group specified by pid (getppid()).
    //The kill() function is successful if the process has permission to send the signal sig(SIGUSR1) to any of the processes specified by pid (getppid()). 
    //If kill() is not successful, no signal is sent.
    kill(getppid(), SIGUSR1);
    
    // продолжение кода Егора (попытка 1) 

    // создание сигнала SIGUSR2 с последующим удачным выходом
    // signal() sets the disposition of the signal "SIGUSR2" to "exitSuccess()"
    // SIGUSR2	1	Intended for use by user applications
    // If unsuccessful, signal() returns a value of SIG_ERR and a positive value in errno
    signal(SIGUSR2, exitSuccess());

	//файл, с помощью которого можно узнать, какое слово получил сервак и отослал 
    //open the file 
	int file = open("help.txt", O_CREAT | O_WRONLY | O_TRUNC, 0777); 
    // проверка, если с какой-то причины - файл не открылся 
    //if opening the file has a problem, than returns the error message
    if(file == -1){ 
        error("problem with opening the file"); //returns the error message
    }

    // цикл (cycle)
	while(1){
		// вписывание семафору 0
		change0();
        //semop() performs operations on selected semaphores in the set
        //indicated by semphr.  Each of the 1 (nsops) elements in the array
        //pointed to by structSmbuff is a structure that specifies an operation to
        //be performed on a single semaphore.  The elements of this
        //structure are of type struct sembuf, containing the following
        //members:
            //unsigned short sem_num;  /* semaphore number */
            //short          sem_op;   /* semaphore operation */
            //short          sem_flg;  /* operation flags */
        //Flags recognized in sem_flg are IPC_NOWAIT and SEM_UNDO.  If an
        //operation specifies SEM_UNDO, it will be automatically undone
        //when the process terminates.
		semop(semphr, structSmbuff, 1);

		// чтение с pipeR2
        //read form the pipeR2
        for(char element; element != '\n'; side++)
        {
            //read from pipe to writeString by 1 element
            read(pipe, &writeString[side], 1);
            //element = one of the element from writeString[]
            element = writeString[side];

            //if our element is '\n', than it will be the end 
            if(element == '\n')
            {
                element = '\0';
                writeString[side - 1] = '\0';
            }
        }

		// запись с переменной для помощи в память
        //copy information
        //The strcpy() function copies the string pointed by writeString(source) (including the null character) to the smbuffer (destination).
		strcpy(smbuffer, writeString);  // копирование строки
        //write into the file all smbuffer content
		writing(file, smbuffer, strlen(smbuffer));  //запись контента в файл с массива, с последующим окончание заполнения, через "\n"
		side = 0;
		
		// вписывание семафору 1
		change1();
        //semop() performs operations on selected semaphores in the set
        //indicated by semphr.  Each of the 1 (nsops) elements in the array
        //pointed to by structSmbuff is a structure that specifies an operation to
        //be performed on a single semaphore.  The elements of this
        //structure are of type struct sembuf, containing the following
        //members:
            //unsigned short sem_num;  /* semaphore number */
            //short          sem_op;   /* semaphore operation */
            //short          sem_flg;  /* operation flags */
        //Flags recognized in sem_flg are IPC_NOWAIT and SEM_UNDO.  If an
        //operation specifies SEM_UNDO, it will be automatically undone
        //when the process terminates.
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
// смена семафоры на 0 (write the theory before, look for the lines 75-86)
void change0(){
    structSmbuff[0].sem_num = 0;
	structSmbuff[0].sem_op = -1;
	structSmbuff[0].sem_flg = SEM_UNDO;
}
// смена семафоры на 1 (write the theory before, look for the lines 116-127)
void change1(){
    structSmbuff[0].sem_num = 1;
	structSmbuff[0].sem_op = 1;
	structSmbuff[0].sem_flg = SEM_UNDO;
}
// запись в файл
//write into the file smbuffer content with "\n" at the end
void writing(int file, char* smbuffer, size_t size){
    write(file, smbuffer, size);
    write(file, "\n", 1);
}
