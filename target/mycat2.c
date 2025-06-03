#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
unsigned buffer_size;
unsigned io_blocksize()
{
    return sysconf(_SC_PAGESIZE);
}
int main(int argc, char *argv[])
{
    buffer_size = io_blocksize();
    if (argc != 2)
    {
        write(STDERR_FILENO, "Usage: mycat2 <filename>\n", 25);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    char *buffer = (char *)malloc(buffer_size);
    if (buffer == NULL)
    {
        perror("malloc");
        close(fd);
        return 1;
    }

    ssize_t bytesRead;
    while ((bytesRead = read(fd, buffer, buffer_size)) > 0)
    {
        if (write(STDOUT_FILENO, buffer, bytesRead) == -1)
        {
            perror("write");
            close(fd);
            return 1;
        }
    }
    free(buffer);
    buffer = NULL;
    if (bytesRead == -1)
    {
        perror("read");
    }

    close(fd);
    free(buffer);
    buffer = NULL;
    return 0;
}
