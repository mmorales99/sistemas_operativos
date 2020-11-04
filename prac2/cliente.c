#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> // funciones auxiliares como inet_addr()
#include <string.h> // strtok
#include <sys/wait.h> // wait
#include <unistd.h> // close
#include <fcntl.h>

#define TRUE 1
#define BUFF_SIZE 99999

int main(int argc, char**argv){

    int sc_servidor; 
    sc_servidor = socket(AF_INET,SOCK_STREAM,0); // socket abierto // int my_file = open("oawkd.c");
    struct sockaddr_in servidor_info;
    servidor_info.sin_family=AF_INET; // PF_INET
    servidor_info.sin_port=htons(9999);
    servidor_info.sin_addr.s_addr = INADDR_ANY; //cleinte-> server_info.sin_addr.s_addr = inet_addr(argv[2]);
    // servidor_info.sin_zero //

    /* cliente*/
    connect(sc_servidor,(struct sockaddr *)&servidor_info,sizeof(servidor_info));
    char buff[BUFF_SIZE];
    unsigned int i = 2;
    while(argv[i]!=NULL){
        strcat(buff,argv[i]);
        strcat(buff," ");
        i++;
    }
    strcat(buff,"\0");
    i=0;
    while (buff[i++]!='\0');
    buff[i-2]='\0';
    write(sc_servidor,buff,sizeof(buff)); // send(sc_servidor,buff,sizeof(buff), 0);
    memset(buff,0,sizeof(buff));
    read(sc_servidor,buff,sizeof(buff)); // recv(sc_servidor,buff,sizeof(buff), 0);
    printf("%s",buff);
    fflush(stdin);
    close(sc_servidor);
    return 0;
}