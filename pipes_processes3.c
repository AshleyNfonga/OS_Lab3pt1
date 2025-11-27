#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
if (argc != 2) {
fprintf(stderr, "Usage: %s <grep-argument>\n", argv[0]);
exit(1);
}


int pipe1[2]; // pipe: cat -> grep
int pipe2[2]; // pipe: grep -> sort

if (pipe(pipe1) == -1) { perror("pipe1"); exit(1); }
if (pipe(pipe2) == -1) { perror("pipe2"); exit(1); }

// First child: executes grep
pid_t pid1 = fork();
if (pid1 < 0) { perror("fork pid1"); exit(1); }

if (pid1 == 0) {
    // Child process: grep
    if (close(pipe1[1]) == -1) { perror("close pipe1 write"); exit(1); }
    if (dup2(pipe1[0], STDIN_FILENO) == -1) { perror("dup2 pipe1 read"); exit(1); }
    if (close(pipe1[0]) == -1) { perror("close pipe1 read"); exit(1); }

    if (close(pipe2[0]) == -1) { perror("close pipe2 read"); exit(1); }
    if (dup2(pipe2[1], STDOUT_FILENO) == -1) { perror("dup2 pipe2 write"); exit(1); }
    if (close(pipe2[1]) == -1) { perror("close pipe2 write"); exit(1); }

    execlp("grep", "grep", argv[1], NULL);
    perror("execlp grep");
    exit(1);
}

// Second child: executes sort
pid_t pid2 = fork();
if (pid2 < 0) { perror("fork pid2"); exit(1); }

if (pid2 == 0) {
    // Child process: sort
    if (close(pipe1[0]) == -1 || close(pipe1[1]) == -1) { perror("close pipe1"); exit(1); }
    if (close(pipe2[1]) == -1) { perror("close pipe2 write"); exit(1); }
    if (dup2(pipe2[0], STDIN_FILENO) == -1) { perror("dup2 pipe2 read"); exit(1); }
    if (close(pipe2[0]) == -1) { perror("close pipe2 read"); exit(1); }

    execlp("sort", "sort", NULL);
    perror("execlp sort");
    exit(1);
}

// Parent process: executes cat
if (close(pipe1[0]) == -1) { perror("close pipe1 read"); exit(1); }
if (close(pipe2[0]) == -1 || close(pipe2[1]) == -1) { perror("close pipe2"); exit(1); }
if (dup2(pipe1[1], STDOUT_FILENO) == -1) { perror("dup2 pipe1 write"); exit(1); }
if (close(pipe1[1]) == -1) { perror("close pipe1 write"); exit(1); }

execlp("cat", "cat", "scores", NULL);
perror("execlp cat");
exit(1);


}
