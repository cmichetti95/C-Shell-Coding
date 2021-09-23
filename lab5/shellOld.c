/* Connor Michetti */

/* To compile, simply type the 'make' command. From there, type 'mysh' to run the program. Next, enter any number of
 * commands. When you are finished, hit CTRL + C to get a list of the last 10 commands entered. From this list, you 
 * can type r <letter> to enter a command starting with <letter> (if none are found, just runs the most recent command).
 * Additionally, you can type r to run the most recent command.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <wait.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
#define BUFFER_SIZE 80

int currentCounter = 0;
int currentIndex = 0;
int sig = 0;
static char command_arr[10][MAX_LINE];
char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
struct sigaction handler;
char file[] = "michetti.5.history";
int fd;

/**
 * setup() reads in the next command line, separating it into distinct tokens
 * using whitespace as delimiters. setup() sets the args parameter as a 
 * null-terminated string.
 */

void addCommand()
{
	currentCounter++;
	currentIndex++;
	if(currentIndex > 9)
	{
	    currentIndex = 0;
	}
	
	strcpy(command_arr[currentIndex], inputBuffer);

}

void readFromFile()
{
	//NOTE** Couldn't get functioning. Based off an example given on website listed in README (not copied, just listing source to avoid any potential COAM).
	char *token;
	
	//open or create file if it doesn't exist
	fd = open(file, O_RDONLY | O_CREAT, 0644);
	if(fd < 0)
	{
		fprintf(stderr, "Could not open file.\n");
		exit(-1);
	}

	//read and place into the buffer
	read(fd, inputBuffer, 10);

	//attempting to break at provided token
	token = strtok(inputBuffer, "\n\0");

	while(token != NULL)
	{
	    //attempting first to copy the token into the array at index
	    strcpy(command_arr[currentIndex], token);

	    //next grabbing length of current string
	    int length = strlen(command_arr[currentIndex]);

	    //placing \n back at the end of the string
	    command_arr[currentIndex][length] = '\n';

	    //continuing through (following example mentioned above)
	    token = strtok(NULL, "/n/0");

	    //increment index so we overwrite if needed.
	    currentIndex++;
	    if(currentIndex > 9) currentIndex = 0;

	    //get corrent total number of commands
	    currentCounter++;
	}

	//close
	close(fd);
}


void setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct,     /* index of where to place the next parameter into args[] */
	j;
    
    ct = 0;

    if(!sig)
    {
	length = read(STDIN_FILENO, inputBuffer, BUFFER_SIZE);
	printf("\n");
	fflush(0);
    }
    else
    {
	length = strlen(inputBuffer);
	sig = 0;
    }

    start = -1;
    if (length == 0)
	fd = open(file, O_WRONLY, 0644);
	for(j = 0; j <= currentCounter; j++)
	{
	    write(fd, command_arr[j], strlen(command_arr[j]));
	}
	close(fd);
        exit(0);            /* ^d was entered, end of user command stream */
    if (length < 0){
        perror("error reading the command");
	exit(-1);           /* terminate with error code of -1 */
    }

    inputBuffer[length] = '\0';
    addCommand();
    
    /* examine every character in the inputBuffer */
    for (i = 0; i < length; i++) { 
        switch (inputBuffer[i]){
        case ' ':
        case '\t' :               /* argument separators */
            if(start != -1){
                args[ct] = &inputBuffer[start];    /* set up pointer */
                ct++;
            }
            inputBuffer[i] = '\0'; /* add a null char; make a C string */
            start = -1;
            break;
            
        case '\n':                 /* should be the final char examined */
            if (start != -1){
                args[ct] = &inputBuffer[start];     
                ct++;
            }
            inputBuffer[i] = '\0';
            args[ct] = NULL; /* no more arguments to this command */
            break;

        case '&':
            *background = 1;
            inputBuffer[i] = '\0';
            break;
            
        default :             /* some other character */
            if (start == -1)
                start = i;
	} 
    }    
    args[ct] = NULL; /* just in case the input line was > 80 */
} 


/* A simple function that prints out the args of the "commands" array */
void printList()
{
    int i, counterCopy, indexCopy;
  
	if(currentCounter < 10)
	{
	    indexCopy = 0;
	    for(i = 1; i <= currentCounter; i++)
	    {
		printf("%d. ", i);
		printf("%s", command_arr[indexCopy + 1]);

		indexCopy++;
	    }
	}
	else
	{
	    counterCopy = currentCounter - 9;
	    indexCopy = currentIndex + 1;
	    if(indexCopy > 9)
	    {
		indexCopy = 0;
	    }

	    for(i = 0; i < 10; i++)
	    {
		printf("%d. ", counterCopy);
		printf("%s", command_arr[indexCopy]);
		indexCopy++;
		counterCopy++;
		if(indexCopy > 9)
		{
		    indexCopy = 0;
		}
	    }
	} 
}

