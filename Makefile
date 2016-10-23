#Makefile for Basic Native Instrumentation dylib


INCLUDE_DIR=./include
SOURCE_DIR=./src
CFLAGS=-g -Wall -I$(INCLUDE_DIR) -o
CFLAGS_LIB=-shared -fPIC

OUTPUT_DIR=obj
LIB_DIR=lib

CC=g++

OBJ_FILES=ClassFile_FirstParser ClassFile_def_imp
OBJ_NAMES=$(patsubst %,$(OUTPUT_DIR)/%.o,$(OBJ_FILES))


$(OUTPUT_DIR)/ClassFile_def_imp.o: $(SOURCE_DIR)/ClassFile_def_imp.cpp $(INCLUDE_DIR)/ClassFile_def.hpp $(INCLUDE_DIR)/ClassFile_errs.hpp
	$(CC) -c $< $(CFLAGS) $@


$(OUTPUT_DIR)/ClassFile_FirstParser.o: $(SOURCE_DIR)/ClassFile_FirstParser.cpp $(INCLUDE_DIR)/ClassFile_FirstParser.hpp $(INCLUDE_DIR)/ClassFile_errs.hpp
	$(CC) -c $< $(CFLAGS) $@


$(LIB_DIR)/libBNI.dylib: $(OBJ_NAMES)
	$(CC) $(CFLAGS_LIB) $(OBJ_NAMES) -o $@

clean:
	rm -f $(OUTPUT_DIR)/*.o

all: $(LIB_DIR)/libBNI.dylib
