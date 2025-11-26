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
            
            char path_buf[PATH_MAX];
            int r = snprintf(path_buf, sizeof(path_buf), "%s/%s", path, ent->d_name);
            if (r < 0 || (size_t)r >= sizeof(path_buf)) {
                fprintf(stderr, "rmv: path too long: %s/%s\n", path, ent->d_name);
                // continue to try and remove other files, but this one failed
                // could also return -1 here to stop the whole operation
                continue; 
            }
            rm_recursive(path_buf);
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