#include "cash.h"

#define MAX_JOURNAL 256

static journal_entry_t log_[MAX_JOURNAL];
static int count = 0;

static void push(journal_op_t op, const char *from, const char *to)
{
    if (count >= MAX_JOURNAL) {
        memmove(&log_[0], &log_[1], sizeof(journal_entry_t) * (MAX_JOURNAL - 1));
        --count;
    }
    journal_entry_t *e = &log_[count++];
    e->op = op;
    e->from[0] = '\0';
    e->to[0] = '\0';
    if (from) strncpy(e->from, from, sizeof e->from - 1);
    if (to) strncpy(e->to, to, sizeof e->to - 1);
}

void journal_push_rename(const char *from, const char *to)
{
    push(J_RENAME, from, to);
}

void journal_push_unlink(const char *path)
{
    push(J_UNLINK, path, NULL);
}

void journal_push_rmdir(const char *path)
{
    push(J_RMDIR, path, NULL);
}

int journal_count(void)
{
    return count;
}

int journal_pop(journal_entry_t *out)
{
    if (count == 0) return 0;
    *out = log_[--count];
    return 1;
}

int journal_peek(journal_entry_t *out)
{
    if (count == 0) return 0;
    *out = log_[count - 1];
    return 1;
}

