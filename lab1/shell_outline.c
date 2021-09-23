#include <stdio.h>

#include <unistd.h>

#define MAX_LINE 80

/* setup() reads in the next command line string stored in inputBuffer,
separating it into distinct tokens using whitespace as delimiters.
setup() modifies the args parameter so that it holds pointers to the
null-terminated strings that are the tokens in the most recent user
command line as well as a NULL pointer, indicating the end of the
argument list, which comes after the string pointers that have been
assigned to args. */

void setup(char inputBuffer[], char *args[], int *background) {
    /** full code available in the file shell.c */
}

int main(void) {
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;       /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE+1]; /* command line arguments */

    while (1) {
        background = 0;
        printf(" COMMAND->\n");
        setup(inputBuffer,args,&background); /* get next command */

    /* The steps are:
     (1) fork a child process using fork()
     (2) the child process will invoke execvp()
     (3) if background == 0, the parent will wait, 
        otherwise returns to the setup() function. */
    }
}
