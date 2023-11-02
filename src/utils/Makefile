.PHONY: all

CC = gcc
CFLAG = -g -I../../src
HEADER = debug.h error.h time.h
OBJPATH = ../../build/utils
TARGET = ${OBJPATH}/debug.o ${OBJPATH}/error.o ${OBJPATH}/time.o

all: $(TARGET)

${OBJPATH} :
	mkdir -p  ${OBJPATH}

${OBJPATH}/debug.o       : $(HEADER) debug.c   ${OBJPATH}
	$(CC) $(CFLAG) -c debug.c -o ${OBJPATH}/debug.o

${OBJPATH}/error.o        : $(HEADER) error.c    ${OBJPATH}
	$(CC) $(CFLAG) -c error.c -o ${OBJPATH}/error.o

${OBJPATH}/time.o        : $(HEADER) time.c    ${OBJPATH}
	$(CC) $(CFLAG) -c time.c -o ${OBJPATH}/time.o

clean:
	rm -r $(OBJPATH)