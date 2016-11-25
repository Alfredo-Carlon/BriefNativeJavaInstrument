#Makefile for Simple Object Creation reporter dylib


INCLUDE_DIR=./include
INCLUDE_DIR_INST=../../include
INCLUDE_JDK=/Library/Java/JavaVirtualMachines/jdk1.8.0_91.jdk/Contents/Home/include
INCLUDE_JDK_MD=/Library/Java/JavaVirtualMachines/jdk1.8.0_91.jdk/Contents/Home/include/darwin
LIB_DIR_INST=../../lib
SOURCE_DIR=./src
CFLAGS=-g -Wall -I$(INCLUDE_DIR) -I$(INCLUDE_DIR_INST) -I$(INCLUDE_JDK) -I$(INCLUDE_JDK_MD) -o
CFLAGS_LIB=-dynamiclib -fPIC -L$(LIB_DIR_INST) -lBNI


OUTPUT_DIR=obj
BIN_DIR=bin

CC=g++

OBJ_FILES=say_hello native_agent StaticClassTagger ObjCreationHistory
OBJ_NAMES=$(patsubst %,$(OUTPUT_DIR)/%.o,$(OBJ_FILES))

all: create_dirs $(BIN_DIR)/libNativeAgent.dylib change_instLib compile_java


$(OUTPUT_DIR)/say_hello.o: $(SOURCE_DIR)/say_hello.cpp $(INCLUDE_DIR)/say_hello.hpp
	$(CC) -c $< $(CFLAGS) $@

$(OUTPUT_DIR)/native_agent.o: $(SOURCE_DIR)/native_agent.cpp $(INCLUDE_DIR)/say_hello.hpp $(SOURCE_DIR)/say_hello.cpp
	$(CC) -c $< $(CFLAGS) $@

$(OUTPUT_DIR)/StaticClassTagger.o: $(SOURCE_DIR)/StaticClassTagger.cpp $(INCLUDE_DIR)/StaticClassTagger.hpp
	$(CC) -c $< $(CFLAGS) $@

$(OUTPUT_DIR)/ObjCreationHistory.o: $(SOURCE_DIR)/ObjCreationHistory.cpp $(INCLUDE_DIR)/ObjCreationHistory.hpp
	$(CC) -c $< $(CFLAGS) $@

$(BIN_DIR)/libNativeAgent.dylib: $(OBJ_NAMES)
	$(CC) $(CFLAGS_LIB) $(OBJ_NAMES) -o $@

clean:
	rm -f $(OUTPUT_DIR)/*.o

create_dirs:
	mkdir -p $(OUTPUT_DIR)
	mkdir -p $(BIN_DIR)

compile_java:
	javac -d $(BIN_DIR) $(SOURCE_DIR)/Test.java

#Changes the instalation dir of libBNI.dylib in libNativeAgent.dylib (tmp patch)
change_instLib:
	install_name_tool -change lib/libBNI.dylib ../$(LIB_DIR_INST)/libBNI.dylib $(BIN_DIR)/libNativeAgent.dylib