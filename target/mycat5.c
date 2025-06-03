#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>
size_t buffer_size;
size_t gcd(size_t a, size_t b)
{
    while (b != 0)
    {
        size_t tmp = b;
        b = a % b;
        a = tmp;
    }
    return a;
}
size_t io_blocksize(int fd)
{
    struct stat file_stat;
    fstat(fd, &file_stat);
    size_t pg = sysconf(_SC_PAGESIZE),
           block_size = file_stat.st_blksize;
    size_t lcm = (pg * block_size) / gcd(pg, block_size);
    return lcm;
}

char *align_alloc(size_t size)
{
    size_t pg = sysconf(_SC_PAGESIZE);
    size_t need = size + pg - 1 + sizeof(void *); // 额外空间：对齐 + 存储原始指针
    void *raw = malloc(need);
    if (!raw)
        return NULL;

    uintptr_t base = (uintptr_t)raw + sizeof(void *);
    uintptr_t aligned = (base + pg - 1) & ~(uintptr_t)(pg - 1);

    ((void **)aligned)[-1] = raw; // 把原始指针存到对齐地址前一个 void* 位置
    return (char *)aligned;
}

void align_free(void *ptr)
{
    if (!ptr)
        return;
    void *raw = ((void **)ptr)[-1]; // 取出当初保存的原始 malloc 指针
    free(raw);
}
int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        write(STDERR_FILENO, "Usage: mycat2 <filename> <multiply>\n", 25);
        return 1;
    }

    int fd = open(argv[1], O_RDONLY);
    int a = atoi(argv[2]);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }
    buffer_size = a * io_blocksize(fd);
    char *buffer = align_alloc(buffer_size);
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
    align_free(buffer);
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
