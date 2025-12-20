LLVM_CONFIG := llvm-config

LLVM_CXXFLAGS := $(shell $(LLVM_CONFIG) --cxxflags)
LLVM_LDFLAGS  := $(shell $(LLVM_CONFIG) --ldflags)
LLVM_LIBS     := $(shell $(LLVM_CONFIG) --libs all)

CXX := clang++

CXXFLAGS := -O0 \
            $(LLVM_CXXFLAGS) \
            -Wall -Wextra -Werror \
            -Wno-error=unused-parameter \
            -Wno-error=unused-variable \
            -Wno-error=deprecated-declarations \
            -std=c++20 \
            -fexceptions \
            -stdlib=libc++


SRC_DIR := src
LIB_DIR := lib
TEST_DIR := test

OBJ_DIR := build
BIN_DIR := bin

INCLUDES := -I$(SRC_DIR) -I$(TEST_DIR) -I$(LIB_DIR)

TARGET := $(BIN_DIR)/app
TEST_TARGET := $(BIN_DIR)/test_runner

SRCS := $(shell find $(SRC_DIR) -name '*.cpp') \
        $(shell find $(LIB_DIR) -name '*.cpp')

TEST_SRCS := $(filter-out $(SRC_DIR)/main.cpp, \
             $(SRCS) $(shell find $(TEST_DIR) -name '*.cpp'))

OBJS := $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJS := $(TEST_SRCS:%.cpp=$(OBJ_DIR)/%.o)

# ---------- build rules ----------

all: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $^ $(LLVM_LDFLAGS) $(LLVM_LIBS) -stdlib=libc++ -lc++ -lc++abi

$(TEST_TARGET): $(TEST_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CXX) -o $@ $^ $(LLVM_LDFLAGS) $(LLVM_LIBS) -stdlib=libc++ -lc++ -lc++abi


$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all run test clean
