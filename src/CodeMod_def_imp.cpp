/*
 *  Implementation of CodeMod_def
 *
 */

#include <CodeMod_def.hpp>

/*************************************************
 *                                               *
 * INSTRUCTION implementation Start              *
 *                                               *
 *************************************************/

INSTRUCTION::INSTRUCTION()
{
    opcode  = 0;
    length  = 0;
    data    = NULL;
}
INSTRUCTION::~INSTRUCTION()
{
    if (data) {
        free(data);
        data = NULL;
    }
}
INSTRUCTION::INSTRUCTION(const u1 *bytecode)
{
    u1 *bytec_ptr = (u1 *)bytecode;
    opcode = *bytec_ptr;
    bytec_ptr++;
    
}

/*************************************************
 *                                               *
 * INSTRUCTION implementation End                *
 *                                               *
 *************************************************/

/*************************************************
 *                                               *
 * Bytecode init implementation Start            *
 *                                               *
 *************************************************/

void __copy_k_bytes(INSTRUCTION *inst, u1 **bytecode, unsigned int size)
{
    inst ->opcode = **bytecode;
    (*bytecode)++;
    inst ->length = size + 1;
    inst ->data = (u1 *)malloc(sizeof(u1)*size);
    memcpy(inst ->data,*bytecode, sizeof(u1)*size);
    *bytecode += size;
}

void __copy_lookupswitch(INSTRUCTION *inst, u1 *base_ptr, u1 **bytecode)
{
    inst ->opcode = **bytecode;
    //Calculate the padding
    unsigned char offset = 4 - ((*bytecode - base_ptr) % 4);
    
    (*bytecode) += offset;
    
    u4 defaultByte = u4(*bytecode);
    (*bytecode) += 4;
    u4 npairs = u4(*bytecode);
    (*bytecode) += 4;
    inst ->length = offset + 8*(npairs + 1);
    inst ->data = (u1 *)malloc(inst ->length - 1);
    u1 *data_ptr = inst ->data;
    data_ptr += offset - 1;
    defaultByte.writeToArray(data_ptr);
    data_ptr += 4;
    npairs.writeToArray(data_ptr);
    data_ptr += 4;
    memcpy(data_ptr, *bytecode, npairs*8);
    (*bytecode) += npairs*8;
    
    /*
    for(unsigned int i=0; i != npairs; i++){
        //Read the pairs
        u4 p = u4(*bytecode);
        (*bytecode) += 4;
        p.writeToArray(data_ptr);
        data_ptr += 4;
        p = u4(*bytecode);
        (*bytecode) += 4;
        p.writeToArray(data_ptr);
        data_ptr += 4;
    }*/
    
    
}

void __copy_tableswitch(INSTRUCTION *inst, u1 *base_ptr, u1 **bytecode)
{
    inst ->opcode = **bytecode;
    //Calculate the padding
    unsigned char offset = 4 - ((*bytecode - base_ptr) % 4);
    
    (*bytecode) += offset;
    u4 defByte = u4(*bytecode);
    (*bytecode) += 4;
    u4 lb = u4(*bytecode);
    (*bytecode) += 4;
    u4 hb = u4(*bytecode);
    (*bytecode) += 4;
    
    inst ->length = 13 + offset-1 + (hb - lb + 1)*4;
    inst ->data = (u1 *)malloc(12 + offset - 1 + (hb - lb + 1)*4);
    bzero(inst ->data, offset - 1);
    
    u1 *data_ptr = inst ->data + offset - 1;
    defByte.writeToArray(data_ptr);
    data_ptr += 4;
    lb.writeToArray(data_ptr);
    data_ptr += 4;
    hb.writeToArray(data_ptr);
    data_ptr += 4;
    memcpy(data_ptr, *bytecode, (hb - lb + 1)*4);
    (*bytecode) += (hb - lb + 1)*4;
    /*
    for(unsigned int i=0; i != hb - lb + 1 ;i++)
    {
        u4 b = u4(*bytecode);
        (*bytecode) += 4;
        b.writeToArray(data_ptr);
        data_ptr += 4;
    }*/
    
}

