CC = gcc
FLAGS = -g -Wall -Werror
OBJ = lines server
LIBS = -lpthread

all: $(OBJ)
	@echo "------------------------------------"
	@echo "Build complete"
	@echo "------------------------------------"

lines: lines.c
	$(CC) $(FLAGS) -c lines.c

server: servidor.c
	$(CC) $(FLAGS) $(LIBS) -o server proxy.c servidor.c lines.c

load: 
	ld -o server lines.o

clean:
	rm -f *.o $(OBJ)
	@echo "------------------------------------"
	@echo "Clean complete"
	@echo "------------------------------------"