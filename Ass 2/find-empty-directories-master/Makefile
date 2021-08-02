all: a.out

a.out: myfind.cpp Makefile
	g++ -O2 -Wall myfind.cpp

clean:
	rm -f *~ a.out

test: a.out
	./create.sh
	(cd dir1 && ../a.out)
