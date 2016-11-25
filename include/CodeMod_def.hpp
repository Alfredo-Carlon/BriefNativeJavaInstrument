
/*
 *  Definitions to parse and modify a Code attribute
 *  of a method. It modifies only parts of a Code attribute
 */
#ifndef CodeMod_def_h
#define CodeMod_def_h

#include <vector>
#include <ClassFile_def.hpp>

#pragma mark Instruction structure definition
/************************************************************
 *             Instructions structures definitions          *
 *                            Start                         *
 ***********************************************************/

typedef struct INSTRUCTION
{
    //Instruction's opcode
    u1 opcode;
    //Instruction's length, including opcode
    unsigned int length;
    //Extra data (if any)
    u1 *data;
    //The byte address of the instruction
    unsigned int address;
    //Empty Constructor
    INSTRUCTION();
    //Constructor from bytecode
    INSTRUCTION(const u1 *bytecode);
    //Destructor
    ~INSTRUCTION();
}INSTRUCTION;

/************************************************************
 *             Instructions structures definitions          *
 *                             End                          *
 ***********************************************************/
#pragma mark -
#pragma mark CodeMod

//Modifiable representation of a Code_attribute structure
typedef struct CodeMod
{
    bool valid;     //If set, the structure represents a valid method
    u2 max_stack;
    u2 max_locals;
    std::vector<INSTRUCTION *> *instructions;
    struct et
    {
        u2 start_pc;
        u2 end_pc;
        u2 handler_pc;
        u2 catch_type;
    };
    std::vector<CodeMod::et *> *exceptionsTable;
    std::vector<attribute_info *> *attributes;
    
    
    //Initializes an empty modCode
    CodeMod();
    //Initializes a modCode from bytecode
    CodeMod(const u1 *bytecode);
    //Initializes a modCode from Code_attribute
    CodeMod(Code_attribute origCode);
    ~CodeMod();
    
    //Initialize the structure with a Code_attribute
    void init(Code_attribute origCode);
    //Initialize the structure with u1 stream
    void init(const u1 *bytecode);
    //Writes this modCode to a Code_attribute
    void toCode_attribute(Code_attribute *tgCode);
    //Adds a new instruction at the index
    unsigned char add_instruction(INSTRUCTION *newInst, unsigned int index);
    
}CodeMod;
#pragma mark -

extern void (*__bytecodeInitFuns [202])(INSTRUCTION *,u1*,u1 **);
void INSTRUCTION_initialize_initFuns();

#endif /*CodeMod_def_h*/
