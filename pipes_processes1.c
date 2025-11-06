#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 100

int main() {
    int fd[2];                         // fd[0] = read end, fd[1] = write end
    pid_t pid;
    char writeMsg[BUFFER_SIZE];
    char readMsg[BUFFER_SIZE];

    // Create the pipe before forking
    if (pipe(fd) == -1) {
        perror("Pipe creation failed");
        exit(1);
    }

    pid = fork();                      // Create a new process

    if (pid < 0) {
        perror("Fork failed");
        exit(1);
    }

    if (pid == 0) {
        /*************** CHILD PROCESS ***************/
        // Child only reads from the pipe, so close the write end
        close(fd[1]);

        // Read the message sent by the parent
        read(fd[0], readMsg, sizeof(readMsg));
        printf("Child received: %s\n", readMsg);

        // Close the read end after use
        close(fd[0]);
    }
    else {
        /*************** PARENT PROCESS ***************/
        // Parent only writes to the pipe, so close the read end
        close(fd[0]);

        printf("Input : ");
        fgets(writeMsg, sizeof(writeMsg), stdin);

        // Remove newline added by fgets
        writeMsg[strcspn(writeMsg, "\n")] = '\0';

        // Send message to the child
        write(fd[1], writeMsg, strlen(writeMsg) + 1);

        // Close the write end after sending
        close(fd[1]);
    }

    return 0;
}
