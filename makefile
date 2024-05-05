all: 
	gcc -Wall -c ./bin/common.c -o ./bin/common.o
	gcc -Wall ./bin/client.c ./bin/common.o -o ./bin/client
	gcc -Wall ./bin/server.c ./bin/common.o -o ./bin/server -lm