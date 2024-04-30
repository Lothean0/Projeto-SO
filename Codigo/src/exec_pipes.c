#include "includes.h"
#include "exec_pipes.h"

long exec_pipes(const char *arg, int taskid, char *output_folder)
{
    struct tempo comeco, fim;
    long segundos, microseg;

    gettimeofday(&comeco, NULL);
    int in_fd = 0; // input file descriptor
    char *command = strdup(arg);
    char *commands[10];
    int i = 0;

    // split the input string into commands
    char *string = strtok(command, "|");
    while (string != NULL)
    {
        commands[i] = string;
        string = strtok(NULL, "|");
        i++;
    }
    commands[i] = NULL;

    pid_t pid[i];
    // execute each command
    for (int j = 0; j < i; j++)
    {
        int fd[2];
        pipe(fd);

        if ((pid[j]=fork()) == 0)
        {
            // child process
            if (in_fd != 0)
            {
                // if not the first command, redirect standard input to the previous pipe
                dup2(in_fd, 0);
                close(in_fd);
            }
            if (commands[j + 1] != NULL)
            {
                // if not the last command, redirect standard output to the current pipe
                dup2(fd[1], 1);
                close(fd[1]);
            }
            else if(j==i-1)
            {
                // last command and no more commands to be executed, redirect output to a file
                char output_file[256];
                sprintf(output_file, "../%s/output_task_id_%d", output_folder, taskid);
                int fd = open(output_file, O_WRONLY | O_CREAT, 0666);
                //acrescentei
                if (fd == -1)
                {
                    perror("open");
                    exit(EXIT_FAILURE);
                }
                dup2(fd, 1);
                //acrescentei
                if (dup2(fd, 1) == -1)
                {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
                close(fd);
            }
            close(fd[0]);

            // split the command into arguments and execute it
            char *exec_args[10];
            int k = 0;
            string = strtok(commands[j], " ");
            while (string != NULL)
            {
                exec_args[k] = string;
                string = strtok(NULL, " ");
                k++;
            }
            exec_args[k] = NULL;

            execvp(exec_args[0], exec_args);
            exit(0);
        }
        else
        {
            // parent process
            if (in_fd != 0)
            {
                close(in_fd);
            }
            in_fd = fd[0];
            close(fd[1]);
        }
    }
    for (int j = 0; j < i; j++)
    {
        waitpid(pid[j], NULL, 0);
    }
    gettimeofday(&fim, NULL);
    segundos = fim.segundos - comeco.segundos;
    microseg = fim.microseg - comeco.microseg;
    long tempoTotal = (segundos * 1000) + (microseg / 1000);
    return tempoTotal;
}