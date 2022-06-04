# MAIN DIRECTIVES
all: $(TARGET)

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBRARY_FLAGS)

$(SRC_DIR)/%.$(OBJ_EXT): $(SRC_DIR)/%.$(SRC_EXT)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c -o $@ $< $(LIBRARY_FLAGS)


# CLEAN DIRECTIVES
clean:
	rm -rf $(OBJ_FILES) $(TARGET)

.PHONY: all clean