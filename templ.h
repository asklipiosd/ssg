#ifndef TEMPL
#define TEMPL
#include <stdio.h>

#define SEP "@@"

char *renderTemplate(const char *in,char *(*cb)(char *, void *), void *user);
void renderTemplateFile(FILE *inf,FILE *out, char *(*cb)(char *, void *), void *user);

typedef struct {
	size_t len;
	size_t cap;
	char *buf;
} str;

void initChar(str *s);

void appendChar(str *s, char c);

#endif
