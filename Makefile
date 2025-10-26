# ===============================
#   Generic C++ Makefile (Clang)
# ===============================

# Compiler and flags
CXX := clang++
CXXFLAGS := -std=c++20 -Wall -Wextra -Werror -Wno-error=unused-variable -Wno-error=unused-parameter -O0

# Directories
SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin

# Target binary name
TARGET := $(BIN_DIR)/app

# Find all .cpp files recursively in src/
SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Default rule
all: $(TARGET) run

# Link object files into the final binary
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Phony targets
.PHONY: all clean run test
