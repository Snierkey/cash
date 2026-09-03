#include "cash.h"

int cmd_undo(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "-p") == 0) {
        journal_entry_t e;
        if (!journal_peek(&e)) {
            fprintf(stderr, "undo: nothing to undo\n");
            return 1;
        }
        switch (e.op) {
        case J_RENAME:
            printf("would restore %s\n", e.to);
            break;
        case J_UNLINK:
        case J_RMDIR:
            printf("would remove %s\n", e.from);
            break;
        }
        return 0;
    }

    int n = 1;
    if (argc > 2) {
        fprintf(stderr, "usage: undo [count|-p]\n");
        return 1;
    }
    if (argc == 2) {
        n = atoi(argv[1]);
        if (n <= 0) {
            fprintf(stderr, "usage: undo [count|-p]\n");
            return 1;
        }
    }

    int status = 0;
    for (int i = 0; i < n; ++i) {
        journal_entry_t e;
        if (!journal_pop(&e)) {
            if (i == 0) fprintf(stderr, "undo: nothing to undo\n");
            break;
        }
        switch (e.op) {
        case J_RENAME:
            if (rename(e.from, e.to) < 0) {
                perror("undo");
                status = 1;
            } else {
                printf("restored %s\n", e.to);
            }
            break;
        case J_UNLINK:
            if (unlink(e.from) < 0) {
                perror("undo");
                status = 1;
            } else {
                printf("removed %s\n", e.from);
            }
            break;
        case J_RMDIR:
            if (rmdir(e.from) < 0) {
                perror("undo");
                status = 1;
            } else {
                printf("removed %s\n", e.from);
            }
            break;
        }
    }
    return status;
}

