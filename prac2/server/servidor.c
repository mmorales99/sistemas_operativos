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

int main(){

    int ss, sc;
    struct sockaddr_in as, ac;
    as.sin_family = AF_INET;
    as.sin_port = htons(9999);
    as.sin_addr.s_addr = INADDR_ANY;
    ss = socket(AF_INET,SOCK_STREAM, 0);
    if(ss<0) perror("NO SE HA PODIDO CREAR EL SOCKET\n\0");
    bind(ss, (struct sockaddr *) &as, sizeof(as));
    int isListening = listen(ss, 5);
    if(isListening<0) perror("ERROR ESCUCHANDO\n\0");

    do{
        int ac_leng = sizeof(ac);
        sc = accept(ss, (struct sockaddr*)&ac, &ac_leng);
        if(sc==-1) continue; // si el socket recivido da error se relanza la escucha
        char buff[BUFF_SIZE];
        recv(sc,&buff,BUFF_SIZE, 0); // hace un read del socket, recv es equivalente 
        if(strcmp("terminate", buff)==0) break; // si recive el comando terminate, sale y ejecuta el protoclo de muerte
        char *comando[BUFF_SIZE];
        char *segments;
        int total=0;
        char *_buff;
        _buff = strdup(buff);
        switch (fork())
        {
        case -1:
            perror("NO SE HA PODIDO GENERAR UN HIJO\n\0");
            exit(EXIT_FAILURE);
            break;
        case 0:
            while((segments = strsep(&_buff," \0"))) comando[total++] = strdup(segments);
            comando[total] = (char *)0;
            memset(buff,0,sizeof(buff));
            int p[2];
            pipe(p);
            if(fork()==0){
                close(p[0]);
                close(stdout); // cerar stdout
                dup(p[1]); // mover al escritor de la tuberia
                if(execvp(comando[0],comando)<0)
                    perror("NO SE HA PODIDO LANZAR EL EXEC\n\0");
            }
            wait(NULL);
            close(p[1]);
            read(p[0],buff,sizeof(buff));
            close(p[0]);
            send(sc,buff,sizeof(buff),0);
            close(sc);
            exit(EXIT_SUCCESS);
            break;
        default:
            break;
        }
    }while(TRUE);
    close(ss);
    return 0;
}