#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "templ.h"

void initChar(str *s) {
	s->cap = 5;
	s->len = 0;
	s->buf = malloc(sizeof(char) * s->cap);
}

void appendChar(str *s, char c) {
	if (s->len < s->cap) {
	} else {
 		s->cap = 2 * s->cap + 1;
		s->buf = realloc(s->buf, s->cap);
	}
	s->buf[s->len] = c;
	s->len++;
}

char *renderTemplate(const char *in,char *(*cb)(char *, void *), void *user) {
	size_t og_len = strlen(in);
	bool inSection = false;
	str lastId;
	initChar(&lastId);
	str output = {
		.cap = og_len,
		.len = 0,
		.buf = malloc(sizeof(char) * og_len),
	};

	for (size_t i = 0; i < og_len; ++i) {
		if (in[i + 1] == SEP[1] && in[i] == SEP[0]) {
			if (inSection) {
				inSection = false;
				i++;
				appendChar(&lastId, '\0');
				char *ret =  cb(lastId.buf, user);
				size_t retlen = strlen(ret);
				for (size_t y = 0; y < retlen; ++y) {
					appendChar(&output, ret[y]);
				}
				free(lastId.buf);
				initChar(&lastId);
				continue;
			} else {
				inSection = true;
				i++;
				continue;
			}
		}
		if (!inSection) {
			appendChar(&output, in[i]);
		} else {
			appendChar(&lastId, in[i]);
		}
	}
	free(lastId.buf);
	appendChar(&output, '\0');
	return output.buf;
}

char *read_all(FILE *fp, size_t *out_len) {
	if (fseek(fp, 0, SEEK_END) != 0) return NULL;
	long size = ftell(fp);
	if (size < 0) return NULL;
	rewind(fp);
	
	char *buf = malloc((size_t)size + 1);
	if (!buf) return NULL;
	
	size_t n = fread(buf, 1, (size_t)size, fp);
	buf[n] = '\0';
	
	if (out_len) *out_len = n;
	return buf;
}

void renderTemplateFile(FILE *inf,FILE *out, char *(*cb)(char *, void *), void *user) {
	size_t inLen;
	char *in = read_all(inf, &inLen);
	size_t og_len = strlen(in);
	bool inSection = false;
	str lastId;
	initChar(&lastId);
	for (size_t i = 0; i < og_len; ++i) {
		if (in[i + 1] == SEP[1] && in[i] == SEP[0]) {
			if (inSection) {
				inSection = false;
				i++;
				appendChar(&lastId, '\0');
				char *ret =  cb(lastId.buf, user);
				size_t retlen = strlen(ret);
				for (size_t y = 0; y < retlen; ++y) {
					fputc(ret[y], out);
				}
				free(lastId.buf);
				initChar(&lastId);
				continue;
			} else {
				inSection = true;
				i++;
				continue;
			}
		}
		if (!inSection) {
			fputc(in[i], out);
		} else {
			appendChar(&lastId, in[i]);
		}
	}
}
