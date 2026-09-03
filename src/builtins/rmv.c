#include "cash.h"

#define TRASH_DIR ".cash_trash"

static int ensure_trash_dir(void)
{
    struct stat st;
    if (stat(TRASH_DIR, &st) == 0) {
        if (!S_ISDIR(st.st_mode)) {
            fprintf(stderr, "rmv: %s exists and is not a directory\n", TRASH_DIR);
            return -1;
        }
        return 0;
    }
    if (mkdir(TRASH_DIR, 0700) < 0) {
        perror("rmv");
        return -1;
    }
    return 0;
}

static int trash(const char *path)
{
    if (ensure_trash_dir() < 0) return -1;

    const char *base = strrchr(path, '/');
    base = base ? base + 1 : path;

    static unsigned counter = 0;
    char trash_path[PATH_MAX];
    int r = snprintf(trash_path, sizeof trash_path, "%s/%ld_%u_%s",
                      TRASH_DIR, (long)getpid(), counter++, base);
    if (r < 0 || (size_t)r >= sizeof trash_path) {
        fprintf(stderr, "rmv: path too long: %s\n", path);
        return -1;
    }

    if (rename(path, trash_path) < 0) {
        perror(path);
        return -1;
    }

    journal_push_rename(trash_path, path);
    return 0;
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

    int status = 0;
    for (int i = idx; i < argc; ++i) {
        struct stat st;
        if (lstat(argv[i], &st) < 0) {
            perror(argv[i]);
            status = 1;
            continue;
        }
        if (S_ISDIR(st.st_mode) && !force) {
            fprintf(stderr, "rmv: %s is a directory (use --force)\n", argv[i]);
            status = 1;
            continue;
        }
        if (trash(argv[i]) < 0) status = 1;
    }
    return status;
}

