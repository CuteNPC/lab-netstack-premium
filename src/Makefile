.PHONY: all

all: 
	mkdir -p ../build
	cd link; make -B; cd ..;
	cd network; make -B; cd ..;
	cd transport; make -B; cd ..;
	cd utils; make -B; cd ..;
	cd test; make -B; cd ..;

clean:
	-rm -r ../build