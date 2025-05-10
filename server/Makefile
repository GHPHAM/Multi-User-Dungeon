CC = gcc
CFLAGS = -Wall -g

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
TARGET = game.exe

RM = del /Q

all: $(TARGET)
	@$(MAKE) clean-obj

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c dungeon.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(TARGET)

clean-obj:
	$(RM) $(OBJ)
