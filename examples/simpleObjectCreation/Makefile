#Makefile for Simple Object Creation reporter dylib


INCLUDE_DIR=./include
INCLUDE_DIR_INST=../../include
INCLUDE_JDK=/Library/Java/JavaVirtualMachines/jdk1.8.0_91.jdk/Contents/Home/include
INCLUDE_JDK_MD=/Library/Java/JavaVirtualMachines/jdk1.8.0_91.jdk/Contents/Home/include/darwin
LIB_DIR_INST=../../lib
SOURCE_DIR=./src
CFLAGS=-g -Wall -I$(INCLUDE_DIR) -I$(INCLUDE_DIR_INST) -I$(INCLUDE_JDK) -I$(INCLUDE_JDK_MD) -o
CFLAGS_LIB=-shared -fPIC

OUTPUT_DIR=obj
LIB_DIR=lib

CC=g++

OBJ_FILES=say_hello native_agent
OBJ_NAMES=$(patsubst %,$(OUTPUT_DIR)/%.o,$(OBJ_FILES))

all: create_dirs $(LIB_DIR)/libNativeAgent.dylib


$(OUTPUT_DIR)/say_hello.o: $(SOURCE_DIR)/say_hello.cpp $(INCLUDE_DIR)/say_hello.hpp
	$(CC) -c $< $(CFLAGS) $@

$(OUTPUT_DIR)/native_agent.o: $(SOURCE_DIR)/native_agent.cpp $(INCLUDE_DIR)/say_hello.hpp $(SOURCE_DIR)/say_hello.cpp
	$(CC) -c $< $(CFLAGS) $@

$(LIB_DIR)/libNativeAgent.dylib: $(OBJ_NAMES)
	$(CC) $(CFLAGS_LIB) $(OBJ_NAMES) -o $@

clean:
	rm -f $(OUTPUT_DIR)/*.o

create_dirs:
	mkdir -p $(OUTPUT_DIR)
	mkdir -p $(LIB_DIR)