void __copy_wide(INSTRUCTION *inst, u1 *base_ptr, u1 **bytecode)
{
    inst ->opcode = **bytecode;
    (*bytecode) += 1;
    //Read the next opcode
    if(**bytecode == INSTRUCTION_iinc){
        inst ->length = 6;
        inst ->data = (u1 *)malloc(5);
        memcpy(inst->data, *bytecode, 5);
        (*bytecode) += 5;
        return;
    }
    inst ->length = 4;
    inst ->data = (u1 *)malloc(3);
    memcpy(inst ->data, *bytecode, 3);
    (*bytecode) += 3;
}

void __copy_0_bytes(INSTRUCTION *inst, u1 *base_ptr, u1 **bytecode)
{
    inst ->opcode = **bytecode;
    inst ->length = 1;
    inst ->data = NULL;
    (*bytecode)++;
}

void __copy_1_bytes(INSTRUCTION *inst, u1 *base_ptr, u1 **bytecode)
{
    __copy_k_bytes(inst, bytecode, 1);
}
void __copy_2_bytes(INSTRUCTION *inst, u1 *base_ptr,u1 **bytecode)
{
    __copy_k_bytes(inst, bytecode, 2);
}
void __copy_3_bytes(INSTRUCTION *inst, u1 *base_ptr,u1 **bytecode)
{
    __copy_k_bytes(inst, bytecode, 3);
}
void __copy_4_bytes(INSTRUCTION *inst, u1 *base_ptr,u1 **bytecode)
{
    __copy_k_bytes(inst, bytecode, 4);
}

void (*__bytecodeInitFuns [202])(INSTRUCTION *,u1 *,u1 **);



void INSTRUCTION_initialize_initFuns()
{
    for(unsigned int i=0; i != 201; i++){
        __bytecodeInitFuns[i] = &__copy_0_bytes;
    }
    __bytecodeInitFuns[0x19] = &__copy_1_bytes;
    __bytecodeInitFuns[0xbd] = &__copy_2_bytes;
    __bytecodeInitFuns[0x3a] = &__copy_1_bytes;
    __bytecodeInitFuns[0x10] = &__copy_1_bytes;
    __bytecodeInitFuns[0xc0] = &__copy_2_bytes;
    __bytecodeInitFuns[0x18] = &__copy_1_bytes;
    __bytecodeInitFuns[0x39] = &__copy_1_bytes;
    __bytecodeInitFuns[0x17] = &__copy_1_bytes;
    __bytecodeInitFuns[0x38] = &__copy_1_bytes;
    __bytecodeInitFuns[0xb4] = &__copy_2_bytes;
    __bytecodeInitFuns[0xb2] = &__copy_2_bytes;
    __bytecodeInitFuns[0xa7] = &__copy_2_bytes;
    __bytecodeInitFuns[0xc8] = &__copy_4_bytes;
    __bytecodeInitFuns[0xa5] = &__copy_2_bytes;
    __bytecodeInitFuns[0xa6] = &__copy_2_bytes;
    __bytecodeInitFuns[0x9f] = &__copy_2_bytes;
    __bytecodeInitFuns[0xa2] = &__copy_2_bytes;
    __bytecodeInitFuns[0xa3] = &__copy_2_bytes;
    __bytecodeInitFuns[0xa4] = &__copy_2_bytes;
    __bytecodeInitFuns[0xa1] = &__copy_2_bytes;
    __bytecodeInitFuns[0xa0] = &__copy_2_bytes;
    __bytecodeInitFuns[0x99] = &__copy_2_bytes;
    __bytecodeInitFuns[0x9c] = &__copy_2_bytes;
    __bytecodeInitFuns[0x9d] = &__copy_2_bytes;
    __bytecodeInitFuns[0x9e] = &__copy_2_bytes;
    __bytecodeInitFuns[0x9b] = &__copy_2_bytes;
    __bytecodeInitFuns[0x9a] = &__copy_2_bytes;
    __bytecodeInitFuns[0xc7] = &__copy_2_bytes;
    __bytecodeInitFuns[0xc6] = &__copy_2_bytes;
    __bytecodeInitFuns[0x84] = &__copy_2_bytes;
    __bytecodeInitFuns[0x15] = &__copy_1_bytes;
    __bytecodeInitFuns[0xc1] = &__copy_2_bytes;
    __bytecodeInitFuns[0xba] = &__copy_4_bytes;
    __bytecodeInitFuns[0xb9] = &__copy_4_bytes;
    __bytecodeInitFuns[0xb7] = &__copy_2_bytes;
    __bytecodeInitFuns[0xb8] = &__copy_2_bytes;
    __bytecodeInitFuns[0xb6] = &__copy_2_bytes;
    __bytecodeInitFuns[0x36] = &__copy_1_bytes;
    __bytecodeInitFuns[0xa8] = &__copy_2_bytes;
    __bytecodeInitFuns[0xc9] = &__copy_4_bytes;
    __bytecodeInitFuns[0x12] = &__copy_1_bytes;
    __bytecodeInitFuns[0x13] = &__copy_2_bytes;
    __bytecodeInitFuns[0x14] = &__copy_2_bytes;
    __bytecodeInitFuns[0x16] = &__copy_1_bytes;
    __bytecodeInitFuns[0xab] = &__copy_lookupswitch;
    __bytecodeInitFuns[0x37] = &__copy_1_bytes;
    __bytecodeInitFuns[0xc5] = &__copy_3_bytes;
    __bytecodeInitFuns[0xbb] = &__copy_2_bytes;
    __bytecodeInitFuns[0xbc] = &__copy_1_bytes;
    __bytecodeInitFuns[0xb5] = &__copy_2_bytes;
    __bytecodeInitFuns[0xb3] = &__copy_2_bytes;
    __bytecodeInitFuns[0xa9] = &__copy_1_bytes;
    __bytecodeInitFuns[0x11] = &__copy_2_bytes;
    __bytecodeInitFuns[0xaa] = &__copy_tableswitch;
    __bytecodeInitFuns[0xc4] = &__copy_wide;
}





