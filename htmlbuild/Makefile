C_FLAGS += -Wall -g
AR_FLAGS += -r


CC = $(MVTOOL_PREFIX)gcc $(INCLUDES) $(C_FLAGS) -c

objects = dict.o list.o util.o parse.o htwatch.o config.o boot.o file_mark.o
testsobjects = hello.o test_util.o test_mark.o

build-server:build-server.o $(objects)
	gcc -o build-server build-server.o $(objects)

test: test.o ${testsobjects} $(objects)
	gcc -o test test.o  $(objects) ${testsobjects}

clean:
	rm -fr test test.o build-server build-server.o $(objects) ${testsobjects}
