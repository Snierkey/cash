#include "cash.h"

static const struct {
    const char *name;
    int (*func)(int, char **);
} builtins[] = {
    {"drm",  cmd_drm},
    {"rmv",  cmd_rmv},
    {"lnp",  cmd_lnp},
    {"dp",   cmd_dp},
    {"sft",  cmd_sft},
    {"show", cmd_show},
    {NULL, NULL}
};

static int execute(int argc, char **argv)
{
    if (argc == 0) return 0;
    for (size_t i = 0; builtins[i].name; ++i) {
        if (strcmp(argv[0], builtins[i].name) == 0)
            return builtins[i].func(argc, argv);
    }
    fprintf(stderr, "cash: unknown command: %s\n", argv[0]);
    return 1;
}

int main(void)
{
    char line[MAX_LINE];
    char *argv[MAX_ARGS];
    int argc;

    while (1) {
        fputs(CASH_PROMPT, stdout);
        fflush(stdout);
        if (!fgets(line, sizeof line, stdin)) {
            if (feof(stdin)) {
                fputc('\n', stdout);
                break;
            }
            continue;
        }
        argc = parse_line(line, argv, MAX_ARGS);
        if (argc == 0) continue;
        if (strcmp(argv[0], "exit") == 0) break;
        execute(argc, argv);
    }
    return 0;
}
