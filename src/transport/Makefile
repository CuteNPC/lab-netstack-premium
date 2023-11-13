.PHONY: all

CC = gcc
CFLAG = -g -I../../src
HEADER = loopthread.h socket.h tcpheader.h mainthread.h transport.h
OBJPATH = ../../build/transport
TARGET = ${OBJPATH}/mainthread.o ${OBJPATH}/socket.o ${OBJPATH}/loopthread.o ${OBJPATH}/tcpheader.o ${OBJPATH}/transport.o 

all: $(TARGET)

${OBJPATH} :
	mkdir -p  ${OBJPATH}

${OBJPATH}/mainthread.o       : $(HEADER) mainthread.c   ${OBJPATH}
	$(CC) $(CFLAG) -c mainthread.c      -o ${OBJPATH}/mainthread.o

${OBJPATH}/socket.o        : $(HEADER) socket.c    ${OBJPATH}
	$(CC) $(CFLAG) -c socket.c -o ${OBJPATH}/socket.o

${OBJPATH}/loopthread.o         : $(HEADER) loopthread.c     ${OBJPATH}
	$(CC) $(CFLAG) -c loopthread.c -o ${OBJPATH}/loopthread.o

${OBJPATH}/tcpheader.o    : $(HEADER) tcpheader.c ${OBJPATH}
	$(CC) $(CFLAG) -c tcpheader.c    -o ${OBJPATH}/tcpheader.o

${OBJPATH}/transport.o    : $(HEADER) transport.c ${OBJPATH}
	$(CC) $(CFLAG) -c transport.c  -o ${OBJPATH}/transport.o

clean:
	rm -r $(OBJPATH)
