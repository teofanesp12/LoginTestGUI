FileC = login
CC = gcc
pgFalgs = -I /usr/include/postgresql -lpq
CFlags = `pkg-config --cflags --libs gtk+-3.0`



all:$(FileC)

$(FileC):$(FileC).o
	$(CC) $(FileC).o -o $(FileC) $(CFlags) $(pgFalgs)

$(FileC).o:$(FileC).c
	$(CC) -c $(FileC).c $(CFlags) $(pgFalgs)
	@echo "O Arquivo $(FileC).o está Pronto"

clean:
	rm $(FileC) $(FileC).o

run:$(FileC)
	./$(FileC)
