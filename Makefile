.PHONY: all

all: 
	cd src; make -B; cd ..;

test:
	gcc -g -Isrc \
	build/link/device.o \
	build/link/ethheader.o \
	build/link/frame.o \
	build/link/link.o \
	build/network/arp.o \
	build/network/ipheader.o \
	build/network/ippacket.o \
	build/network/network.o \
	build/network/route.o \
	build/utils/debug.o \
	build/utils/error.o \
	build/utils/time.o \
	playground/main.c \
	-o playground/main \
	-lpcap 

clean:
	-rm -r build