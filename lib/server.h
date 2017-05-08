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
      char * extension;
      char * buffer;
      int size;
    };

    char *mapaMimeTypes[85]={
    	"au","audio/basic",
    	"avi","video/avi",
    	"bmp","image/bmp",
    	"bz2","application/x-bzip2",
    	"css","text/css",
    	"dtd","application/xml-dtd",
    	"doc","application/msword",
    	"exe","application/octet-stream",
    	"gif","image/gif",
    	"gz","application/x-gzip",
    	"hqx","application/mac-binhex40",
     	"html","text/html",
    	"jar","application/java-archive",
    	"jpg","image/jpeg",
      "jpeg","image/jpeg",
    	"js","application/x-javascript",
    	"midi","audio/x-midi",
    	"mp3","audio/mpeg",
    	"mpeg" "video/mpeg",
    	"ogg","audio/vorbis",
    	"pdf","application/pdf",
    	"pl","application/x-perl",
    	"png","image/png",
    	"ppt","application/vnd.ms-powerpoint",
    	"ps","application/postscript",
    	"qt","video/quicktime",
    	"ra","audio/x-pn-realaudio",
    	"ram","audio/x-pn-realaudio",
    	"rdf","application/rdf",
    	"rtf","application/rtf",
    	"sgml","text/sgml",
    	"sit","application/x-stuffit",
    	"svg","image/svg+xml",
    	"swf","application/x-shockwave-flash",
    	"tar.gz","application/x-tar",
    	"tgz","application/x-tar",
    	"tiff","image/tiff",
    	"tsv","text/tab-separated-values",
    	"txt","text/plain",
    	"wav","audio/wav",
    	"xls","application/vnd.ms-excel",
    	"xml","application/xml",
    	"zip","application/zip"
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
  void configurarHeader(struct cabecera * header, struct data * dataLectura);
  void * leer (char * path);


#endif
