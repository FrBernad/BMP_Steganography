# MAIN FILES
export TARGET := stegobmp
MAIN := main

# DIRECTORIES
export SRC_DIR := $(CURDIR)/src


# FILE EXTENSIONS
export SRC_EXT := c
export OBJ_EXT := o

# FILES
export SRC_FILES := $(shell find $(SRC_DIR) -type f -name *.$(SRC_EXT) ! -name *$(MAIN).$(SRC_EXT))
export OBJ_FILES := $(SRC_FILES:.$(SRC_EXT)=.$(OBJ_EXT)) $(MAIN).$(OBJ_EXT)

# COMPILATION FLAGS
STD :=-std=c99
EXTRA_FLAGS := -g -fsanitize=address -pedantic -pedantic-errors
#OPTIMIZATION := O3
W_NO := -Wno-unused-parameter -Wno-newline-eof -Wno-implicit-fallthrough
W := -Wextra -Werror -Wall
export CFLAGS := $(W) $(W_NO) $(STD) $(EXTRA_FLAGS)

# MAIN DIRECTIVES
all:
	cd $(SRC_DIR) && $(MAKE)

# CLEAN DIRECTIVES
clean:
	cd $(SRC_DIR) && $(MAKE) clean