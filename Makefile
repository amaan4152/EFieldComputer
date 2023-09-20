PROJ=$(shell pwd)
CXX=g++
CXXFLAGS=-lpthread -O3 -Wpedantic -Wall -pedantic-errors -Waddress -Wextra -Warray-bounds
LDFLAGS=-lpthread
BINFLAGS=-display
SRC=src
BIN=bin
EXEC_NAME=exec

SRC_FILES=$(wildcard $(SRC)/*cpp)
EXEC_FILES=$(EXEC_NAME:%=$(BIN)/%)
OBJ_FILES=$(SRC_FILES:%.cpp=$(BIN)/%.o)

.PHONY: pack all run link comp clean

build: $(EXEC_FILES)

clean:
	rm -rf $(BIN)

$(EXEC_FILES): $(OBJ_FILES)
	$(CXX) -I $(PROJ) $(LDFLAGS) -o $@ $^
	@echo "Build Successful!"

$(OBJ_FILES): $(BIN)/%.o : %.cpp
	@echo Compiling $<
	@mkdir -p $(@D)
	$(CXX) -I $(PROJ) -c $(CXXFLAGS) -o $@ $<
