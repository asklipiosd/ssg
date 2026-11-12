CC      := gcc
INCLUDE := $(shell pkg-config --cflags libmarkdown)
LINK    := $(shell pkg-config --libs   libmarkdown)
FLAGS   := -Wall -Wextra -g -fsanitize=address,undefined
BIN     := ssg
BLD_DIR := build

MAIN    := ${BLD_DIR}/main.o
TEMPL   := ${BLD_DIR}/templ.o
WALK    := ${BLD_DIR}/walk.o

.PHONY: default run clean

default: ${MAIN} ${TEMPL} ${WALK}
	${CC} ${FLAGS} ${MAIN} ${TEMPL} ${WALK} ${LINK} -o ${BIN}
run: default
	./${BIN}
${MAIN}: main.c
	${CC} ${FLAGS} ${INCLUDE} -c $< -o $@

${TEMPL}: templ.c
	${CC} ${FLAGS} ${INCLUDE} -c $< -o $@

${WALK}: walk.c
	${CC} ${FLAGS} ${INCLUDE} -c $< -o $@
clean:
	rm ${MAIN} ${TEMPL} ${WALK} ${BIN}
