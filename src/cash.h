#ifndef CASH_H
#define CASH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/wait.h>

#define CASH_PROMPT "cash> "
#define MAX_ARGS 64
#define MAX_LINE 4096
#define MAX_CMDS 16

int parse_line(char *line, char **argv, int max);

/* built ins */
int cmd_drm(int argc, char **argv);
int cmd_rmv(int argc, char **argv);
int cmd_lnp(int argc, char **argv);
int cmd_dp(int argc, char **argv);
int cmd_sft(int argc, char **argv);
int cmd_show(int argc, char **argv);
int cmd_fref(int argc, char **argv);
int cmd_undo(int argc, char **argv);

void die(const char *msg);
char *xstrdup(const char *s);

typedef enum { J_RENAME, J_UNLINK, J_RMDIR } journal_op_t;

typedef struct {
    journal_op_t op;
    char from[PATH_MAX];
    char to[PATH_MAX];
} journal_entry_t;

void journal_push_rename(const char *from, const char *to);
void journal_push_unlink(const char *path);
void journal_push_rmdir(const char *path);
int journal_pop(journal_entry_t *out);
int journal_count(void);

#endif