int getCommand()
{
    int wait = 0;
    int indexOfCom = 9;
    int letterFound = 0;
    int indexCopy, i, length;
    char letter;
    
    
    while(wait == 0)
    {
	printf("Enter 'r' to run the most recent command. Enter 'r' <letter> to enter the first command starting with <letter>: ");
	fflush(0);
	length = read(STDIN_FILENO, inputBuffer, BUFFER_SIZE);

	if(length ==0) exit(0);

	if(length < 0) exit(-1);

	inputBuffer[length -1] = '\0';
	length = strlen(inputBuffer);

	if(length < 1)
	{
	    printf("Not a valid entry, try again");
	    fflush(0);
	}
	//check user input for correctness
	else
	{
	    if(length == 1 && inputBuffer[0] != 'r')
	    {
		printf("Not a valid entry, try again");
	    }
	    else if(length == 3 && inputBuffer[0] != 'r')
	    {
		printf("Not a valid entry, try again");
	    }
	    //user just enters 'r'
	    else if(length == 1 && inputBuffer[0] == 'r')
	    {
		return currentIndex;
	    }
	    //user enters 'r <letter>'
	    else if(length == 3 && inputBuffer[0] == 'r')
	    {

		letter = inputBuffer[2];
		//less than 10 commands
		if(currentCounter < 10)
		{
		    for(i = 0; i < currentCounter; i++)
		    {
			if(command_arr[i][0] == letter)
			{
			    indexOfCom = i;
			    return indexOfCom;
			}
		    }
		    return currentIndex;
		}
		//10 or more commands
		else
		{

		

		    indexCopy = currentIndex + 1;
		    if(indexCopy > 9) indexCopy == 0;

		    for(i = 0; i < 10; i++)
		    {
		        if(command_arr[indexCopy][0] == letter)
		        {
			    indexOfCom = indexCopy;
			    return indexOfCom;
		        }
		    
		        indexCopy++;
		        if(indexCopy > 9) indexCopy == 0;
		    }
		    return indexOfCom;
		}
	    }
	}
    }
}

void getUserComm(int cmd_ind)
{
	//Partially the same as setup in "main".
	int bg, temp;
	bg = 0;
	char *arg[BUFFER_SIZE/2 + 1];

	//grab the command at the given index (this is the user command).
	strcpy(inputBuffer, command_arr[cmd_ind]);
	sig = 1;

	//use setup
	setup(inputBuffer, arg, &bg);
	pid_t pid = fork();

	//executes the given command (also checks for if there was a & at the end to indicate background process.
	if(pid == 0)
	{
	    printf("COMMAND->%s\n", inputBuffer);
	    fflush(0);
	    if(execvp(inputBuffer, arg) < 0)
	    {
		fprintf(stderr, "Fork failed.\n");
		exit(-1);
	    }
	}
	else if(pid < 0)
	{
	    fprintf(stderr, "Fork failed.\n");
	    exit(-1);
	}
	else
	{
	    if(bg == 0)
	    {
		waitpid(pid, &temp, 0);
		printf("Child completed.\n");
	    }
	}
	printf("COMMAND->");
	fflush(0);
	
}


void handle_SIGINT()
{
    int index;

    printf("\n");
    fflush(0);
    printList();

    //grabs the index of "r x" command and executes.
    index = getCommand();
    getUserComm(index);

}

int main(int argc, char *argv[])
{

    int background, check;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/2+1];/* command line (of 80) has max of 40 arguments */

    int pid_t;	/* variable to hold fork val */

    //setup the signal.
    
    handler.sa_handler = handle_SIGINT;
    handler.sa_flags = SA_RESTART;
    sigaction(SIGINT, &handler, NULL);

    //read data from the file
    readFromFile();
    

    while (1){            /* Program terminates normally inside setup */
	background = 0;
	printf("COMMAND->");
        fflush(0);
        setup(inputBuffer, args, &background);       /* get next command */

	/* the steps are:
	 (1) fork a child process using fork()
	 (2) the child process will invoke execvp()
	 (3) if background == 0, the parent will wait, 
		otherwise returns to the setup() function. */

	pid_t = fork();		/* For the process and check to make sure it was succesful */
	if(pid_t < 0)
	{
	    fprintf(stderr, "Fork failed\n");
	    exit(-1);
	}
	else if (pid_t == 0)	/* if it works, call execvp */
	{
	    check = execvp(inputBuffer, args);
	    if(check < 0)
	    {
		fprintf(stderr, "Failed.");
		exit(-1);
	    }
	    
	}
	if (background == 0)	/* wait while child process completes */
	{
	    wait(NULL);
	    printf("Child completed\n");
	}
	    
    }
return 0;
}
