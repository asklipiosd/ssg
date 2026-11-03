main:
	cc -Wall -Wextra $(pkg-config --cflags --libs libmarkdown) main.c templ.c -o ssg
