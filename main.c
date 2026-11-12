#include     <mkdio.h>
#include     <stdio.h>
#include    <dirent.h>
#include   <stdbool.h>
#include    <stdlib.h>
#include    <string.h>
#include    <libgen.h>
#include   <threads.h>
#include <stdatomic.h>
#include    <unistd.h>
#include      "walk.h"
#include     "templ.h"

typedef struct {
	char *given;
	char *output;
	bool success;
} templStatus;

typedef struct {
	char **list;
	size_t len;
        size_t cap;
  atomic_size_t ptr;
} charList;

typedef struct {
	char         *siteName;
	FILE      *contentFile;
        char          *content;
        char            *title;
} renderData;

FILE *xfopen(const char *path, const char *mode) {
    FILE *f = fopen(path, mode);
    if (!f) { perror(path); exit(1); }
    return f;
}

// -- [find template file] ----------------------------------------------------
char *stripExt(const char *d) {
	const char *dot = strrchr(d, '.');
	const char *slash = strrchr(d, '/');
	size_t len = (dot && (!slash || dot > slash)) ? (size_t)(dot - d) : strlen(d);
        char *r = malloc(len + 1);
        if (!r) {
          fputs("out of memory\n", stderr);
          exit(1);
	}
	memcpy(r, d, len);
	r[len] = '\0';
	return r;
}

char *baseNoExt(const char *p) {
	const char *slash = strrchr(p, '/');
	return stripExt(slash ? slash + 1 : p);
}

int templCb(fsobj t, char *c, void *data) {
	templStatus* d = (templStatus *) data;
	if (t == F){
	        char *fname = stripExt(d->given);
		char *one = baseNoExt(fname);
		char *two = baseNoExt(c);
		bool yes  = !strcmp(one, two);
		free(one);
                free(two);
		free(fname);
		if (yes) {
			d->success = true;
			d->output = malloc(sizeof(char) * (strlen(c) + 1));
			strcpy(d->output, c);
			return -1;
		}
	}
	return 0;
}

int rmCb(fsobj t, char *c, void *data) {
  (void) data;
  if (t == F) {
    remove(c);
  }
  return 0;
}

bool findTemplate(char* c, char **out) {
	templStatus d = {
		.output = NULL,
		.success = false,
		.given = c,
	};
	walk("./templates", &templCb, &d);
	if (d.success) {	
		*out = d.output;
		return true;
	}
	return false;
}

// -- [structure] -------------------------------------------------------------
//
// templates
// 	| index.html
// 	| menu.html
// 	| foo.html
// assets
// 	| img.png
// articles
// 	| programming
// 		| c_sucks.md        -> c_sucks.html
// 		| I_stil_use_c.md   -> I_stil_use_c.html
// 	| about
// 		| about.md          -> about.html
// out                              -> self explanatory

void appendFileList(charList *s, char *c) {
	if (s->len < s->cap) {
	} else {
 		s->cap = 2 * s->cap + 1;
		s->list = realloc(s->list, sizeof(char *) * s->cap);
	}
	s->list[s->len] = c;
	s->len++;
}

int inputCb(fsobj t, char *c, void *data) {
	charList *list = (charList *) data;
	if (t == F) {
		size_t n = strlen(c);
		if (n < 3 || strcmp(c + n - 3, ".md") != 0) {
                  return 0;
		}
		char *elem = malloc(sizeof(char) * (strlen(c) + 1));
		strcpy(elem, c);
		appendFileList(list, elem);
	}
	return 0;
}

char *renderCb(char *c, void *user) {
	renderData *data = (renderData *) user;
        if (!strcmp(c, "content")) {
                if (data->content[0] != '\0') {
		  return strdup(data->content);
                }
		if (fseek(data->contentFile, 0, SEEK_END) != 0) return NULL;
		long size = ftell(data->contentFile);
		if (size < 0) return NULL;
		rewind(data->contentFile);
		
		char *buf = malloc((size_t)size + 1);
		if (!buf) return NULL;
		
		size_t n = fread(buf, 1, (size_t)size, data->contentFile);
		buf[n] = '\0';
                mkd_flag_t *flags = mkd_flags();
		mkd_set_flag_num(flags, MKD_FENCEDCODE);
		MMIOT *m = mkd_string(buf, n, NULL);
                if (!mkd_compile(m, flags)) {
		  printf("oops\n");
                }
		char *html;
		mkd_document(m, &html);
                mkd_free_flags(flags);
                char *b = strdup(html);
		mkd_cleanup(m);
                free(buf);
		return b;
	} else if (!strcmp(c, "sitename")) {
	  return strdup(data->siteName);
	} else if (!strcmp(c, "title")) {
	  return strdup(data->title);
	} else {
		char *ptr = malloc(sizeof(char) * 1);
		ptr[0] = '\0';
		return ptr;
	}
}

