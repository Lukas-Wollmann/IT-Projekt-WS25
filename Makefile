CXX := clang++
CXXFLAGS := -std=c++23 -Wall -Wextra -Werror -Wno-error=unused-variable -Wno-error=unused-parameter -O0

SRC_DIR := src
TEST_DIR := test
OBJ_DIR := build
BIN_DIR := bin

TARGET := $(BIN_DIR)/app
TEST_TARGET := $(BIN_DIR)/test_runner

SRCS := $(shell find $(SRC_DIR) -name '*.cpp')
TEST_SRCS := $(filter-out $(SRC_DIR)/main.cpp, $(SRCS) $(shell find $(TEST_DIR) -name '*.cpp'))

OBJS := $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJS := $(TEST_SRCS:%.cpp=$(OBJ_DIR)/%.o)

all: $(TARGET) run

# Build the app
# Build the app
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

# Build the test_runner
$(TEST_TARGET): $(TEST_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Generic rule: compile any .cpp file
$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all run test clean
