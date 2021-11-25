/*#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

void error(char *msg);

int main(int argc, char *argv[]){
    int sock, length, fromlen, n;
    struct sockaddr_in server;
    struct sockaddr_in from;
    char buf[1024];
    from.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(argc < 2){
        fprintf(stderr, "ERROR, no port provided\n");
        exit(0);
    }

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0){
        error("Opening socket");
    }
    length = sizeof(server);
    bzero(&server, length);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(atoi(argv[1]));
    if(bind(sock, (struct sockaddr *)&server, length) < 0){
        error("blinding");
    }
    fromlen = sizeof(struct sockaddr_in);

    while(1){
        n = recvfrom(sock, buf, 1024, 0, (struct sockaddr *)&from, &fromlen);
        if(n < 0){
            error("recvfrom");
        }
        write(1, "Received a datagram: ", 21);
        write(1, buf, n);
        n = sendto(sock, "Got your message\n", 17, 0, (struct sockaddr *)&from, fromlen);
        if(n < 0){
            error("sendto");
        }
    }

    return 0;
}

void error(char *msg){
    perror(msg);
    exit(0);
}*/


#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>


int getch(void)
{
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


#define BUFFER 1024 * 4

void error(char *error);

int main(int argc, char* argv[]) {

    if(argc != 2) { error("incorrect number of input arguments \n"); }

    int port1 = atoi(argv[1]);
    if(port1 == 0) { error("input argument isn't number \n"); }

    struct sockaddr_in client;

    int socket1 = socket(AF_INET, SOCK_DGRAM, 0);
    if(socket < 0) { error("error socket\n"); }

    int file = open("serv2.txt", O_CREAT | O_WRONLY | O_TRUNC | O_APPEND, 0777);
    if(file < 0) { error("error txt file \n"); }

    // Filling server information
    client.sin_family = AF_INET; //IpV4;
    client.sin_port = htons(port1);
    client.sin_addr.s_addr = inet_addr("127.0.0.1");

    if(bind(socket1, (struct sockaddr*) &client, sizeof(struct sockaddr)) < 0) {
        error("error bind\n");
    }
    //kill(getppid(),SIGUSR1);

    char str[BUFFER];
    //socklen_t size = sizeof(struct sockaddr);

    /*for(int i = 0; i < 20; i++) {
        if((recv(socket1, str, BUFFER, 0)) < 0) {
            error("error recv");
        }
        write(file, str, BUFFER);
    }*/

    int c, input;
    while((c = read(socket1, str, BUFFER)) > 0) {
        write(file, str, c);
    }

    /*if(recvfrom(socket1, str, BUFFER, 0, (struct sockaddr*) &server, &size) < 0) {
        error("error recvfrom");
    }*/

    //kill(getppid(),SIGUSR2);

    close(socket1);
    exit(EXIT_SUCCESS);
}

void error(char *error) {
    fprintf(stderr,"[./proc_serv2] %s", error);
    exit(EXIT_FAILURE);
}
