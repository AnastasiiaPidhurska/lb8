CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c11 -pthread
LDFLAGS = -pthread

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

TARGET  = $(BIN_DIR)/producer_consumer

SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/buffer.c
OBJS = $(OBJ_DIR)/main.o $(OBJ_DIR)/buffer.o

.PHONY: all run clean dirs

all: dirs $(TARGET)

dirs:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

run: all
	$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
