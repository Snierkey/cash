#include "cash.h"
#include <fcntl.h>

int cmd_dp(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: dp <src> <dst>\n");
        return 1;
    }
    int src = open(argv[1], O_RDONLY);
    if (src < 0) {
        perror(argv[1]);
        return 1;
    }
    int dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst < 0) {
        close(src);
        perror(argv[2]);
        return 1;
    }
    char buf[8192];
    ssize_t n;
    while ((n = read(src, buf, sizeof buf)) > 0)
        if (write(dst, buf, n) != n) {
            perror("write");
            break;
        }
    close(src);
    close(dst);
    return 0;
}
