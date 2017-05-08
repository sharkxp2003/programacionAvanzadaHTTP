#include "../lib/execphp.h"

int main() {
    int message_fd[2][2];
    int i;

    pipe(message_fd[READ]);
    pipe(message_fd[WRITE]);

    if(!fork()) {
        close(message_fd[READ][READ]);
        close(message_fd[WRITE][WRITE]);

        dup2(message_fd[READ][WRITE], 1);
        dup2(message_fd[WRITE][READ], 0);

		putenv("REQUEST_METHOD=GET");
		putenv("REDIRECT_STATUS=True");
		putenv("QUERY_STRING=hola=a&mundo=b");
		putenv("SCRIPT_FILENAME=test.php");

		execlp("php-cgi", "php-cgi", "test.php", 0);
    }
    close(message_fd[READ][WRITE]);
    close(message_fd[WRITE][READ]);

    FILE *fin = fdopen(message_fd[READ][READ], "r");
    FILE *fout = fdopen(message_fd[WRITE][WRITE], "w");

	char buffer[32];
	int size = 0;
	while(1) {
		if(feof(fin)) break;
		size = fread(buffer, 32, 1, fin);
		fwrite(buffer, 32, 1, stdout);
	}



}
