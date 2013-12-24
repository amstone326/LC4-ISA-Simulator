all: trace

LC4.o: LC4.c LC4.h
	clang -g -c LC4.c

ObjectFiles.o: ObjectFiles.c ObjectFiles.h
	clang -g -c ObjectFiles.c

trace: trace.c ObjectFiles.o LC4.o
	clang -g -o trace trace.c ObjectFiles.o LC4.o

clean:
	rm *.o trace
