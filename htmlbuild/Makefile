objects =  dict.o list.o util.o parse.o

build-server:build-server.o $(objects)
	gcc -o build-server build-server.o $(objects)

clean:
	rm -fr test build-server build-server.o $(objects)


test: test.o $(objects)
	gcc -o test test.o $(objects) 
