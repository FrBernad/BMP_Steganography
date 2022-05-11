# MAIN DIRECTIVES
all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^

$(SRC_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.$(SRC_EXT)
	$(CC) $(CFLAGS) -c -o $@ $<


# CLEAN DIRECTIVES
clean:
	rm -rf $(OBJ_FILES) $(TARGET)

.PHONY: all clean