#include "cash.h"

int cmd_fref(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "usage: fref <pattern> <file1> [file2 ...]\n");
        return 1;
    }

    const char *pattern = argv[1];
    int status = 0;

    for (int i = 2; i < argc; ++i) {
        const char *filepath = argv[i];
        FILE *fp = fopen(filepath, "r");
        if (!fp) {
            perror(filepath);
            status = 1;
            continue;
        }

        char line[MAX_LINE];
        while (fgets(line, sizeof(line), fp)) {
            if (strstr(line, pattern)) {
                size_t len = strlen(line);
                if (len > 0 && line[len - 1] == '\n') {
                    line[len - 1] = '\0';
                }
                printf("%s: %s\n", filepath, line);
            }
        }

        if (ferror(fp)) {
            perror(filepath);
            status = 1;
        }

        fclose(fp);
    }

    return status;
}