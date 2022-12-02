objects = dict.o list.o util.o parse.o htwatch.o config.o boot.o

build-server:build-server.o $(objects)
	gcc -o build-server build-server.o $(objects)

test: test.o $(objects)
	gcc -o test test.o $(objects)

clean:
	rm -fr test test.o build-server build-server.o $(objects)
