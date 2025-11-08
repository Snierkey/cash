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

#define CASH_PROMPT "cash> "
#define MAX_ARGS 64
#define MAX_LINE 4096

int parse_line(char *line, char **argv, int max);

/* built ins */
int cmd_drm(int argc, char **argv);
int cmd_rmv(int argc, char **argv);
int cmd_lnp(int argc, char **argv);
int cmd_dp(int argc, char **argv);
int cmd_sft(int argc, char **argv);
int cmd_show(int argc, char **argv);

void die(const char *msg);
char *xstrdup(const char *s);

#endif
