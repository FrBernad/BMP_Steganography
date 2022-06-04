# TARGET/MAIN FILES
export TARGET := stegobmp
MAIN := main

# DIRECTORIES
export SRC_DIR := $(CURDIR)/src

# FILE EXTENSIONS
export SRC_EXT := c
export OBJ_EXT := o

# SOURCE/OBJECT FILES
export SRC_FILES := $(shell find $(SRC_DIR) -type f -name *.$(SRC_EXT))
export OBJ_FILES := $(SRC_FILES:.$(SRC_EXT)=.$(OBJ_EXT))

# COMPILATION FLAGS
STD :=-std=c99
DEBUG_MEMORY := -g -fsanitize=address -pedantic -pedantic-errors
EXTRA_FLAGS := -DLOG_USE_COLOR
#OPTIMIZATION := O3
W_NO := -Wno-unused-parameter -Wno-newline-eof -Wno-implicit-fallthrough
W := -Wextra -Werror -Wall
export CFLAGS := $(W) $(W_NO) $(STD) $(EXTRA_FLAGS) $(DEBUG_MEMORY)

# MAIN DIRECTIVES
all:
	echo $(OBJ_FILES)
	cd $(SRC_DIR) && $(MAKE)

# CLEAN DIRECTIVES
clean:
	cd $(SRC_DIR) && $(MAKE) clean