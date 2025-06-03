#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        write(STDERR_FILENO, "Usage: mycat1 <filename>\n", 25);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    char buffer;
    ssize_t bytesRead;
    while ((bytesRead = read(fd, &buffer, 1)) > 0) {
        if (write(STDOUT_FILENO, &buffer, 1) == -1) {
            perror("write");
            close(fd);
            return 1;
        }
    }

    if (bytesRead == -1) {
        perror("read");
    }

    close(fd);
    return 0;
}
