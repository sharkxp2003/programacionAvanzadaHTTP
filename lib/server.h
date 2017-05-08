#ifndef H_SERVER
  #define H_SERVER
  #define PORT 80
  #define SIZE 8
  #define MSGSIZE 1024

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


  struct cabecera {
    char * resultado;
    char * server;
    char * content_type;
    char * content_length;
    char * cuerpo;
  };

  enum mensajesHTTP {
    ok = 200,
    bad_request = 400,
    not_found = 404,
    forbiden = 500,
    not_implemented = 501
    };

    struct data {
      enum mensajesHTTP status;
      char * buffer;
      int size;
    };


  int readLine(int s, char *line, int *result_size);
  int writeLine(int s, char *line, int total_size);
  char * substr(char * cadena, int comienzo, int longitud);
  int commandParser(char * command);
  char * substrUntilChar(char * string, char c);
  int leerDebug (char * path);
  void * openAndReadFile(char * uri, char * root);
  void * HTTPGET (char * command);
  void * commandExecutor(int value, char * command);
  int serve(int s);
  void enviarHeader(struct cabecera header, char * buffer, enum mensajesHTTP status);
  void configurarHeader(struct cabecera * header, struct data * dataLectura);
  void * leer (char * path);


#endif
