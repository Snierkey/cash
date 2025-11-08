#include "cash.h"

static int rm_recursive(const char *path)
{
    struct stat st;
    if (lstat(path, &st) < 0) return -1;
    if (S_ISDIR(st.st_mode)) {
        DIR *d = opendir(path);
        if (!d) return -1;
        struct dirent *ent;
        while ((ent = readdir(d))) {
            if (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, ".."))
                continue;
            size_t len = strlen(path) + strlen(ent->d_name) + 2;
            char *buf = malloc(len);
            snprintf(buf, len, "%s/%s", path, ent->d_name);
            rm_recursive(buf);
            free(buf);
        }
        closedir(d);
        return rmdir(path);
    } else {
        return unlink(path);
    }
}

int cmd_rmv(int argc, char **argv)
{
    bool force = false;
    int idx = 1;
    if (argc > 1 && (strcmp(argv[1], "--force") == 0 || strcmp(argv[1], "-f") == 0)) {
        force = true;
        idx = 2;
    }
    if (idx >= argc) {
        fprintf(stderr, "usage: rmv [--force|-f] <file|dir> ...\n");
        return 1;
    }
    for (int i = idx; i < argc; ++i) {
        struct stat st;
        if (lstat(argv[i], &st) < 0) {
            perror(argv[i]);
            continue;
        }
        if (S_ISDIR(st.st_mode) && force) {
            if (rm_recursive(argv[i]) < 0) perror(argv[i]);
        } else {
            if (unlink(argv[i]) < 0) perror(argv[i]);
        }
    }
    return 0;
}
