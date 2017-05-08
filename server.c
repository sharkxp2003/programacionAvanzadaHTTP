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
  7 métodos
 65 LOC
 9.4 LOC media
*/


    /*
      Devuelve un substring de un string dado con una longitud
      especificada
      5 LOC
    */
    char * substr(char * cadena, int comienzo, int longitud) {
      if (longitud == 0) longitud = strlen(cadena)-comienzo-1;
      char * nuevo = (char *)malloc(sizeof(char) * longitud);
      strncpy(nuevo, cadena+comienzo,longitud);
      return nuevo;
    }

    /*
    Toma por parámetro un comando y lo compara con los comandos HTTP
    devolviendo un valor (int) en cada caso.
    6 LOC
    */

    int commandParser(char * command) {
      char * aux = NULL;
      aux = substr(command,0,4);
      if (!strcmp(aux,"GET ")) return 0;
      if (!strcmp(aux,"PUT ")) return 1;
      if (!strcmp(aux,"POST")) return 2;
      if (!strcmp(aux,"HEAD")) return 3;
      return -1;
    }

    /*
    Devuelve el substring hasta la primera aparición del
    carácter c en el char * string
    5 LOC
    */
    char * substrUntilChar(char * string, char c) {
      int i = 0;
      while (string[i] != c) {
        i++;
      }
      return substr(string,0,i);
    }

    /*
      Lee desde una ruta un fichero y lo muestra por pantalla
      11 LOC
    */
    int leer (char * path) {
      char c;
      FILE * fp = NULL;
      fp = fopen(path,"r");
      if (fp == NULL) return -1;
        while (!feof(fp)){
          c = fgetc(fp);
          printf("%c",c);
      } printf("\n");
      fclose(fp);
      return 0;
    }

    /*
      Concatena la uri del cliente con el path simbólico donde se
      encuentra la carpeta que contiene los ficheros html y demás.
      23 LOC
    */
    void openAndReadFile(char * uri, char * root) {
      FILE * fp = NULL;
      char * fullPath = NULL;
      char * path = NULL;
      char * uri2 = NULL;
      int a,b;

      if (strcmp(uri,"/")==0) {
        leer("./root/index.html");
      } else {
        a = strlen(substr(uri,1,strlen(uri)-1));
        uri2 = malloc(sizeof(char)*a);
        strcpy(uri2,substr(uri,1,strlen(uri)-1));
        b = strlen(root);
        path = malloc(sizeof(char)*b);
        strcpy(path,root);
        fullPath = malloc(sizeof(char)*a+b);
        sprintf(fullPath,"%s%s",path,uri2);
        printf("%s\n",fullPath);
        int c = leer(fullPath);
        printf("%d\n",c);
        if (c == -1) {
          printf ("Error 404");
        }
          //Error 404
      }
      free(path); free(uri2); free(fullPath);
    }


    /*
      Método que se invoca tras reconocer que se llama al método
      http GET desde un cliente. Separa la línea del procedimiento
      del resto de la cabecera http. Extrae de la línea la uri y
      buscar en el directorio raíz los recursos que se piden.
      9 LOC
    */
    void HTTPGET (char * command) {
      char * uri = NULL;
      char * linea = NULL;
      int http_command_size = 0, command_size = 0, proto_size = 9;
      char * root = "./root/";
      linea = substrUntilChar(command,'\r');
      http_command_size = strlen("GET ");//Tamaño del comando http
      uri = substr(linea,http_command_size,strlen(linea)-http_command_size-proto_size);
      openAndReadFile(uri,root);
    }

    /*
      commandExecutor toma como parámetro un valor numérico
      para luego hacer un fetch a través de un switch del método
      a ejecutar.
      6 LOC
    */
    void commandExecutor(int value, char * command) {

      switch (value) {
        case 0://GET
        HTTPGET(command);
         break;
        case 1: printf("%s\n","Ejecutar pasos de PUT");break;
        case 2: printf("%s\n","Ejecutar pasos de POST");break;
        case 3: printf("%s\n","Ejecutar pasos de HEAD");break;
      }
    }
/*********FIN*DESARROLLO***********/

int serve(int s) {
    char command[MSGSIZE];
    int size, r, nlc = 0, fd, read_bytes, operacion;
    // Lee lo que pide el cliente
    while(1) {
        r = readLine(s, command, &size);

        command[size-2] = 0;
        size-=2;

/**************DESARROLLO***************/
/*=========================================*/
          operacion = commandParser(command);
          commandExecutor(operacion, command);
/*=========================================*/

/*********FIN*DESARROLLO***********/

        if(command[size-1] == '\n' && command[size-2] == '\r') {
            break;
        }
        /*
    	// Esto esta mal mal mal
    	if(strlen(command) == 0) {
    	    break;
    	}
      */
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
