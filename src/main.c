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
    {"fref", cmd_fref},
    {"undo", cmd_undo},
    {NULL, NULL}
};

typedef struct {
    char *argv[MAX_ARGS];
    int argc;
} cmd_t;

static int run_external(char **argv)
{
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return 1;
    }
    if (pid == 0) {
        execvp(argv[0], argv);
        perror(argv[0]);
        _exit(127);
    }
    int status;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return 1;
    }
    return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
}

static int execute(int argc, char **argv)
{
    if (argc == 0) return 0;
    for (size_t i = 0; builtins[i].name; ++i) {
        if (strcmp(argv[0], builtins[i].name) == 0)
            return builtins[i].func(argc, argv);
    }
    return run_external(argv);
}

/* splits argv on "|" into pipeline stages, pulls "<", ">", ">>" out as
   redirects on the whole pipeline. returns stage count, or -1 on error. */
static int split_pipeline(char **argv, cmd_t cmds[], int max_cmds,
                           char **infile, char **outfile, int *append)
{
    int ncmds = 0;
    int j = 0;

    *infile = NULL;
    *outfile = NULL;
    *append = 0;

    for (int i = 0; argv[i]; ++i) {
        if (strcmp(argv[i], "|") == 0) {
            cmds[ncmds].argv[j] = NULL;
            cmds[ncmds].argc = j;
            if (++ncmds >= max_cmds) {
                fprintf(stderr, "cash: too many pipeline stages\n");
                return -1;
            }
            j = 0;
        } else if (strcmp(argv[i], "<") == 0) {
            if (!argv[i + 1]) {
                fprintf(stderr, "cash: expected filename after <\n");
                return -1;
            }
            *infile = argv[++i];
        } else if (strcmp(argv[i], ">") == 0) {
            if (!argv[i + 1]) {
                fprintf(stderr, "cash: expected filename after >\n");
                return -1;
            }
            *outfile = argv[++i];
            *append = 0;
        } else if (strcmp(argv[i], ">>") == 0) {
            if (!argv[i + 1]) {
                fprintf(stderr, "cash: expected filename after >>\n");
                return -1;
            }
            *outfile = argv[++i];
            *append = 1;
        } else {
            if (j >= MAX_ARGS - 1) {
                fprintf(stderr, "cash: too many arguments\n");
                return -1;
            }
            cmds[ncmds].argv[j++] = argv[i];
        }
    }
    cmds[ncmds].argv[j] = NULL;
    cmds[ncmds].argc = j;
    return ++ncmds;
}

static int run_pipeline(cmd_t cmds[], int ncmds,
                         const char *infile, const char *outfile, int append)
{
    int in_fd = -1;
    if (infile) {
        in_fd = open(infile, O_RDONLY);
        if (in_fd < 0) {
            perror(infile);
            return 1;
        }
    }

    pid_t pids[MAX_CMDS];
    int prev_read = -1;

    for (int i = 0; i < ncmds; ++i) {
        int have_pipe = (i < ncmds - 1);
        int pipefd[2];
        if (have_pipe && pipe(pipefd) < 0) {
            perror("pipe");
            return 1;
        }

        pid_t pid = fork();
        if (pid < 0) {
            perror("fork");
            return 1;
        }
        if (pid == 0) {
            if (i == 0 && in_fd != -1) {
                dup2(in_fd, STDIN_FILENO);
            } else if (prev_read != -1) {
                dup2(prev_read, STDIN_FILENO);
            }
            if (have_pipe) {
                dup2(pipefd[1], STDOUT_FILENO);
            } else if (outfile) {
                int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
                int out_fd = open(outfile, flags, 0644);
                if (out_fd < 0) {
                    perror(outfile);
                    _exit(1);
                }
                dup2(out_fd, STDOUT_FILENO);
                close(out_fd);
            }
            if (in_fd != -1) close(in_fd);
            if (prev_read != -1) close(prev_read);
            if (have_pipe) {
                close(pipefd[0]);
                close(pipefd[1]);
            }
            int rc = execute(cmds[i].argc, cmds[i].argv);
            fflush(stdout);
            _exit(rc);
        }

        pids[i] = pid;
        if (prev_read != -1) close(prev_read);
        prev_read = have_pipe ? (close(pipefd[1]), pipefd[0]) : -1;
    }

    if (in_fd != -1) close(in_fd);

    int status = 0;
    for (int i = 0; i < ncmds; ++i) {
        int st;
        waitpid(pids[i], &st, 0);
        if (i == ncmds - 1) status = WIFEXITED(st) ? WEXITSTATUS(st) : 1;
    }
    return status;
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

        cmd_t cmds[MAX_CMDS];
        char *infile, *outfile;
        int append;
        int ncmds = split_pipeline(argv, cmds, MAX_CMDS, &infile, &outfile, &append);
        if (ncmds < 0) continue;

        if (ncmds == 1 && !infile && !outfile)
            execute(cmds[0].argc, cmds[0].argv);
        else
            run_pipeline(cmds, ncmds, infile, outfile, append);
    }
    return 0;
}
