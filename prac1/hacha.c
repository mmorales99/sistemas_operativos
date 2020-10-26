// MANUEL MORALES AMAT DNI 48789509T

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/wait.h> // esta libreria no esta en los apuntes, sin ella la funcion wait no funciona correctamente
#include <fcntl.h>

void generate_file(int tam, char *new_file_name, char* name, int childs_born, int* pipe); // funcio que genera los archivos continentes del contenido del archivo origen

int count_digits(int); // funcion que cuenta los digitos de un numero, usada mas adelante

/**
 * ! Funcion principal, recive como parametros un nombre de archivo y un numero
 * ! Ejemplo de ejecucion: ./hacha holamundo 300 -> generara tantos hola.hX como tenga que hacer, cada archivo pesara 300 bytes
 * @param argc : int con el total de argumentos, siempre sera 1 o mayor que 1
 * @param argv : contiene los argumentos del programa
 * @return siempre devuelve 0 como senyal de ejecucion correcta
 */
int main(int argc, char *argv[]){
    if(argc!=3){ // si no hay suficientes argumentos el proceso padre muere con una señal de fallo
        perror("ERROR: Argumentos incorrectos.\nPueba con: ./hacha [Nombre archivo] [Tamanyo de los nuevos archivos]\n");
        exit(EXIT_FAILURE);
    }
    int tam = atoi(argv[2]); // tamanyo de los nuevos archivos
    char name[strlen(argv[1])]; // nombre del archivo origen
    strcpy(name,argv[1]); 
    int file = open(name,O_RDONLY); // se abre el archivo origen en formato de solo lectura
    if(file==-1){ // si no existe el archivo el proceso padre muere con una señal de fallo
        perror("ERROR: El archivo no se ha abierto correctamente o no existe.\n");
        exit(EXIT_FAILURE);
    }
    int file_start = lseek(file,0,SEEK_CUR); // obtenemos la direccion de inicio del archivo
    int file_end = lseek(file, 0, SEEK_END); // obtenemos la direccion final del archivo 
    int file_size = file_end-file_start; // calculamos la longitud del archivo
    lseek(file,file_start,SEEK_SET); // devolvemos a la posicion de inicio el archivo
    struct stat file_stats; // inicializamos una variable del struct stat para obtener los metadatos del archivo
    fstat(file,&file_stats); // obtenemos con fstat los metadatos del archivo - file_stats.size debe ser igual a file_size obtenido meidante recorrer todo el archivo; en caso de que sea un sparse_file es posible que file_size falle
    double total_childs = file_stats.st_size/(double)tam; // Aqui se calcula el total de hijos que se han de generar -> en caso de estar seguros de que no se usan archivos del tipo sparse_file usar :: double total_childs = file_size/(double)tam;  
    total_childs++; // se redondea siempre arriba para evitar que faltan trozos del archivo, es posible que se genere un archivo vacio
    int childs_born = 0; // contador del total de hijos que se va a generar
    int pipes[(int)total_childs][2]; // se genera el array que contendra las pipes de cada hijo donde se cargaran los trozos del archivo
    while(childs_born<(int)total_childs){ // se generan hijos desde 0 hasta el total - 1
        char buff[tam+1]; // buffer intermedio entre el archivo y la pipa
        char new_file_name[strlen(name)+3+count_digits((int)childs_born)]; // nuevo nombre del archivo generado, tiene 3 huecos mas para la terminacion .h y el '\0' + tantos huecos como childs_born necesite
        if(pipe(pipes[childs_born])){ // se generan los descriptores y se guardan en el vector de pipes en la posicion childs_born
            perror("ERROR: No se ha podido generar la pipe correspondiente al proceso.\n");
            exit(EXIT_FAILURE);
        }
        if(lseek(file,childs_born*tam,SEEK_SET)<0){ // desplazamos desde el inicio el puntero de lectura el tamanyo de los nuevos archivos multiplicado por el numero de hijo
            perror("ERROR: No se ha podido operar el archivo.\n");
            exit(EXIT_FAILURE);
        }
        if(read(file,&buff,tam)<0){ // leemos el archivo y lo almacenamos en el buffer
            perror("ERROR: No se ha podido leer el archivo.\n");
            exit(EXIT_FAILURE);
        }
        if(write(pipes[childs_born][1],&buff,tam)<0){ // escribimos el buffer en la pipe
            perror("ERROR: No se ha podido operar con la pipe.\n");
            exit(EXIT_FAILURE);
        }
        close(pipes[childs_born][1]); // cerramos el descriptor de escritura dado que no lo necesitamos mas
        switch (fork()){ // creamos los hijos
            case -1:
                perror("ERROR: No se ha podido generar el hijo.");
                exit(EXIT_FAILURE);
            break;
            case 0:
                generate_file(tam, new_file_name,name,childs_born,pipes[childs_born]); // generamos los archivos
            break;
        }
        childs_born++; // contamos los hijos generados
    }
    close(file); // cerramos el archivo por seguridad
    return 0;
}

/**
 * ! Genera un archivo con el contenido leido y pasado por la pìpe
 *  @param tam :t tamanyo del archivo
 *  @param new_file_name : nombre del nuevo archivo, es un puntero con memoria reservada para el nombre del archivo original y 5 caracteres mas para la terminacion .hX
 *  @param name : nombre del archivo original
 *  @param childs_born : contador que sirve para saber cuantos hijos se han generado, el hijo actual y cuanto se ha de desplazar dentro del archivo para leer
 *  @param pipe : tuberia de lectura donde esta el fragmento del archivo que se ha de copiar
 */ 
void generate_file(int tam, char *new_file_name, char* name, int childs_born, int* pipe){
    char child_buff[tam+1]; // buffer temporal intermedio donde se guarda el contenido 
    sprintf(new_file_name,"%s.h%d",name,childs_born); // se copia el nombre y se anyade el numero de hijo que esta tratando ese segmento del archivo, si es el hijo 3 se anyade la terminacion .h3
    int new_file = creat(new_file_name,0666); // se crea el archivo con permisos de escritura y lectura para todos los usuarios
    read(pipe[0],&child_buff,tam); // se lee de la pipe con los datos necesarios
    close(pipe[0]); // se cierra el descriptor de lectura, y por tanto la pipe dado que esta cerrado el descriptor de escritura
    write(new_file,&child_buff,tam); // se escribe el segmento del archivo origen en el nuevo archivo destino
    close(new_file); // se cierra en archivo y se guarda la informacion
    exit(EXIT_SUCCESS); // se mata el proceso con una salida correcta
}
/**
 * Cuenta los digitos que componen un entero
 * ! n solo puede tomar valores de 0 a Infinitos, siempre positivo
 * @param n : entero a analizar
 * @return i : total de digitos que componen n
 */ 
int count_digits(int n){
    int m = n; // copia 'defensiva' para evitar a toda costa de que n se modifique, realmente no es necesario
    int i=0;
    while (m>0){ // cuenta los digitos dividiendo entre 10 el numero hasta que sea 0        \
                    se podría hacer mas facil si simplemnte se hiciera el log10             \
                    pero la version del compilador y la libreria math de C no permiten su uso
        m=m/10;
        i++;
    }
    return i; // devolvemos la cantidad de digitos
}