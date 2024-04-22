#include "includes.h"
#include "mysystem.h"

int pipes(char *pipeline, int taskid, char *output_folder) {
    char *commands[MAX_COMMANDS];
    int N;

    // Parse the pipeline command into individual commands
    char *token = strtok(pipeline, "|");
    while (token != NULL && N < MAX_COMMANDS) {
        commands[N++] = token;
        token = strtok(NULL, "|");
    }

    if (N < 2) {
        fprintf(stderr, "Error: Pipeline must contain at least two commands\n");
        return -1;
    }

    // Create pipes for communication between commands
    int pipes[N- 1][2];
    for (int i = 0; i < N- 1; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            return -1;
        }
    }

    // Fork processes for each command
    pid_t pids[N];
    for (int i = 0; i < N; i++) {
        pids[i] = fork();
        if (pids[i] == -1) {
            perror("fork");
            return -1;
        }
        
        if (pids[i] == 0) { // Child process
            if (i == 0) { // First command
                // Redirect stdout to the write end of the first pipe
                dup2(pipes[0][1], STDOUT_FILENO);
                close(pipes[0][0]); // Close the read end of the first pipe
            } else if (i == N - 1) { // Last command
                // Redirect stdin to the read end of the last pipe
                dup2(pipes[N - 2][0], STDIN_FILENO);
                close(pipes[N - 2][1]); // Close the write end of the last pipe
            } else { // Intermediate commands
                // Redirect stdin to the read end of the previous pipe
                dup2(pipes[i - 1][0], STDIN_FILENO);
                close(pipes[i - 1][1]); // Close the write end of the previous pipe
                // Redirect stdout to the write end of the current pipe
                dup2(pipes[i][1], STDOUT_FILENO);
                close(pipes[i][0]); // Close the read end of the current pipe
            }

            // Execute the command
            execlp(commands[i], commands[i], NULL);
            perror("execlp");
            exit(EXIT_FAILURE);
        }
    }

    // Close all pipe ends in the parent process
    for (int i = 0; i < N - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all child processes to finish
    for (int i = 0; i < N; i++) {
        int status;
        waitpid(pids[i], &status, 0);
    }

    return 0;
}