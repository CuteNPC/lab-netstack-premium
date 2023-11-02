.PHONY: all

CC = gcc
CFLAG = -g -I../../src
HEADER = link.h frame.h ethheader.h device.h
OBJPATH = ../../build/link
TARGET = ${OBJPATH}/device.o ${OBJPATH}/frame.o ${OBJPATH}/link.o ${OBJPATH}/ethheader.o

all: $(TARGET)

${OBJPATH} :
	mkdir -p  ${OBJPATH}

${OBJPATH}/device.o       : device.c    ${OBJPATH}
	$(CC) $(CFLAG) -c device.c -o ${OBJPATH}/device.o

${OBJPATH}/frame.o        : frame.c     ${OBJPATH}
	$(CC) $(CFLAG) -c frame.c -o ${OBJPATH}/frame.o

${OBJPATH}/link.o         : link.c      ${OBJPATH}
	$(CC) $(CFLAG) -c link.c -o  ${OBJPATH}/link.o

${OBJPATH}/ethheader.o    : ethheader.c ${OBJPATH}
	$(CC) $(CFLAG) -c ethheader.c -o ${OBJPATH}/ethheader.o

clean:
	rm -r $(OBJPATH)