#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

#include <sys/stat.h>
#include <unistd.h>


#define PORT 80
#define SIZE 8
#define MSGSIZE 1024

int readLine(int s, char *line, int *result_size) {
    int acum=0, size;
    char buffer[SIZE];

    while( (size=read(s, buffer, SIZE)) > 0) {
        if (size < 0) return -1;
        strncpy(line+acum, buffer, size);
        acum += size;
        if(line[acum-1] == '\n' && line[acum-2] == '\r') {
            break;
        }
    }

    *result_size = acum;

    return 0;
}

int writeLine(int s, char *line, int total_size) {
    int acum = 0, size;
    char buffer[SIZE];

    if(total_size > SIZE) {
        strncpy(buffer, line, SIZE);
        size = SIZE;
    } else  {
        strncpy(buffer, line, total_size);
        size = total_size;
    }

    while( (size=write(s, buffer, size)) > 0) {
        if(size<0) return size;
        acum += size;
        if (acum >= total_size) break;

        size = ((total_size-acum)>=SIZE)?SIZE:(total_size-acum)%SIZE;
        strncpy(buffer, line+acum, size);
    }

    return 0;
}

/**************DESARROLLO***************/

    /*
    Toma por parámetro un comando y lo compara con los comandos HTTP
    devolviendo un valor (int) en cada caso.
    6 LOC
    */

    int commandParser(char * command) {
      if (!strcmp(command,"GET")) return 0;
      if (!strcmp(command,"PUT")) return 1;
      if (!strcmp(command,"POST")) return 2;
      if (!strcmp(command,"HEAD")) return 3;
      return -1;
    }

    /*
      commandExecutor toma como parámetro un valor numérico
      para luego hacer un fetch a través de un switch del método
      a ejecutar.
      6 LOC
    */
    void commandExecutor(int value, char * command) {
      switch (value) {
        case 0: printf("%s\n","Ejecutar pasos de GET");break;
        case 1: printf("%s\n","Ejecutar pasos de PUT");break;
        case 2: printf("%s\n","Ejecutar pasos de POST");break;
        case 3: printf("%s\n","Ejecutar pasos de HEAD");break;
      }
    }
/*********FIN*DESARROLLO***********/

int serve(int s) {
    char command[MSGSIZE];
    int size, r, nlc = 0, fd, read_bytes;
    // Lee lo que pide el cliente
    while(1) {
        r = readLine(s, command, &size);
        //printf("Valor bruto de size: %d -> [%c] [%d] [%d]\n",size,command[0],command[1],command[2]);
        command[size-2] = 0;
        size-=2;
        //printf("Valor nomial de size: %d\n",size);
        //printf("[%s - %d - %c]\n", command,size,command[size-1]);
        printf("[%s]\n",command);

/**************DESARROLLO***************/
/*=========================================*/
          int aux_value = commandParser(command);
          commandExecutor(aux_value, command);
/*=========================================*/

        /*
          extractParameters toma el comando y extrae aquellos parámetros
          necesarios para la interpretación de la cabacera HTTP
        */
        void * extractParameters (char * command) {


        }



/*********FIN*DESARROLLO***********/



        if(command[size-1] == '\n' && command[size-2] == '\r') {
          printf("Sí\n");
            break;
        }
    	// Esto esta mal mal mal
    	if(strlen(command) == 0) {
    	    break;
    	}
    }
    sleep(1);

    sprintf(command, "HTTP/1.0 200 OK\r\n");
    writeLine(s, command, strlen(command));

    sprintf(command, "Date: Fri, 31 Dec 1999 23:59:59 GMT\r\n");
    writeLine(s, command, strlen(command));

    sprintf(command, "Content-Type: image/jpeg\r\n");
    writeLine(s, command, strlen(command));

    sprintf(command, "Content-Length: 29936\r\n");
    writeLine(s, command, strlen(command));

    sprintf(command, "\r\n");
    writeLine(s, command, strlen(command));
/*
    FILE *fin = fopen("mainiso_forcampus.jpg", "r");
	FILE *fout = fdopen(s, "w");

	struct stat buf;

	stat("mainiso?forcampus.jpg", &buf);
	printf("Size -----------> %d\n", buf.st_size);

	char file[32*1024];
	int suma = 0;
	size = fread(file, 1, 29936, fin);
	printf("Archivo: %d\n", size);

    while( (size=write(s, &file[suma], MSGSIZE)) > 0) {
		suma += size;
		if (suma >= 29936) break;
	}
*/


    sync();
}

int main() {
    int sd, sdo, addrlen, size, r;
    struct sockaddr_in sin, pin;

    // 1. Crear el socket
    sd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(PORT);

    // 2. Asociar el socket a un IP/puerto
    r = bind(sd, (struct sockaddr *) &sin, sizeof(sin));
	if (r < 0) {
		perror("bind");
		return -1;
	}
    // 3. Configurar el backlog
    listen(sd, 5);

    addrlen = sizeof(pin);
    // 4. aceptar conexión
    while( (sdo = accept(sd, (struct sockaddr *)  &pin, &addrlen)) > 0) {
        //if(!fork()) {
            printf("Conectado desde %s\n", inet_ntoa(pin.sin_addr));
            printf("Puerto %d\n", ntohs(pin.sin_port));

            serve(sdo);

            close(sdo);
            exit(0);
        //}
    }
    close(sd);

    sleep(1);

}
