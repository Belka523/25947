#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

struct LineInfo {
    long pos;
    int len;
};

int main(int args, char **argv)
{
    if (args < 2)
    {
        printf("Enter filename\n");
        return 1;
    }

    int fopen = open(argv[1], O_RDONLY);

    struct LineInfo table[100];
    int lines = 0;

    char c;
    long start = 0;

    while (read(fopen, &c, 1) > 0)
    {
        if (c == '\n')
        {
            long pos = lseek(fopen, 0L, 1);

            table[lines].pos = start;
            table[lines].len = pos - start;

            lines++;
            start = pos;
        }
    }
    long file_size = lseek(fopen, 0L, SEEK_END);
    printf("File size: %ld bytes\n", file_size);
    for (int i = 0; i < lines; i++)
    {
        printf("Line %d: pos = %ld, len = %d\n",
               i + 1, table[i].pos, table[i].len);
    }

    int number;

    while (1)
    {
        printf("number line:");

        if (scanf("%d", &number) != 1)
        {
            break;
        }

        if (number == 0)
        {
            break;
        }

        int index = number - 1;

        lseek(fopen, table[index].pos, 0);

        char *buffer = malloc(table[index].len + 1);

        read(fopen, buffer, table[index].len);
        buffer[table[index].len] = '\0';

        printf("%s", buffer);

        free(buffer);
    }
    long end = lseek(fopen, 0L, SEEK_CUR);

    if (end > start && lines < 100)
    {
        table[lines].pos = start;
        table[lines].len = end - start;
        lines++;
    }

    close(fopen);

    return 0;
}