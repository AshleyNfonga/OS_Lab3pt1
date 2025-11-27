#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 256

int main() {
int pipe1[2]; // P1 -> P2
int pipe2[2]; // P2 -> P1
char buffer[BUFFER_SIZE];


// Create two pipes
if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
    perror("pipe");
    exit(1);
}

pid_t pid = fork();
if (pid < 0) {
    perror("fork");
    exit(1);
}

if (pid == 0) {
    // ---------------------------
    // P2: Child process
    // ---------------------------
    close(pipe1[1]); // close write end of pipe1
    close(pipe2[0]); // close read end of pipe2

    // Read string from P1
    if (read(pipe1[0], buffer, BUFFER_SIZE) <= 0) {
        perror("read from pipe1");
        exit(1);
    }
    close(pipe1[0]);

    // Append "howard.edu" and print intermediate output
    strcat(buffer, "howard.edu");
    printf("Output : %s\n", buffer);

    // Prompt for second input
    char input2[BUFFER_SIZE];
    printf("Input : ");
    if (!fgets(input2, BUFFER_SIZE, stdin)) {
        perror("fgets");
        exit(1);
    }
    input2[strcspn(input2, "\n")] = 0; // remove newline

    // Append second input and send back to P1
    strcat(buffer, input2);
    if (write(pipe2[1], buffer, strlen(buffer) + 1) == -1) {
        perror("write to pipe2");
        exit(1);
    }
    close(pipe2[1]);
    exit(0);
} else {
    // ---------------------------
    // P1: Parent process
    // ---------------------------
    close(pipe1[0]); // close read end of pipe1
    close(pipe2[1]); // close write end of pipe2

    // Prompt for first input
    char input1[BUFFER_SIZE];
    printf("Input : ");
    if (!fgets(input1, BUFFER_SIZE, stdin)) {
        perror("fgets");
        exit(1);
    }
    input1[strcspn(input1, "\n")] = 0; // remove newline

    // Send input to child
    if (write(pipe1[1], input1, strlen(input1) + 1) == -1) {
        perror("write to pipe1");
        exit(1);
    }
    close(pipe1[1]);

    // Read modified string back from P2
    if (read(pipe2[0], buffer, BUFFER_SIZE) <= 0) {
        perror("read from pipe2");
        exit(1);
    }
    close(pipe2[0]);

    // Append "gobison.org" and print final output
    strcat(buffer, "gobison.org");
    printf("Output : %s\n", buffer);

    // Wait for child to finish
    wait(NULL);
}

return 0;


}
