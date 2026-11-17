#ifndef TEMPL
#define TEMPL
#include <stdio.h>

#define SEP "@@"

char *renderTemplate(const char *in,char *(*cb)(char *, void *), void *user);
void renderTemplateFile(FILE *inf,FILE *out, char *(*cb)(char *, void *), void *user);

#endif
