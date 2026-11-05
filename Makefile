CC      := gcc
INCLUDE := $(shell pkg-config --cflags libmarkdown)
LINK    := $(shell pkg-config --libs   libmarkdown)
FLAGS   := -Wall -Wextra -g
BIN     := ssg

.PHONY: default run clean

default: main.o templ.o
	${CC} ${FLAGS} main.o templ.o  ${LINK} -o ${BIN}

run: default
	./${BIN}

main.o: main.c
	${CC} ${FLAGS} ${INCLUDE} -c $< -o $@

templ.o: templ.c
	${CC} ${FLAGS} ${INCLUDE} -c $< -o $@

clean:
	rm main.o templ.o ${BIN}
