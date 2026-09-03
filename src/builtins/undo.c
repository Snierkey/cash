#include "cash.h"

int cmd_undo(int argc, char **argv)
{
    int n = 1;
    if (argc > 2) {
        fprintf(stderr, "usage: undo [count]\n");
        return 1;
    }
    if (argc == 2) {
        n = atoi(argv[1]);
        if (n <= 0) {
            fprintf(stderr, "usage: undo [count]\n");
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

