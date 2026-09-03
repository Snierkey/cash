#include "cash.h"

int cmd_sft(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: sft <src> <dst>\n");
        return 1;
    }
    if (rename(argv[1], argv[2]) < 0) {
        perror("sft");
        return 1;
    }
    journal_push_rename(argv[2], argv[1]);
    return 0;
}
