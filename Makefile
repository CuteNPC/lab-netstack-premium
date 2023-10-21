# Please forgive me, this Makefile is a bit messy (but it works).
# I will clean it up in the next version.

CC = gcc
CFLAG = -g -Isrc
HEADER = src/link/link.h src/link/frame.h src/link/ethheader.h src/link/device.h
TARGET = build/link/device.o build/link/frame.o build/link/link.o build/link/ethheader.o 

all: build/test/lab1/sender build/test/lab1/receiver build/test/lab1/detect

build/test/lab1/sender: $(TARGET) build/test/lab1/sender.o    build/test/lab1
	$(CC) $(CFLAG) $(TARGET) build/test/lab1/sender.o -o build/test/lab1/sender -lpcap

build/test/lab1/receiver: $(TARGET) build/test/lab1/receiver.o build/test/lab1
	$(CC) $(CFLAG) $(TARGET) build/test/lab1/receiver.o -o build/test/lab1/receiver -lpcap

build/test/lab1/detect: $(TARGET) build/test/lab1/detect.o  build/test/lab1
	$(CC) $(CFLAG) $(TARGET) build/test/lab1/detect.o -o build/test/lab1/detect -lpcap

build :
	mkdir -p build

build/link : build
	mkdir -p  build/link

build/test : build
	mkdir -p  build/test

build/test/lab1 : build/test
	mkdir -p  build/test/lab1

build/link/device.o       : $(HEADER) src/link/device.c   build/link
	$(CC) $(CFLAG) -c src/link/device.c -o build/link/device.o

build/link/frame.o        : $(HEADER) src/link/frame.c    build/link
	$(CC) $(CFLAG) -c src/link/frame.c -o build/link/frame.o

build/link/link.o         : $(HEADER) src/link/link.c     build/link
	$(CC) $(CFLAG) -c src/link/link.c -o build/link/link.o

build/link/ethheader.o    : $(HEADER) src/link/ethheader.c build/link
	$(CC) $(CFLAG) -c src/link/ethheader.c -o build/link/ethheader.o

build/test/lab1/sender.o    : $(HEADER) src/test/lab1/sender.c build/test/lab1
	$(CC) $(CFLAG) -c src/test/lab1/sender.c -o build/test/lab1/sender.o

build/test/lab1/receiver.o : $(HEADER) src/test/lab1/receiver.c build/test/lab1
	$(CC) $(CFLAG) -c src/test/lab1/receiver.c -o build/test/lab1/receiver.o

build/test/lab1/detect.o  : $(HEADER) src/test/lab1/detect.c build/test/lab1
	$(CC) $(CFLAG) -c src/test/lab1/detect.c -o build/test/lab1/detect.o

clean:
	-rm -r build