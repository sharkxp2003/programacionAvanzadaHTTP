#include "../lib/server.h"



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
  9 métodos
 134 LOC
 14.8 LOC media
*/


    /*

    */
    char * strext(char * string) {
      int size = 0;
      if (string != NULL) {
        size = strlen(string);
        while (string[size]!='.' && size != 1) size--;
        return substr(string,size+1,strlen(string)-size);
      }
      return NULL;
    }


    char * fetchMIME(char * extension) {
      int i=0;
      char * mime = malloc(sizeof(char)*40);
      while (strcmp(mapaMimeTypes[i],extension) != 0 && i<84) i++;
      sprintf(mime,"%s",mapaMimeTypes[i+1]);
      return mime;
    }



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
      24 LOC
    */
    void * leer (char * path) {
      int size;
      struct data * dataLectura = malloc(sizeof(struct data));
      FILE * fp = NULL;
      fp = fopen(path,"rb");
      size_t result;
      if (fp != NULL) {
        dataLectura->extension = malloc(sizeof(char)*40);
        sprintf(dataLectura->extension,"%s",fetchMIME(strext(path)));
        fseek(fp,0,SEEK_END);
        size = ftell(fp);
        rewind(fp);
        dataLectura->size = size;
        if (strcmp(dataLectura->extension,"image/jpeg") == 0) {
          dataLectura->buffer = malloc(sizeof(char)*size);
          char file[size];
          size = fread(file,1,size,fp);
          memcpy(dataLectura->buffer,file,size);

        } else fread(dataLectura->buffer,1,size-1,fp);
        dataLectura->status = ok;

        sprintf(dataLectura->extension,"%s",fetchMIME(strext(path)));
        fclose(fp);
     } else {
        fp = fopen("./root/cabecerasHTTP/not_found.html","rb");
        dataLectura->status = not_found;
        fseek(fp,0,SEEK_END);
        size = ftell(fp);
        rewind(fp);
        dataLectura->buffer = malloc(sizeof(char)*size);
        dataLectura->size = size;
        sprintf(dataLectura->extension,"%s",fetchMIME(strext("./root/cabecerasHTTP/not_found.html")));
        fread(dataLectura->buffer,1,size,fp);
        }
      return (void *) dataLectura;
    }

    /*
      Lee desde una ruta un fichero y lo muestra por pantalla
      11 LOC
    */
    int leerDebug (char * path) {
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
/*

  20 LOC
*/
void * openAndReadFile(char * uri, char *root) {
  char * fullPath = NULL;
  char * path = NULL;
  char * uri2 = NULL;
  int a,b;
  void * aux;

  if (strcmp(uri,"/")==0) {
    aux = leer("./root/index.html");
  } else {
    a = strlen(substr(uri,1,strlen(uri)-1));
    uri2 = malloc(sizeof(char)*a);
    strcpy(uri2,substr(uri,1,strlen(uri)-1));
    b = strlen(root);
    path = malloc(sizeof(char)*b);
    strcpy(path,root);
    fullPath = malloc(sizeof(char)*a+b);
    sprintf(fullPath,"%s%s",path,uri2);
    aux = leer(fullPath);
  }
  free(path); free(uri2); free(fullPath);
  return aux;
}


    /*
      Método que se invoca tras reconocer que se llama al método
      http GET desde un cliente. Separa la línea del procedimiento
      del resto de la cabecera http. Extrae de la línea la uri y
      buscar en el directorio raíz los recursos que se piden.
      10 LOC
    */
    void * HTTPGET (char * command) {
      char * uri = NULL;
      char * linea = NULL;
      void * aux;
      int http_command_size = 0, proto_size = 9;
      char * root = "./root/";
      linea = substrUntilChar(command,'\r');
      http_command_size = strlen("GET ");//Tamaño del comando http
      uri = substr(linea,http_command_size,strlen(linea)-http_command_size-proto_size);
      aux = openAndReadFile(uri,root);
      return aux;
    }
    /*
      19 LOC
    */
    void configurarHeader(struct cabecera * header, struct data * dataLectura) {
      if (dataLectura->status == not_found) {
        header->resultado = malloc(sizeof(char)*30);
        sprintf(header->resultado,"HTTP/1.0 %u Not found\r\n",dataLectura->status);
        header->server = "Server: Mac OS X PA_Server\r\n";
        header->content_type = malloc(sizeof(char)*56);
        sprintf(header->content_type,"Content-Type: %s\r\n",dataLectura->extension);
        header->content_length = malloc(sizeof(char)*6);
        sprintf(header->content_length,"Content-Length: %d\r\n",dataLectura->size);
        header->cuerpo = malloc(sizeof(char *)*dataLectura->size);
        sprintf(header->cuerpo,"%s\r\n",dataLectura->buffer);
      }
      if (dataLectura->status == ok) {
        header->resultado = malloc(sizeof(char)*30);
        sprintf(header->resultado,"HTTP/1.0 %u ok\r\n",dataLectura->status);
        header->server = "Server: Mac OS X PA_Server\r\n";
        header->content_type = malloc(sizeof(char)*56);
        sprintf(header->content_type,"Content-Type: %s\r\n",dataLectura->extension);
        header->content_length = malloc(sizeof(char)*6);
        sprintf(header->content_length,"Content-Length: %d\r\n",dataLectura->size);
        header->cuerpo = malloc(sizeof(char)*dataLectura->size);
        memcpy(header->cuerpo,dataLectura->buffer,dataLectura->size);

        //sprintf(header->cuerpo,"%s\r\n",dataLectura->buffer);
      //  free(dataLectura->buffer);//OJO
      }
    }

    /*
      commandExecutor toma como parámetro un valor numérico
      para luego hacer un fetch a través de un switch del método
      a ejecutar.
      11 LOC
    */
    void * commandExecutor(int value, char * command) {
      struct cabecera * header = malloc(sizeof(struct cabecera));
      struct data * dataLectura;
      switch (value) {
        case 0://GET
        dataLectura = HTTPGET(command);
        configurarHeader(header,dataLectura);
        printf("%s",header->resultado);
        printf("%s",header->server);
        printf("%s",header->content_type);
        printf("%s",header->content_length);
        printf("%s",header->cuerpo);
        //char * prueba = malloc(sizeof(char)*100);
        //sprintf(prueba,"%s",strext("./root/cabecerasHTTP/not_found.jpeg"));
        //printf("%s\n",fetchMIME(prueba));
         break;
        case 1: printf("%s\n","Ejecutar pasos de PUT");break;
        case 2: printf("%s\n","Ejecutar pasos de POST");break;
        case 3: printf("%s\n","Ejecutar pasos de HEAD");break;
      }
      return (void *) header;
    }
/*********FIN*DESARROLLO***********/

int serve(int s) {
    char command[MSGSIZE*2];
    int size, r, nlc = 0, fd, read_bytes, operacion, i=0;
    struct cabecera * response;

    // Lee lo que pide el cliente
    while(1) {
        r = readLine(s, command, &size);

        /**************DESARROLLO***************/
        /*=========================================*/
        if (i==0) {
          operacion = commandParser(command);
          response = commandExecutor(operacion, command);
        }
        /*=========================================*/

        /*********FIN*DESARROLLO***********/

        i++;
        command[size-2] = 0;
        size-=2;
        if(command[size-1] == '\n' && command[size-2] == '\r') {
            break;
        }

    }
    sleep(1);
    writeLine(s,response->resultado,strlen(response->resultado));
    writeLine(s,response->server,strlen(response->server));
    writeLine(s,response->content_type,strlen(response->content_type));
    writeLine(s,response->content_length,strlen(response->content_length));
    writeLine(s,"\r\n",strlen("\r\n"));
    if (strcmp(response->content_type,"Content-Type: image/jpeg\r\n") == 0) {
      FILE *fin = fopen("./root/imagen.jpeg","r");
      FILE *fout = fdopen(s, "w");
      char file[65340];
      int suma = 0;
      size = fread(file, 1,65340, fin);
      printf("Archivo: %d\n", size);
      while( (size=write(s, &file[suma],size)) > 0) {
        suma += size;
        if (suma >= 65340) break;
      }
    } else {
        writeLine(s,response->cuerpo,strlen(response->cuerpo));

  }



/*
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
