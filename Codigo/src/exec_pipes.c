#include "includes.h"
#include "exec_pipes.h"

long exec_pipes(const char *arg, int taskid, char *output_folder, struct tempo comeco)
{
    struct tempo fim;
    long segundos, microseg;

    gettimeofday(&comeco, NULL);
    char *command = strdup(arg);
    char **commands = malloc(sizeof(char *));
    int i = 0;

    // split the input string into commands
    char *string = strtok(command, "|");
    while (string != NULL)
    {
        commands[i] = string;
        string = strtok(NULL, "|");
        i++;
        // resize the array if necessary
        commands = realloc(commands, (i + 1) * sizeof(char *));
    }
    commands[i] = NULL;

    pid_t pid[i];
    int fd[i][2];

    // execute each command
    for (int j = 0; j < i; j++)
    {
        if (commands[j + 1] != NULL)
        {
            // if not the last command, create a new pipe
            pipe(fd[j]);
        }

        if ((pid[j] = fork()) == 0)
        {
            // child process
            if (j != 0)
            {
                // if not the first command, redirect standard input from the read end of the previous pipe
                dup2(fd[j - 1][0], 0);
                close(fd[j - 1][0]);
            }
            if (commands[j + 1] != NULL)
            {
                // if not the last command, redirect standard output to the write end of the current pipe
                dup2(fd[j][1], 1);
                close(fd[j][1]);
            }
            else if (j == i - 1)
            {
                // last command and no more commands to be executed, redirect output to a file
                char output_file[256];
                sprintf(output_file, "../%s/output_task_id_%d", output_folder, taskid);
                int fd_out = open(output_file, O_WRONLY | O_CREAT, 0666);

                if (fd_out == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                dup2(fd_out, 1);
                close(fd_out);
            }

            // split the command into arguments and execute it
            char **exec_args = malloc(sizeof(char *));
            int k = 0;
            int current_size = 1; // keep track of the current size of the array
            string = strtok(commands[j], " ");
            while (string != NULL)
            {
                exec_args[k] = string;
                string = strtok(NULL, " ");
                k++;
                // resize the array if necessary
                if (k >= current_size)
                {
                    current_size *= 2; // double the size of the array
                    exec_args = realloc(exec_args, current_size * sizeof(char *));
                }
            }
            exec_args[k] = NULL;

            int res = execvp(exec_args[0], exec_args);
            if (res == -1)
                perror("No such file");
            free(exec_args);
            exit(res);
        }
        else
        {
            // parent process
            if (j != 0)
            {
                close(fd[j - 1][0]);
            }
            if (commands[j + 1] != NULL)
            {
                close(fd[j][1]);
            }
        }
    }
    for (int j = 0; j < i; j++)
    {
        waitpid(pid[j], NULL, 0);
    }
    free(commands);
    gettimeofday(&fim, NULL);
    segundos = fim.segundos - comeco.segundos;
    microseg = fim.microseg - comeco.microseg;
    long tempoTotal = (segundos * 1000) + (microseg / 1000);
    return tempoTotal;
}