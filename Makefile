CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude
BIN_DIR = bin
TEST_SRC = tests/test.cpp
MAIN_SRC = main/Main.cpp
HEADERS = include/MyContainer.hpp
TEST_BIN = $(BIN_DIR)/test_bin
MAIN_BIN = $(BIN_DIR)/main_bin

all: test

test: $(TEST_BIN)
	./$(TEST_BIN)

Main: $(MAIN_BIN)
	./$(MAIN_BIN)

$(TEST_BIN): $(TEST_SRC) $(HEADERS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(TEST_SRC) -o $(TEST_BIN)

$(MAIN_BIN): $(MAIN_SRC) $(HEADERS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(MAIN_SRC) -o $(MAIN_BIN)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

valgrind: $(TEST_BIN) $(MAIN_BIN)
	valgrind --leak-check=full ./$(TEST_BIN)
	valgrind --leak-check=full ./$(MAIN_BIN)

clean:
	rm -rf $(BIN_DIR)

.PHONY: all test Main valgrind clean
