#ifndef STR_H
#define STR_H
#include <stdlib.h>

typedef struct {
	size_t len;
	size_t cap;
	char *buf;
} str;

typedef struct {
	size_t len;
	char *buf;
} slice;

static inline void initChar(str *s) {
	s->cap = 5;
	s->len = 0;
	s->buf = malloc(sizeof(char) * s->cap);
}

static inline void appendChar(str *s, char c) {
	if (s->len < s->cap) {
	} else {
 		s->cap = 2 * s->cap + 1;
		s->buf = realloc(s->buf, s->cap);
	}
	s->buf[s->len] = c;
	s->len++;
}

static inline void appendSlice(str *s, slice *sl) {
	for (size_t i = 0; i < sl->len; ++i) {
		appendChar(s, sl->buf[i]);
	}
}

static inline void appendStr(str *s, str *sl) {
	for (size_t i = 0; i < sl->len; ++i) {
		appendChar(s, sl->buf[i]);
	}
}

#define append(s,T) _Generic((T), \
        char*  :   appendChar(s, T), \
        str*   :    appendStr(s, T), \
        slice* :  appendSlice(s, T), \
)

#endif
