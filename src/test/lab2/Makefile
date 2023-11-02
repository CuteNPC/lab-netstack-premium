.PHONY: all

CC = gcc
CFLAG = -g -I../../../src
BUILDPATH = ../../../build
SRCOBJ = $(BUILDPATH)/link/device.o $(BUILDPATH)/link/ethheader.o $(BUILDPATH)/link/frame.o $(BUILDPATH)/link/link.o $(BUILDPATH)/network/arp.o $(BUILDPATH)/network/ipheader.o $(BUILDPATH)/network/ippacket.o $(BUILDPATH)/network/network.o $(BUILDPATH)/network/route.o $(BUILDPATH)/utils/debug.o $(BUILDPATH)/utils/error.o $(BUILDPATH)/utils/time.o 
EXEPATH = $(BUILDPATH)/test/lab2

all: ${EXEPATH}/looptest ${EXEPATH}/sender ${EXEPATH}/sendercp6 ${EXEPATH}/receivercp6

${EXEPATH} :
	mkdir -p  ${EXEPATH}

${EXEPATH}/looptest       : looptest.c ${EXEPATH}
	$(CC) $(CFLAG) $(SRCOBJ) looptest.c -lpcap -o ${EXEPATH}/looptest

${EXEPATH}/sender        : sender.c ${EXEPATH}
	$(CC) $(CFLAG) $(SRCOBJ) sender.c -lpcap -o ${EXEPATH}/sender

${EXEPATH}/sendercp6       : sendercp6.c ${EXEPATH}
	$(CC) $(CFLAG) $(SRCOBJ) sendercp6.c -lpcap -o ${EXEPATH}/sendercp6

${EXEPATH}/receivercp6        : receivercp6.c ${EXEPATH}
	$(CC) $(CFLAG) $(SRCOBJ) receivercp6.c -lpcap -o ${EXEPATH}/receivercp6

clean:
	-rm -r ${EXEPATH}