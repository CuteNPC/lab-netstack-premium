.PHONY: all

CC = gcc
CFLAG = -g -I../../src
HEADER = ippacket.h ipheader.h route.h arp.h network.h
OBJPATH = ../../build/network
TARGET = ${OBJPATH}/arp.o ${OBJPATH}/ipheader.o ${OBJPATH}/ippacket.o ${OBJPATH}/route.o ${OBJPATH}/network.o 

all: $(TARGET)

${OBJPATH} :
	mkdir -p  ${OBJPATH}

${OBJPATH}/arp.o       : $(HEADER) arp.c   ${OBJPATH}
	$(CC) $(CFLAG) -c arp.c -o ${OBJPATH}/arp.o

${OBJPATH}/ipheader.o        : $(HEADER) ipheader.c    ${OBJPATH}
	$(CC) $(CFLAG) -c ipheader.c -o ${OBJPATH}/ipheader.o

${OBJPATH}/ippacket.o         : $(HEADER) ippacket.c     ${OBJPATH}
	$(CC) $(CFLAG) -c ippacket.c -o ${OBJPATH}/ippacket.o

${OBJPATH}/route.o    : $(HEADER) route.c ${OBJPATH}
	$(CC) $(CFLAG) -c route.c -o ${OBJPATH}/route.o

${OBJPATH}/network.o    : $(HEADER) network.c ${OBJPATH}
	$(CC) $(CFLAG) -c network.c -o ${OBJPATH}/network.o

clean:
	rm -r $(OBJPATH)