#include <stdio.h>
#include <stdlib.h> //for atoi()
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>



int getch(void);
void error(char *errorMessage);

//the main function
int main(int argc, char* argv[]){
    //checks if input arguments are less, or more than 2 => returns the error message
    if(argc != 2){ 
        error("Error: Incorrect number of input arguments\n"); 
    }

    int firstPort = atoi(argv[1]);  //converts the string argument argv[1] to an integer (firstPort)
    //if firstPort is 0, than we will have a mistake (error message)
    if(firstPort == 0){ //checks, if converting has a mistake
        error("Error: Input argument isn't number\n");  //return the mistake message
    }

    int firstSocket = socket(AF_INET, SOCK_DGRAM, 0);   // function shall create an unbound socket in a communications domain
    //AF_INET (AF - Address Family) -  communication domain, refers to addresses from the internet, IP addresses specifically
    //SOCK_DGRAM - Provides datagrams, which are connectionless-mode, unreliable messages of fixed maximum length.

    if(firstSocket == -1){ //if we can't create a socket, than returns error message
        error("Error in creating a socket\n"); 
    }

    //open the file "serv2.txt", with flags and 0777 mod
    int file = open("serv2.txt", O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, 0777);
    if(file == -1){ //if we can't open the file, than it returns error message
        error("Error in opening the file\n"); 
    }


    struct sockaddr_in client;  //the basic structure for all syscalls and functions that deal with internet addresses
    // Filling server information

    /*      Information about "struct sockaddr_in" and "struct in_addr"
    struct sockaddr_in {
        short            sin_family;   // e.g. AF_INET
        unsigned short   sin_port;     // e.g. htons(3490)
        struct in_addr   sin_addr;     // see struct in_addr, below
        char             sin_zero[8];  // zero this if you want to
    };

    struct in_addr {
        unsigned long s_addr;  // load with inet_aton()
    };
    */

    client.sin_family = AF_INET; //IPv4 protocol
    // inet_aton("127.0.0.1", &client.sin_addr.s_addr); or we can write like this:
    //function shall convert the string pointed to by cp, in the standard IPv4 dotted decimal notation, to an integer value suitable for use as an Internet address
    client.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    client.sin_port = htons(firstPort); //translates a short integer from host byte order to network byte order

    //assigns the address specified by &client to the socket referred to by the file descriptor firstSocket. 
    // sizeOfSocket specifies the size, in bytes, of the address structure pointed to by client
    socklen_t sizeOfSocket = sizeof(struct sockaddr);
    if(bind(firstSocket, (struct sockaddr*) &client, sizeOfSocket) < 0){
        error("Error in bind\n");   //if it has an error, than it returns the error message
    }
    // kill(getppid(), SIGUSR1);

    char buffer[4096];  //create a buffer for read and write the file's content

    int element, input;
    while((element = read(firstSocket, buffer, 4096)) > 0){
        write(file, buffer, element);
    }


    //kill(getppid(), SIGUSR2);
    //the exit of the program
    close(firstSocket);
    exit(EXIT_SUCCESS);

    return 0;
}

// a method for an error message and exit with failure 
void error(char *errorMessage){
    fprintf(stderr,"[./proc_serv2] %s", errorMessage);

    exit(EXIT_FAILURE);
}

//create this method for having a mistake with declaration method in the code (old version of the code)
int getch(void){
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}
