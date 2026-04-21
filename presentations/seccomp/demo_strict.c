#include <stdio.h>
#include <sys/prctl.h>
#include <linux/seccomp.h>

int main(void) {
    printf("Before seccomp: everything works fine\n");

    prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT);

    printf("After seccomp: write() still works (fd=stdout)\n");

    printf("Attempting open()...\n");

    fopen("/etc/passwd", "r");

    printf("You will never see this\n");
    return 0;
}
