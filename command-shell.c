#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

/* Exit the program */
void exitProgram(int status) {
  exit(status);
}

/* Prints the working directory*/
void pwd() {
  char cwd[1024];
  //chdir("/path/to/change/directory/to");
  getcwd(cwd, sizeof(cwd));
  printf("Current working direcory: %s\n", cwd);
}

/* Change directory */
void cd(char* directory) {
  chdir(directory);
}

/* Display line of text/string that are passed as an argument  */
void echo(char* arguments[], int argumentCount) {

  int i;
  // loop through arguments
  for (i = 1; i < argumentCount; i++) {
    char* a;

    // check if '$' is first char
    for (a = arguments[i]; *a != '\0'; a++) {
      char env = '$';

      // '$' is first char
      if (env == *a) { // according to ASCII codes
        char envToGet[100];
        strncpy(envToGet, arguments[i]+1, strlen(arguments[i]));
        printf("%s \n", getenv(envToGet));
        break;
      } else {
        // '$' is not first char
        printf("%s ", arguments[i]);
        break;
      }
    }
  }
  printf("\n");
}

/* Prints the current values of the environment variables */
void env(char **environ) {
  char **env = environ;
  //for (char **env = environ; *env != 0; env++){
  for (**env; *env != 0; env++){
    char *thisEnv = *env;
    printf("%s\n", thisEnv);    
  }
}

/* Update or add a variable in the environment of the calling process. */
// Usage: setenv a b ---> Will create variable a = b.
void setenvir(char* var, char *envval) {
  setenv(var, envval, 1);
}
