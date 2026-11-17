#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include "walk.h"

int walk(const char *c, int (*callback)(fsobj, char *, void *), void *usr) {
	DIR *d = opendir(c);
	if (d == NULL) {
		fprintf(stderr, "couldn't open dir %s\n", c);
		exit(1);
	}

	struct dirent *de;
	while ((de = readdir(d)) != NULL ) {
		if (!strcmp(de->d_name, ".")) {
		} else if (!strcmp(de->d_name, "..")) {
		} else if (de->d_type == DT_DIR) {
			// -- [dir] ----------------------------------------------------------------
			int n = strlen(c) + strlen(de->d_name) + 2;
			char nd[n];
			snprintf(nd, n, "%s/%s", c, de->d_name);
                        if (callback(D, nd, usr) == -1) {
			        closedir(d);
				return -1;
			}
			if (walk(nd, callback, usr) == -1) {
			        closedir(d);
				return -1;

			}
		} else if (de->d_type == DT_REG) {
			// -- [file] ---------------------------------------------------------------
			int n = strlen(c) + strlen(de->d_name) + 2;
			char nd[n];
			snprintf(nd, n, "%s/%s", c, de->d_name);
			if (callback(F, nd, usr) == -1) {
			        closedir(d);
				return -1;
			}
		}
	}
	closedir(d);
	return 0;
}

