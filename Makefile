CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude

SRC = $(shell find src -name '*.c')
OBJ = $(SRC:.c=.o)

TARGET = jogo

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all run clean