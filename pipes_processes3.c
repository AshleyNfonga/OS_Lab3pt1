#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUF_SIZE 512

int main() {
    int to_child[2];   // Parent -> Child pipe
    int to_parent[2];  // Child -> Parent pipe
    pid_t pid;
    char input1[BUF_SIZE];
    char input2[BUF_SIZE];
    char child_out[BUF_SIZE];
    char final_out[BUF_SIZE];

    // Create both pipes
    if (pipe(to_child) == -1) { perror("pipe to_child failed"); exit(1); }
    if (pipe(to_parent) == -1) { perror("pipe to_parent failed"); exit(1); }

    pid = fork();
    if (pid < 0) { perror("fork failed"); exit(1); }

    if (pid == 0) {
        /************** CHILD PROCESS **************/
        close(to_child[1]);  // Close write end of parent->child
        close(to_parent[0]); // Close read end of child->parent

        // Read string from parent
        read(to_child[0], child_out, BUF_SIZE);

        // Concatenate "howard.edu"
        strncat(child_out, "howard.edu", BUF_SIZE - strlen(child_out) - 1);

        // Print first child output as in assignment
        printf("Other string is: howard.edu\n");
        printf("Output : %s\n", child_out);
        fflush(stdout);

        // Prompt for second input
        printf("Input : ");
        fflush(stdout);
        if (fgets(input2, BUF_SIZE, stdin) != NULL) {
            input2[strcspn(input2, "\n")] = '\0'; // Remove newline
        } else {
            input2[0] = '\0';
        }

        // Append second input to child_out
        strncat(child_out, input2, BUF_SIZE - strlen(child_out) - 1);

        // Send the concatenated string back to parent
        write(to_parent[1], child_out, strlen(child_out) + 1);

        // Close pipes
        close(to_child[0]);
        close(to_parent[1]);
        exit(0);
    } else {
        /************** PARENT PROCESS **************/
        close(to_child[0]);  // Close read end of parent->child
        close(to_parent[1]); // Close write end of child->parent

        // Read first input from user
        printf("Input : ");
        fflush(stdout);
        if (fgets(input1, BUF_SIZE, stdin) != NULL) {
            input1[strcspn(input1, "\n")] = '\0'; // Remove newline
        } else {
            input1[0] = '\0';
        }

        // Send input to child
        write(to_child[1], input1, strlen(input1) + 1);

        // Read concatenated string from child
        read(to_parent[0], final_out, BUF_SIZE);

        // Parent appends "gobison.org" and prints final output
        strncat(final_out, "gobison.org", BUF_SIZE - strlen(final_out) - 1);
        printf("Output : %s\n", final_out);

        // Close pipes
        close(to_child[1]);
        close(to_parent[0]);

        wait(NULL); // Wait for child to finish
    }

    return 0;
}
