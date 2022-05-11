include makefile.inc

export TARGET := stegobmp
export SRC_DIR := $(CURDIR)/src

MAIN := main

export SRC_EXT := c
export OBJ_EXT := o

export SRC_FILES := $(shell find $(SRC_DIR) -type f -name *.$(SRC_EXT) ! -name *$(MAIN).$(SRC_EXT))
export OBJ_FILES := $(SRC_FILES:.$(SRC_EXT)=.$(OBJ_EXT)) $(MAIN).$(OBJ_EXT)

all:
	cd $(SRC_DIR) && $(MAKE)

clean:
	cd $(SRC_DIR) && $(MAKE) clean