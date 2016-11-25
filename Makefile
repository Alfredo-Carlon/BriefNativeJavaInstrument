#Makefile for Basic Native Instrumentation dylib


INCLUDE_DIR=./include
SOURCE_DIR=./src
CFLAGS=-g -Wall -I$(INCLUDE_DIR) -o
CFLAGS_LIB=-dynamiclib -fPIC

OUTPUT_DIR=obj
LIB_DIR=lib

CC=g++

OBJ_FILES=ClassFile_def_imp ClassFile_FirstParser CodeMod_def_imp
OBJ_NAMES=$(patsubst %,$(OUTPUT_DIR)/%.o,$(OBJ_FILES))

all: create_dirs $(LIB_DIR)/libBNI.dylib


$(OUTPUT_DIR)/ClassFile_def_imp.o: $(SOURCE_DIR)/ClassFile_def_imp.cpp $(INCLUDE_DIR)/ClassFile_def.hpp $(INCLUDE_DIR)/ClassFile_errs.hpp
	$(CC) -c $< $(CFLAGS) $@

$(OUTPUT_DIR)/ClassFile_FirstParser.o: $(SOURCE_DIR)/ClassFile_FirstParser.cpp $(INCLUDE_DIR)/ClassFile_FirstParser.hpp $(INCLUDE_DIR)/ClassFile_errs.hpp
	$(CC) -c $< $(CFLAGS) $@

$(OUTPUT_DIR)/CodeMod_def_imp.o: $(SOURCE_DIR)/CodeMod_def_imp.cpp $(INCLUDE_DIR)/CodeMod_def.hpp
	$(CC) -c $< $(CFLAGS) $@

$(LIB_DIR)/libBNI.dylib: $(OBJ_NAMES)
	$(CC) $(CFLAGS_LIB) $(OBJ_NAMES) -o $@

clean:
	rm -f $(OUTPUT_DIR)/*.o

create_dirs:
	mkdir -p $(OUTPUT_DIR)
	mkdir -p $(LIB_DIR)
