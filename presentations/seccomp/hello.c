#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
    int fd = open("hello-world.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return 1; }
    write(fd, "hello-world\n", 12);
    printf("written, sleeping 120s...\n");
    sleep(120);
    close(fd);
    return 0;
}
