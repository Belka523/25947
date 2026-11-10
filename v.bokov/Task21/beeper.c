#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t beep_count = 0;

void sigint_handler(int sig)
{
    (void)sig;

    write(STDOUT_FILENO, "\a", 1);
    beep_count++;
}

void sigquit_handler(int sig)
{
    (void)sig;

    const char msg[] = "\nTotal beeps: ";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);

    char number[20];
    int i = 0;
    unsigned int n = (unsigned int)beep_count;

    do
    {
        number[i++] = '0' + n % 10;
        n /= 10;
    }
    while (n > 0);

    while (i > 0)
    {
        i--;
        write(STDOUT_FILENO, &number[i], 1);
    }

    const char end[] = "\nProgram finished.\n";
    write(STDOUT_FILENO, end, sizeof(end) - 1);

    _exit(0);
}

int main(void)
{
    struct sigaction sa_int = {0};
    struct sigaction sa_quit = {0};

    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sigaddset(&sa_int.sa_mask, SIGQUIT);
    sa_int.sa_flags = 0;

    if (sigaction(SIGINT, &sa_int, NULL) == -1)
    {
        perror("sigaction SIGINT");
        return 1;
    }

    sa_quit.sa_handler = sigquit_handler;
    sigemptyset(&sa_quit.sa_mask);
    sigaddset(&sa_quit.sa_mask, SIGINT);
    sa_quit.sa_flags = 0;

    if (sigaction(SIGQUIT, &sa_quit, NULL) == -1)
    {
        perror("sigaction SIGQUIT");
        return 1;
    }

    printf("Beeper started!\n");
    printf("Ctrl+C - beep\n");
    printf("Ctrl+\\ - show count and exit\n");
    fflush(stdout);

    while (1)
    {
        pause();
    }

    return 0;
}