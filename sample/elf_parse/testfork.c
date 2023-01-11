#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include <string.h>

#define PIPE_READ 0
#define PIPE_WRITE 1

int createChild(const char* szCommand, char* const aArguments[], char* const aEnvironment[], const char* szMessage) {
  int aStdinPipe[2];
  int aStdoutPipe[2];
  int nChild;
  char nChar;
  int nResult;

  if (pipe(aStdinPipe) < 0) {
    perror("allocating pipe for child input redirect");
    return -1;
  }
  if (pipe(aStdoutPipe) < 0) {
    close(aStdinPipe[PIPE_READ]);
    close(aStdinPipe[PIPE_WRITE]);
    perror("allocating pipe for child output redirect");
    return -1;
  }

  nChild = fork();
  if (0 == nChild) {
    // child continues here

    // redirect stdin
    if (dup2(aStdinPipe[PIPE_READ], STDIN_FILENO) == -1) {
      exit(errno);
    }

    // redirect stdout
    if (dup2(aStdoutPipe[PIPE_WRITE], STDOUT_FILENO) == -1) {
      exit(errno);
    }

    // redirect stderr
    if (dup2(aStdoutPipe[PIPE_WRITE], STDERR_FILENO) == -1) {
      exit(errno);
    }

    // all these are for use by parent only
    close(aStdinPipe[PIPE_READ]);
    close(aStdinPipe[PIPE_WRITE]);
    close(aStdoutPipe[PIPE_READ]);
    close(aStdoutPipe[PIPE_WRITE]); 

    // run child process image
    // replace this with any exec* function find easier to use ("man exec")
    nResult = execve(szCommand, aArguments, aEnvironment);

    // if we get here at all, an error occurred, but we are in the child
    // process, so just exit
    exit(nResult);
  } else if (nChild > 0) {
    // parent continues here

    // close unused file descriptors, these are for child only
    close(aStdinPipe[PIPE_READ]);
    close(aStdoutPipe[PIPE_WRITE]); 

    while (1) {
      int nRead = 0;

      // Include error check here
      if (NULL != szMessage) {
        printf("\nsending hello message...\n");
        write(aStdinPipe[PIPE_WRITE], szMessage, strlen(szMessage));
      }

      // Just a char by char read here, you can change it accordingly
      while (read(aStdoutPipe[PIPE_READ], &nChar, 1) == 1) {
        nRead++;
        write(STDOUT_FILENO, &nChar, 1);

        if (nRead >= strlen(szMessage)) {
          // read the entire echo of the message, send another one
          break;
        }
      }
      sleep(1);
    }

    // done with these in this example program, you would normally keep these
    // open of course as long as you want to talk to the child
    close(aStdinPipe[PIPE_WRITE]);
    close(aStdoutPipe[PIPE_READ]);
  } else {
    // failed to create child
    close(aStdinPipe[PIPE_READ]);
    close(aStdinPipe[PIPE_WRITE]);
    close(aStdoutPipe[PIPE_READ]);
    close(aStdoutPipe[PIPE_WRITE]);
  }
  return nChild;
}

int main(int argc, char* argv[]) {
  char* const* aEnv = { NULL };
  return createChild("/usr/bin/tee", argv, aEnv, "hello world");
}
