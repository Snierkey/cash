#include "cash.h"

int cmd_lnp(int argc, char **argv)
{
    const char *path = (argc > 1) ? argv[1] : ".";
    DIR *d = opendir(path);
    if (!d) {
        perror("lnp");
        return 1;
    }
    struct dirent *ent;
    while ((ent = readdir(d))) {
        puts(ent->d_name);
    }
    closedir(d);
    return 0;
}
