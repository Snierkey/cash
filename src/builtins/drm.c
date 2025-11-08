#include "cash.h"

int cmd_drm(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "usage: drm <directory>\n");
        return 1;
    }
    if (mkdir(argv[1], 0755) < 0) {
        perror("drm");
        return 1;
    }
    return 0;
}
