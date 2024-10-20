# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I include -nostdlib -nostartfiles -fPIE -ffreestanding

# Directories
SRC_DIR = impl
OBJ_DIR = obj
LIB_NAME = target/spudlibc.a
CRT0_NAME = target/crt0.o

# Find all C source files in the SRC_DIR
SRC_FILES := $(shell find $(SRC_DIR) -name '*.c')
 
# Create a list of corresponding object files in the OBJ_DIR
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRC_FILES))

# Target: build the static library
$(LIB_NAME): $(OBJ_FILES)
	ar rvs $@ $^

# Rule to compile C source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean rule
.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(LIB_NAME) $(CRT0_NAME)

