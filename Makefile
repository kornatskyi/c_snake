# Makefile for JustForFun Files

# A few variables

CC=gcc -g -O0
LIBS=-lncurses

SRC_DIR=.
EXE_DIR=./bin

EXES = \
    ${EXE_DIR}/snake \

${EXE_DIR}/%: %.o
	${CC} -Wall -Wextra -o $@ $< ${LIBS}

%.o: ${SRC_DIR}/%.c
	${CC} -o $@ -c $<

all:    ${EXES}

clean:
	@rm -f ${EXES}
