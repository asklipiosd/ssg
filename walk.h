#ifndef WALK_H
#define WALK_H

typedef enum { F, D } fsobj;
int walk(const char *c, int (*callback)(fsobj, char *, void *), void *usr);

#endif
