#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void)
{
    int pipefd[2];

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    if (pid == 0)
    {
        close(pipefd[1]);

        char buffer[256];
        ssize_t n;

        while ((n = read(pipefd[0], buffer,
                         sizeof(buffer))) > 0)
        {
            for (ssize_t i = 0; i < n; i++)
            {
                buffer[i] = toupper(
                    (unsigned char)buffer[i]);
            }

            fwrite(buffer, 1, n, stdout);
            fflush(stdout);
        }

        if (n < 0)
        {
            perror("read");
            close(pipefd[0]);
            return 1;
        }

        close(pipefd[0]);
        return 0;
    }
    else
    {
        close(pipefd[0]);

        char text[256];

        printf("Enter text (empty line to exit):\n");
        fflush(stdout);

        while (1)
        {
            printf("> ");
            fflush(stdout);

            if (fgets(text, sizeof(text), stdin) == NULL)
                break;

            if (strcmp(text, "\n") == 0)
                break;

            size_t len = strlen(text);
            size_t sent = 0;

            while (sent < len)
            {
                ssize_t n = write(pipefd[1],
                                  text + sent,
                                  len - sent);

                if (n <= 0)
                {
                    perror("write");
                    close(pipefd[1]);
                    waitpid(pid, NULL, 0);
                    return 1;
                }

                sent += n;
            }
        }

        close(pipefd[1]);
        waitpid(pid, NULL, 0);

        printf("Program finished.\n");
    }

    return 0;
}