CodeMod::CodeMod()
{
    valid               = false;
    max_stack           = u2((unsigned short)0);
    max_locals          = u2((unsigned short)0);
    
    instructions        = NULL;
    exceptionsTable     = NULL;
    attributes          = NULL;
}

CodeMod::CodeMod(Code_attribute atrib)
{
    this ->init(atrib);
}

//Creates the next instruction from the bytecode
//base_ptr is only used for lookupswitch and tableswitch
//to calculate the padding.
INSTRUCTION *CodeMod__getNextInstruction(u1 *base_ptr, u1 **code_ptr)
{
    INSTRUCTION *inst = new INSTRUCTION();
    __bytecodeInitFuns[**code_ptr](inst, base_ptr, code_ptr);
    return inst;
}

CodeMod::CodeMod(const u1 *bytecode)
{
    this ->init(bytecode);
    
}

void CodeMod::init(const u1 *bytecode)
{
    u1 *bytecode_ptr = (u1 *)bytecode;
    max_stack = u2(bytecode_ptr);
    bytecode_ptr += 2;
    max_locals = u2(bytecode_ptr);
    bytecode_ptr += 2;
    u4 codeLen = u4(bytecode_ptr);
    bytecode_ptr += 4;
    instructions = new std::vector<INSTRUCTION *>();
    unsigned int curntAdd = 0;
    while(curntAdd < codeLen){
        INSTRUCTION *newInst = CodeMod__getNextInstruction((u1 *)(bytecode+8),
                                                           &bytecode_ptr);
        newInst ->address = curntAdd;
        curntAdd += newInst ->length;
        instructions ->push_back(newInst);
    }
    //Fill the exceptions
    u2 numExcep = u2(bytecode_ptr);
    bytecode_ptr += 2;
    if (!numExcep) {
        exceptionsTable = NULL;
        return;
    }else{
        exceptionsTable = new std::vector<CodeMod::et *>();
        for(unsigned short i=0; i != numExcep; i++){
            CodeMod::et *except = new CodeMod::et();
            except->start_pc = u2(bytecode_ptr);
            bytecode_ptr += 2;
            except->end_pc = u2(bytecode_ptr);
            bytecode_ptr += 2;
            except->handler_pc = u2(bytecode_ptr);
            bytecode_ptr += 2;
            except->catch_type = u2(bytecode_ptr);
            bytecode_ptr += 2;
            exceptionsTable ->push_back(except);
        }
    }
    u2 attribsSize = u2(bytecode_ptr);
    bytecode_ptr += 2;
    if(attribsSize.uShort() != 0){
        attributes = new std::vector<attribute_info *>();
        for(unsigned int i=0; i != attribsSize; i++){
            //copy each attribute
            attribute_info *newAttrib = new attribute_info();
            newAttrib ->attribute_name_index = u2(bytecode_ptr);
            bytecode_ptr += 2;
            newAttrib ->attribute_length = u4(bytecode_ptr);
            bytecode_ptr += 4;
            newAttrib ->info = (u1 *)malloc(newAttrib ->attribute_length);
            memcpy(newAttrib ->info, bytecode_ptr, newAttrib ->attribute_length);
            bytecode_ptr += newAttrib ->attribute_length;
            attributes ->push_back(newAttrib);
        }
    }
}

