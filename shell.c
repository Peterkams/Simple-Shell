#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include <time.h>

#define CWD_MAX_LENGTH 50
#define MAX_COMMAND_LINE_LEN 1024
#define MAX_COMMAND_LINE_ARGS 128

char prompt[] = "> ";
char delimiters[] = " \t\r\n";
extern char **environ;

int argumentCount = 0;

/* Implement Built In Commands */
int main(int argc, char **argv);
void exitProgram();
void pwd();
void cd(char* directory);
void echo(char* arguments[], int argumentCount);
void env(char **environ);
void setenvir(const char* var, const char* envval);
/* --------------------- */

int printPrompt() {
  // Get current working directory
  char cwd[CWD_MAX_LENGTH];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    // Check if error
    perror("getcwd() error");
    return 1;
  }
  
  // Print prompt
  printf("%s%s", cwd, prompt);
  fflush(stdout);
  return 0;
}

void tokenizeInput(char command_line[MAX_COMMAND_LINE_LEN], char* arguments[]) {
  int index = 0;
  char* token = strtok(command_line, delimiters);

  // loop through the string to extract all other tokens
  while ( token != NULL ) {
    arguments[index] = token;
    index++;
    token = strtok(NULL, delimiters);
    argumentCount++;
  }
}

bool inBuiltCommand(char* command) {
  // String compare returns 0 if equal. 
  return (!strcmp(command, "cd") || !strcmp(command, "pwd") || !strcmp(command, "echo") || !strcmp(command, "exit") || !strcmp(command, "env") || !strcmp(command, "setenv"));
}

void executeCommand(char* arguments[]) {
  // make null terminated
  arguments[argumentCount] = NULL;
 
  // Forking a child 
  clock_t t = clock(); 
  pid_t pid = fork();  

  if (pid == -1) { 
    printf("\nFailed forking child.."); 
    return; 
  } else if (pid == 0) { 
    if ( execvp(arguments[0], arguments) < 0) { 
      perror("Error printed by perror");
    } 
    exit(0); 
  } else { 
    // waiting for child to terminate 
    bool terminated = false;
    while (((double) clock() - t)/CLOCKS_PER_SEC < 10) {
      if (waitpid(pid, NULL, WNOHANG)) {
        terminated = true;
        break;
      }
    }
    if (!terminated) {
      kill(pid, SIGKILL);
    }

    return; 
  } 
}




void processCommand(char* arguments[]) {
  char* cmd = arguments[0];

  // Check background process
  int i;
  for (i = 0; i < argumentCount; i++) {
    if (!strcmp(arguments[i], "&")) {
      arguments[i] = NULL;
      // fork child
      int pid = fork();

      if (pid < 0) {
        printf("\n Error ");
        exit(1);
      } else if (pid == 0) {
        executeCommand(arguments);
        exit(0);
      } else {
        return;
      }
      
    }
  }
  // Check pipe

  // Check if we have this command
  if (inBuiltCommand(cmd)) {

    // String compare returns 0 if equal. 
    if (!strcmp(cmd, "cd")) {
      // error check
      if (argumentCount < 2) {
        printf("error - Must include directory for cd.\n");
        return;
      }
      cd(arguments[1]);
    }
    else if (!strcmp(cmd, "pwd")) pwd();
    else if (!strcmp(cmd, "exit")) exitProgram();
    else if (!strcmp(cmd, "echo")) echo(arguments, argumentCount);
    else if (!strcmp(cmd, "env")) env(environ);
    else if (!strcmp(cmd, "setenv")) {
      if (argumentCount < 3) {
        printf("error - Must include variable and value.\n");
        return;
      }
      setenvir(arguments[1], arguments[2]);
    }
  } else {
    int pid = fork();
    if (pid < 0) {
      printf("\n Error ");
      exit(1);
    } else if (pid == 0) {
      executeCommand(arguments);
      exit(0);
    } else {
      wait(NULL);
    }
    //executeCommand(arguments);
  }
}

int shell() {
  // Stores the string typed into the command line.
  char command_line[MAX_COMMAND_LINE_LEN];
  char cmd_bak[MAX_COMMAND_LINE_LEN];
  
  // Stores the tokenized command line input.
  char *arguments[MAX_COMMAND_LINE_ARGS];

  while (true) {
    do { 
      // Print the shell prompt.
      int error = printPrompt();
      if (error) exit(1); 

      // Read input from stdin and store it in command_line. If there's an
      // error, exit immediately. (If you want to learn more about this line,
      // you can Google "man fgets")
        
      if ((fgets(command_line, MAX_COMMAND_LINE_LEN, stdin) == NULL) && ferror(stdin)) {
        fprintf(stderr, "fgets error");
        exit(0);
      }

      // tokenize input placed in arguments string array
      argumentCount = 0;
      tokenizeInput(command_line, arguments);

      // process command
      processCommand(arguments);

      
    } while (command_line[0] == 0x0A);  // while just ENTER pressed
    
    
    // If the user input was EOF (ctrl+d), exit the shell.
    if ( feof(stdin) ) {
      printf("\n");
      fflush(stdout);
      fflush(stderr);
      return 0;
    }

  
  
    // Hints (put these into Google):
    // man fork
    // man execvp
    // man wait
    // man strtok
    // man environ
    // man signals
    
    // Extra Credit
    // man dup2
    // man open
    // man pipes
  }
}

/* Signal handler (CTRL+C) */
void handle_sigint(int sig) { 
  printf("\n");
  shell();
} 

int main(int argc, char **argv) {
  
  // Register signals
  signal(SIGINT, handle_sigint); 
  
  // Listen for commands
  return shell();

}
