.PHONY: all

CC = gcc
CFLAG = -g -I../../../src
WRAPFLAG = \
-Wl,--wrap,socket       \
-Wl,--wrap,bind         \
-Wl,--wrap,listen       \
-Wl,--wrap,connect      \
-Wl,--wrap,accept       \
-Wl,--wrap,read         \
-Wl,--wrap,write        \
-Wl,--wrap,close        \
-Wl,--wrap,getaddrinfo  \
-Wl,--wrap,setsockopt   \

BUILDPATH = ../../../build
SRCOBJ = \
$(BUILDPATH)/utils/debug.o \
$(BUILDPATH)/utils/error.o \
$(BUILDPATH)/utils/time.o \
$(BUILDPATH)/utils/callbacklist.o\
$(BUILDPATH)/link/device.o \
$(BUILDPATH)/link/ethheader.o \
$(BUILDPATH)/link/frame.o \
$(BUILDPATH)/link/link.o \
$(BUILDPATH)/network/arp.o \
$(BUILDPATH)/network/ipheader.o \
$(BUILDPATH)/network/ippacket.o \
$(BUILDPATH)/network/network.o \
$(BUILDPATH)/network/route.o \
$(BUILDPATH)/transport/transport.o \
$(BUILDPATH)/transport/tcpheader.o \
$(BUILDPATH)/transport/loopthread.o \
$(BUILDPATH)/transport/mainthread.o \


EXEPATH = $(BUILDPATH)/test/lab3

all: ${EXEPATH}/echo_client ${EXEPATH}/echo_server ${EXEPATH}/perf_client ${EXEPATH}/slow_perf_client ${EXEPATH}/perf_server ${EXEPATH}/router

${EXEPATH} :
	mkdir -p  ${EXEPATH}

${EXEPATH}/echo_client        : echo_client.c unp.c ${EXEPATH}
	$(CC) $(CFLAG) $(WRAPFLAG) $(SRCOBJ) $(BUILDPATH)/transport/socket.o unp.c echo_client.c -lpcap -o ${EXEPATH}/echo_client

${EXEPATH}/echo_server        : echo_server.c unp.c ${EXEPATH}
	$(CC) $(CFLAG) $(WRAPFLAG) $(SRCOBJ) $(BUILDPATH)/transport/socket.o unp.c echo_server.c -lpcap -o ${EXEPATH}/echo_server

${EXEPATH}/perf_client        : perf_client.c unp.c ${EXEPATH}
	$(CC) $(CFLAG) $(WRAPFLAG) $(SRCOBJ) $(BUILDPATH)/transport/socket.o unp.c perf_client.c -lpcap -o ${EXEPATH}/perf_client

${EXEPATH}/perf_server        : perf_server.c unp.c ${EXEPATH}
	$(CC) $(CFLAG) $(WRAPFLAG) $(SRCOBJ) $(BUILDPATH)/transport/socket.o unp.c perf_server.c -lpcap -o ${EXEPATH}/perf_server

${EXEPATH}/slow_perf_client   : slow_perf_client.c unp.c ${EXEPATH}
	$(CC) $(CFLAG) $(WRAPFLAG) $(SRCOBJ) $(BUILDPATH)/transport/socket.o unp.c slow_perf_client.c -lpcap -o ${EXEPATH}/slow_perf_client

${EXEPATH}/router        : router.c ${EXEPATH}
	$(CC) $(CFLAG) $(SRCOBJ) router.c -lpcap -o ${EXEPATH}/router

clean:
	-rm -r ${EXEPATH}
