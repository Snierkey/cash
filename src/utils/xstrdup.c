#include "cash.h"
char *xstrdup(const char *s)
{
    char *p = strdup(s);
    if (!p) die("strdup");
    return p;
}
