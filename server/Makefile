CC = gcc
CFLAGS = -Wall -g

SRC = main.c level1.c
OBJ = $(SRC:.c=.o)
TARGET = game.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c dungeon.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)