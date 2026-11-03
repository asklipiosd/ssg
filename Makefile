main:
	cc -Wall -Wextra $(pkgconf --cflags --libs libmarkdown) main.c templ.c -o ssg
