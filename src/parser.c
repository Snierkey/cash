#include "cash.h"

int parse_line(char *line, char **argv, int max)
{
    int argc = 0;
    char *p = line;
    while (*p && argc < max - 1) {
        while (*p && strchr(" \t\r\n", *p)) *p++ = '\0';
        if (!*p) break;
        if (*p == '"') {
            ++p;
            argv[argc++] = p;
            while (*p && *p != '"') ++p;
            if (*p == '"') *p++ = '\0';
        } else {
            argv[argc++] = p;
            while (*p && !strchr(" \t\r\n", *p)) ++p;
        }
    }
    argv[argc] = NULL;
    return argc;
}