void genFromFileName(char *input) {
  char *template;
  FILE *templFile;
  if (findTemplate(input, &template)) {
    templFile = xfopen(template, "r");
  } else {
    findTemplate("index.html", &template);
    templFile = xfopen(template, "r");
  }
  char *base = basename(input);
  char *nameNoExt = stripExt(base);
  char outName[
	       strlen("out/")                   +
	       strlen(nameNoExt)                +
	       strlen(".html")                  +
	       1
  ];
  sprintf(outName, "out/%s.html", nameNoExt);
  FILE *outFile = xfopen(outName, "w");
  renderData data = {
    .siteName = "asklipiosd",
    .content = "\0",
    .title = nameNoExt,
    .contentFile = xfopen(input, "r")
  };
  renderTemplateFile(templFile, outFile, &renderCb, &data);
  fclose(templFile);
  
  char *nameLink =malloc(strlen(nameNoExt) + strlen(".html") + 1);
  sprintf(nameLink, "%s.html", nameNoExt);
  free(nameLink);
  free(template);
  free(nameNoExt);
  fclose(data.contentFile);
}

void genSitemap(str *sitemap, char *input) {
  char *base = basename(input);
  char *nameNoExt = stripExt(base);
  char outName[
	       strlen("out/")                   +
	       strlen(nameNoExt)                +
	       strlen(".html")                  +
	       1
  ];
  sprintf(outName, "out/%s.html", nameNoExt);
  char *sitebuf = malloc(strlen("<ul><a href=\"\"></a></ul>\n") +
			 strlen(outName) + strlen(nameNoExt) + 1);
  char *nameLink =malloc(strlen(nameNoExt) + strlen(".html") + 1);
  sprintf(nameLink, "%s.html", nameNoExt);
  sprintf(sitebuf, "<li><a href=\"%s\">%s</a></li>\n", nameLink,
	  nameNoExt);
  int len = strlen(sitebuf);
  for (int y = 0; y < len; ++y) {
    appendChar(sitemap, sitebuf[y]);
  }
  free(sitebuf);
  free(nameLink);
  free(nameNoExt);
}

static int worker(void *arg) {
  charList *job = arg;
  size_t i = atomic_fetch_add(&job->ptr, 1);
  while (true) {
    if (i >= job->len) {
      break;
    }
    genFromFileName(job->list[i]);
    i = atomic_fetch_add(&job->ptr, 1);
  }
  return 0;
}

int main() {
//	FILE *f = xfopen("ex.md", "r");
//	FILE *fo = xfopen("out.html", "w");
//	MMIOT *m = mkd_in(f, mkd_flags());
//	markdown(m, fo, mkd_flags());;
        walk("out", &rmCb, NULL);
        charList input = {
          .cap = 5,
          .len = 0,
	  .ptr = 0,
		.list = malloc(sizeof(char *) * 5)
	};
	FILE *sitemapFile = xfopen("out/sitemap.html", "w");
	str sitemap;
	initChar(&sitemap);
	for (int i = 0; i < 4; i++) {
		appendChar(&sitemap, "<ul>"[i]);
	}

	walk("articles", &inputCb, (void *) &input);
        long N = sysconf(_SC_NPROCESSORS_ONLN);


	thrd_t th[N];
	for (int i = 0; i < N; i++) {
          if (thrd_create(&th[i], worker,(void *) &input) != thrd_success) {
	    exit(1);
	  }
	}
	for (int i = 0; i < N; i++){
	  thrd_join(th[i], NULL);
        }
	  
        for (size_t i = 0; i < input.len; ++i) {
          genSitemap(&sitemap, input.list[i]);
	  free(input.list[i]);
	}
	for (int i = 0; i < 5; i++) {
		appendChar(&sitemap, "</ul>"[i]);
	}
        appendChar(&sitemap, '\0');

	char *template;
	FILE *templFile;
	if (findTemplate("sitemap.html", &template)) {
		templFile = xfopen(template, "r");
	} else {
		findTemplate("index.html", &template);
		templFile = xfopen(template, "r");
        }

        renderData data = {
          .siteName = "asklipiosd",
          .content = sitemap.buf,
	  .title = "sitemap",
	};
        renderTemplateFile(templFile, sitemapFile, &renderCb, &data);
	free(input.list);
        free(sitemap.buf);
	free(template);
	fclose(templFile);
	fclose(sitemapFile);
}