void CodeMod::init(Code_attribute origCode)
{
    max_stack = origCode.max_stack;
    max_locals = origCode.max_locals;
    
    //For the momment we do not modify attributes
    //So we ignore them
    attributes = NULL;
    
    instructions = new std::vector<INSTRUCTION *>();
    u1 *code_ptr = origCode.code;
    for(unsigned int i=0; i != origCode.code_length; i++)
    {
        INSTRUCTION *newInst = CodeMod__getNextInstruction(origCode.code,
                                                           &code_ptr);
        instructions ->push_back(newInst);
    }
    if(!origCode.exception_table_length){
        exceptionsTable = NULL;
        return;
    }
    
}
//Adds a new instruction at the index
unsigned char CodeMod::add_instruction(INSTRUCTION *newInst, unsigned int index)
{
    
    unsigned char nopPaddingSize = (4 - (newInst ->length % 4));
    
    
    unsigned int startByte = (*(instructions ->begin() + index))->address;
    newInst ->address = startByte;
    
    instructions ->insert(instructions ->begin()+index, newInst);
    unsigned startAdd = newInst ->address + newInst ->length;
    
    
    for(unsigned int i=0; i != nopPaddingSize; i++){
        INSTRUCTION *pad = new INSTRUCTION();
        pad ->opcode = INSTRUCTION_nop;
        pad ->length = 1;
        pad ->data = NULL;
        pad ->address = startAdd;
        startAdd++;
        instructions ->insert(instructions ->begin()+index+i+1, pad);
    }
    //All the addresses are correct up to newInst ->address + nopPaddingSize
    //Shift the addresses by length + nopPaddingSize
    std::vector<INSTRUCTION *>::iterator inst_it = instructions ->begin() +
    index+nopPaddingSize+1;
    
    
    
    while(inst_it != instructions ->end()){
        (*inst_it) ->address = startAdd;
        startAdd += (*inst_it) ->length;
        inst_it++;
    }
    
    //Sanity check exception table update
    //Exceptions shift
    if(!exceptionsTable){
        return newInst ->length + nopPaddingSize;
    }
    std::vector<CodeMod::et*>::iterator except_it = exceptionsTable ->begin();
    while (except_it != exceptionsTable ->end()) {
        if (newInst ->address <= (*except_it) ->start_pc) {
            (*except_it) ->start_pc     += newInst ->length + nopPaddingSize;
            (*except_it) ->end_pc       += newInst ->length + nopPaddingSize;
        }else if(newInst ->address < (*except_it) ->end_pc){
            (*except_it) ->end_pc       += newInst ->length + nopPaddingSize;
        }
        if (newInst ->address <= (*except_it) ->handler_pc) {
            (*except_it) ->handler_pc   += newInst ->length + nopPaddingSize;
        }
        except_it++;
    }
    return newInst ->length + nopPaddingSize;
}

CodeMod::~CodeMod()
{
    if (instructions) {
        std::vector<INSTRUCTION *>::iterator inst_it = instructions->begin();
        while (inst_it != instructions ->end()) {
            delete *inst_it;
            inst_it++;
        }
        delete instructions;
        instructions = NULL;
    }
    if(exceptionsTable){
        delete exceptionsTable;
        exceptionsTable = NULL;
    }
    if(attributes){
        std::vector<attribute_info *>::iterator attr_it = attributes ->begin();
        while(attr_it != attributes ->end()){
            delete *attr_it;
            attr_it ++;
        }
        delete attributes;
        attributes = NULL;
    }
}
