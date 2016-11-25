//
//  ClassFile_FirstParser.cpp
//
//
//  Created by Alfredo Cruz on 9/30/16.
//
//

#include <ClassFile_FirstParser.hpp>
#include <CodeMod_def.hpp>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>


#define EXPORT __attribute__((visibility("default")))

#pragma mark Holders
std::vector<method_info *> *methodsHolder(ClassFile *_cf)
{
    return (std::vector<method_info *> *)_cf->methods;
}
std::vector<field_info *> *fieldsHolder(ClassFile *_cf)
{
    return (std::vector<field_info *> *)_cf->fields;
}
std::vector<cp_info> *constantPoolHolder(ClassFile *_cf)
{
    return (std::vector<cp_info> *)_cf ->constant_pool;
}
#pragma mark -

#pragma mark Private Methods Imp


unsigned char *ClassFile_FP::_fillFieldAttribute(field_info **fi,
                                                 unsigned short fi_count,
                                                 unsigned char *payload_ptr)
{
    //Fetch the fields
    for(unsigned short i=0; i != fi_count; i++){
        fi[i] = new field_info();
        //For each fiel, fill the field_info structure
        memcpy(fi[i],payload_ptr,sizeof(u2)*4);
        payload_ptr += sizeof(u2)*4;
        //Get the attributes_count to see if we have more attributes
        unsigned short attributes_count =
        fi[i] -> attributes_count;
        if(!attributes_count){
            fi[i]->attributes = NULL;
            continue;
        }
        //We have more attributes, so we fetch them
        //Fetch the attributes
        for(unsigned short j=0; j != attributes_count; j++){
            attribute_info newAttr;
            newAttr.attribute_name_index = u2(payload_ptr);
            payload_ptr += 2;
            newAttr.attribute_length = u4(payload_ptr);
            payload_ptr += 4;
            unsigned int attribute_length =
            newAttr.attribute_length;
            //Allocate memory for the attribute's data
            newAttr.info =
            (u1 *)malloc(sizeof(u1)*attribute_length);
            if(!newAttr.info)
                throw NoMemoryError();
            memcpy(newAttr.info,
                   payload_ptr,
                   sizeof(u1)*attribute_length);
            payload_ptr += attribute_length;
            fi[i]->addNewAttribute(newAttr, j);
        }
    }
    return payload_ptr;
}


unsigned char *ClassFile_FP::_fillMethodAttribute(method_info **fi,
                                                  unsigned short fi_count,
                                                  unsigned char *payload_ptr)
{
    //Fetch the fields
    for(unsigned short i=0; i != fi_count; i++){
        fi[i] = new method_info();
        //For each fiel, fill the field_info structure
        memcpy(fi[i],payload_ptr,sizeof(u2)*4);
        payload_ptr += sizeof(u2)*4;
        
        //Check if the method has attributes
        if(!fi[i] -> attributes_count){
            fi[i]->attributes = NULL;
            continue;
        }
        //We have attributes
        //Fetch the attributes
        for(unsigned short j=0; j != fi[i] -> attributes_count; j++){
            attribute_info newAttr;
            memcpy(&newAttr,payload_ptr,sizeof(u2)+sizeof(u4));
            payload_ptr += 6;
            //Allocate memory for the attribute's data
            newAttr.info =
            (u1 *)malloc(sizeof(u1)*newAttr.attribute_length);
            if(!newAttr.info)
                throw NoMemoryError();
            memcpy(newAttr.info,
                   payload_ptr,
                   sizeof(u1)*newAttr.attribute_length);
            payload_ptr += newAttr.attribute_length;
            fi[i]->addNewAttribute(newAttr, j);
        }
    }
    return payload_ptr;
}

u2 ClassFile_FP::_push_into_constant_pool(cp_info& data, bool updateSize)
{
    ((std::vector<cp_info> *)(_classFile->constant_pool))->push_back(data);
    _constant_pool_next_index++;
    //Check if we have to push a dummy slot just to
    //align the count of our constant pool with that of the
    //bytecode's
    if (data.tag == CONSTANT_Double ||
        data.tag == CONSTANT_Long) {
        ((std::vector<cp_info> *)(_classFile->constant_pool))->push_back(data);
        _constant_pool_next_index++;
    }
    //Update the bytecode size
    if(updateSize)
        _size_in_bytes += sizeof(u1); //cp_info tag size
    switch (data.tag) {
        case CONSTANT_Class:
            if(updateSize)
                _size_in_bytes += ((CONSTANT_Class_info *)(data.info)) ->size();
            break;
        case CONSTANT_Fieldref:
            if(updateSize)
                _size_in_bytes += ((CONSTANT_Fieldref_info *)
                                   (data.info)) ->size();
            break;
        case CONSTANT_Methodref:
            if(updateSize)
                _size_in_bytes += ((CONSTANT_Methodref_info *)
                                   (data.info)) ->size();
            break;
        case CONSTANT_InterfaceMethodref:
            if(updateSize)
                _size_in_bytes += ((CONSTANT_InterfaceMethodref_info *)
                               (data.info)) ->size();
            break;
        case CONSTANT_String:
            if(updateSize)
                _size_in_bytes += ((CONSTANT_String_info *)
                                   (data.info)) ->size();
            break;
        case CONSTANT_Integer:
            if(updateSize)
            _size_in_bytes += ((CONSTANT_Integer_info *)
                               (data.info)) ->size();
            break;
        case CONSTANT_Float:
            if(updateSize)
            _size_in_bytes += ((CONSTANT_Float_info *)
                               (data.info)) ->size();
            break;
        case CONSTANT_Long:
            if(updateSize)
                _size_in_bytes += ((CONSTANT_Long_info *)
                                   (data.info)) ->size();
            break;
        case CONSTANT_Double:
            if(updateSize)
                _size_in_bytes += ((CONSTANT_Double_info *)
                                   (data.info)) ->size();
            break;
        case CONSTANT_NameAndType:
            if(updateSize)
                _size_in_bytes += ((CONSTANT_NameAndType_info *)
                               (data.info)) ->size();
            break;
        case CONSTANT_Utf8:
            if(updateSize)
                _size_in_bytes += ((CONSTANT_Utf8_info *)(data.info)) ->size();
            break;
        case CONSTANT_MethodHandle:
            if(updateSize)
                _size_in_bytes += ((CONSTANT_MethodHandle_info *)
                               (data.info)) ->size();
            break;
        case CONSTANT_MethodType:
            if(updateSize)
                _size_in_bytes += ((CONSTANT_MethodType_info *)
                               (data.info)) ->size();
            break;
        case CONSTANT_InvokeDynamic:
            if(updateSize)
                _size_in_bytes += ((CONSTANT_InvokeDynamic_info *)
                               (data.info)) ->size();
            break;
        default:
            throw ConstantPool_TagNotRecognized();
            break;
    }
    return _constant_pool_next_index - 1;
}

bool ClassFile_FP::_cpEntryMatches(unsigned short entry, const char *str)
{
    std::vector<cp_info> *cp = constantPoolHolder(this ->_classFile);
    cp_info cpentry = cp ->at(entry-1);
    if(cpentry.tag != CONSTANT_Utf8)
        return false;
    CONSTANT_Utf8_info *info = (CONSTANT_Utf8_info *)(cpentry.info);
    if(strlen(str) != info ->length)
        return false;
    //check each character
    for(unsigned short i=0; i != info ->length; i++)
        if (str[i] != info ->bytes[i]) {
            return false;
        }
    return true;
}

void ClassFile_FP::_loadCodeModForMethod(unsigned short methodIndex)
{
    if(_modifiedMethods[methodIndex].valid)
        return;
    //Get the method at index
    std::vector<method_info *> *methods = methodsHolder(this ->_classFile);
    if (!(methodIndex < methods ->size())) {
        throw InvalidMethodIndex();
    }
    method_info *method = methods ->at(methodIndex);
    //For the method look the Code attribute
    Code_attribute code;
    u1 *attribInfo = NULL;
    unsigned short attribIndex = -1;
    for(unsigned short i=0; i != method ->attributes_count; i++){
        if(_cpEntryMatches(method ->attributes[i].attribute_name_index,
                           "Code")){
            attribInfo = (method ->attributes[i].info);
            attribIndex = i;
            break;
        }
    }
    
    //Check if there is a "Code" attribe
    if(!attribInfo)
        return; //Shouldn't happen
    _modifiedMethods[methodIndex].init(attribInfo);
    _modifiedMethods[methodIndex].valid = true;
}
void ClassFile_FP::_saveCodeModeForAllMethods()
{
    std::vector<method_info *> *methods = methodsHolder(this ->_classFile);
    //Iterate over all mod methods and save the valid ones
    for(unsigned int i=0; i != _modifiedMethodsSize; i++){
        if(_modifiedMethods[i].valid){
            //Dump the modified version
            //Get the code attribute
            method_info *method = methods ->at(i);
            
            //Get the total size of the code attribute and allocate a buffer
            unsigned int attribSize = 12; //Base size from u2 and u4 types
            INSTRUCTION *lastInst = _modifiedMethods[i].instructions ->back();
            attribSize += lastInst ->address + lastInst ->length;
            if(_modifiedMethods[i].exceptionsTable){
                attribSize += _modifiedMethods[i].exceptionsTable ->size()*8;
            }
            if(_modifiedMethods[i].attributes){
                std::vector<attribute_info *>::iterator att_it =
                _modifiedMethods[i].attributes ->begin();
                while (att_it != _modifiedMethods[i].attributes ->end()) {
                    attribSize += 6;
                    attribSize += (*att_it) ->attribute_length;
                    att_it++;
                }
            }
            u1 *attribBuffer = NULL;
            for(unsigned short i=0; i != method ->attributes_count; i++){
                if(_cpEntryMatches(method ->attributes[i].attribute_name_index,
                                   "Code")){
                    free(method ->attributes[i].info);
                    method ->attributes[i].attribute_length = u4(attribSize);
                    method ->attributes[i].info = (u1 *)malloc(attribSize);
                    attribBuffer = method ->attributes[i].info;
                    break;
                }
            }
            
            //Copy the data
            attribBuffer = _modifiedMethods[i].max_stack.
            writeToArray(attribBuffer);
            attribBuffer = _modifiedMethods[i].max_locals.
            writeToArray(attribBuffer);
            attribBuffer = u4((unsigned int)
                              (lastInst ->address + lastInst ->length)).
            writeToArray(attribBuffer);
            
            
            //Copy the code portion
            std::vector<INSTRUCTION *>::iterator inst_it =
            _modifiedMethods[i].instructions ->begin();
            while (inst_it != _modifiedMethods[i].instructions ->end()) {
                *attribBuffer = (*inst_it)->opcode;
                attribBuffer++;
                if((*inst_it) ->length != 1){
                    memcpy(attribBuffer,
                           (*inst_it) ->data,
                           (*inst_it) ->length -1);
                    attribBuffer += (*inst_it)->length - 1;
                }
                inst_it++;
            }
            
            //The exceptions table does not change in numbers, however
            //the indices may have been shifted
            if(_modifiedMethods[i].exceptionsTable){
                attribBuffer = u2((unsigned short)(_modifiedMethods[i].
                                                   exceptionsTable ->size())).
                                                writeToArray(attribBuffer);
                std::vector<CodeMod::et *>::iterator except_it =
                _modifiedMethods[i].exceptionsTable ->begin();
                while(except_it != _modifiedMethods[i].exceptionsTable ->end()){
                    attribBuffer = u2((*except_it) ->start_pc).
                    writeToArray(attribBuffer);
                    attribBuffer = u2((*except_it) ->end_pc).
                    writeToArray(attribBuffer);
                    attribBuffer = u2((*except_it) ->handler_pc).
                    writeToArray(attribBuffer);
                    attribBuffer = u2((*except_it) ->catch_type).
                    writeToArray(attribBuffer);
                    except_it++;
                }
            }else{
                attribBuffer = u2((unsigned short)0).writeToArray(attribBuffer);
            }
            //Copy the code's attributes
            
            if(_modifiedMethods[i].attributes){
                attribBuffer = u2((unsigned short)(_modifiedMethods[i].
                                                   attributes ->size())).
                                writeToArray(attribBuffer);
                std::vector<attribute_info *>::iterator att_it =
                _modifiedMethods[i].attributes ->begin();
                while (att_it != _modifiedMethods[i].attributes ->end()) {
                    attribBuffer = (*att_it) ->attribute_name_index.
                    writeToArray(attribBuffer);
                    attribBuffer = (*att_it) ->attribute_length.
                    writeToArray(attribBuffer);
                    memcpy(attribBuffer, (*att_it) ->info,
                           (*att_it) ->attribute_length);
                    attribBuffer += (*att_it) ->attribute_length;
                    att_it++;
                }
            }else{
                attribBuffer = u2((unsigned short)0).writeToArray(attribBuffer);
            }
            /*printf("\n#######################################\n\n");
            for(unsigned short i=0; i != method ->attributes_count; i++){
                if(_cpEntryMatches(method ->attributes[i].attribute_name_index,
                                   "Code")){
                    for(unsigned int j=0; j != method ->attributes[i].attribute_length; j++)
                    {
                        if(!(j%16))
                            printf("\n");
                        printf("%02x ", method ->attributes[i].info[j]);
                    }
                    break;
                }
            }*/
        }
    }
}

#pragma mark -


#pragma mark Public Methods Imp
//Default constructor
EXPORT
ClassFile_FP::ClassFile_FP()
{
    _classFile = NULL;
    _className = NULL;
    _constant_pool_next_index = 1;
    
    //Initialize the INSTRUCTION functions pointers
    INSTRUCTION_initialize_initFuns();
}

//Destructor
EXPORT
ClassFile_FP::~ClassFile_FP()
{
    if(_className)
        free((void *)_className);
    
    if(!_classFile)
        return;
    
    //Assume the whole classFile is fully loaded
    if(_classFile ->constant_pool)
        delete (std::vector<cp_info> *)_classFile ->constant_pool;
    if(_classFile ->fields)
        delete (std::vector<field_info *> *)_classFile ->fields;
    if(_classFile ->methods)
        delete (std::vector<method_info *> *)_classFile ->methods;
    if(_classFile ->attributes)
        delete (std::vector<attribute_info> *)_classFile ->attributes;
    
}

EXPORT
void ClassFile_FP::loadBytecode(const unsigned char *payload,
                                unsigned int payloadSize)
{
    //Read the magic number and check its correct
    u4 magic(payload);
    if((unsigned int)magic != 0xCAFEBABE)
    {
        printf("Magic number does not match. Found: %d\n",
               (unsigned int)magic);
        return;
    }
    _size_in_bytes = payloadSize;
    const unsigned char *payload_ptr = payload;
    
    _classFile = new ClassFile();
    if(!_classFile)
        throw NoMemoryError();
    //Copy up to constant_pool
    memcpy(_classFile, payload_ptr, sizeof(u2)*3+sizeof(u4));
    
    //Copy and parse the constant pool
    //Update the payload pointer
    payload_ptr += sizeof(u2)*3+sizeof(u4);
    
    
    
    
    /*****************************************************
     Start: Constant pool fill
     ****************************************************/
    
    //Get the number of elements in the constant pool
    unsigned short constant_pool_size = _classFile ->
    constant_pool_count;
    
    std::vector<cp_info> *constant_pool = new std::vector<cp_info>();
    
    if(!constant_pool)
        throw NoMemoryError();
    
    _classFile ->constant_pool = (void *)constant_pool;
    
    for(unsigned short i=0; i != constant_pool_size-1; i++){
        //For each element in the CP fill the cp_info structure
        //Read the tag and construct accordingly
        u1 cpTag = *(payload_ptr++);
        cp_info newInfo;
        newInfo.tag = cpTag;
        
        switch (cpTag) {
            case CONSTANT_Class:
                newInfo.info =
                (void *)new CONSTANT_Class_info(payload_ptr);
                payload_ptr += CONSTANT_Class_info::size();
                break;
            case CONSTANT_Fieldref:
                newInfo.info =
                (void *)new CONSTANT_Fieldref_info(payload_ptr);
                payload_ptr += CONSTANT_Fieldref_info::size();
                break;
            case CONSTANT_Methodref:
                newInfo.info =
                (void *)new CONSTANT_Methodref_info(payload_ptr);
                payload_ptr += CONSTANT_Methodref_info::size();
                break;
            case CONSTANT_InterfaceMethodref:
                newInfo.info =
                (void *)new CONSTANT_InterfaceMethodref_info(payload_ptr);
                payload_ptr += CONSTANT_InterfaceMethodref_info::size();
                break;
            case CONSTANT_String:
                newInfo.info =
                (void *)new CONSTANT_String_info(payload_ptr);
                payload_ptr+=CONSTANT_String_info::size();
                break;
            case CONSTANT_Integer:
                newInfo.info =
                (void *)new CONSTANT_Integer_info(payload_ptr);
                payload_ptr+=CONSTANT_Integer_info::size();
                break;
            case CONSTANT_Float:
                newInfo.info =
                (void *)new CONSTANT_Float_info(payload_ptr);
                payload_ptr+= CONSTANT_Float_info::size();
                break;
            case CONSTANT_Long:
                newInfo.info =
                (void *)new CONSTANT_Long_info(payload_ptr);
                payload_ptr+= CONSTANT_Long_info::size();
                i++;
                break;
            case CONSTANT_Double:
                newInfo.info =
                (void *)new CONSTANT_Double_info(payload_ptr);
                payload_ptr+= CONSTANT_Double_info::size();
                i++;
                break;
            case CONSTANT_NameAndType:
                newInfo.info =
                (void *)new CONSTANT_NameAndType_info(payload_ptr);
                payload_ptr+= CONSTANT_NameAndType_info::size();
                break;
            case CONSTANT_Utf8:
                newInfo.info =
                (void *)new CONSTANT_Utf8_info(payload_ptr);
                payload_ptr+=
                ((CONSTANT_Utf8_info*)(newInfo.info))->size();
                break;
            case CONSTANT_MethodHandle:
                newInfo.info =
                (void *)new CONSTANT_MethodHandle_info(payload_ptr);
                payload_ptr+= CONSTANT_MethodHandle_info::size();
                break;
            case CONSTANT_MethodType:
                newInfo.info =
                (void *)new CONSTANT_MethodType_info(payload_ptr);
                payload_ptr+= CONSTANT_MethodType_info::size();
                break;
            case CONSTANT_InvokeDynamic:
                newInfo.info =
                (void *)new CONSTANT_InvokeDynamic_info(payload_ptr);
                payload_ptr+= CONSTANT_InvokeDynamic_info::size();
                break;
            default:
                throw ConstantPool_TagNotRecognized();
                break;
        }
        _push_into_constant_pool(newInfo,false);
        newInfo.claimInfoMemory();
    }
    
    
    /*****************************************************
     End: Constant pool fill
     ****************************************************/
    
    
    
    /*****************************************************
     Start: Interfaces fill
     ****************************************************/
    
    //Fill the fields from access_flags up to fields
    memcpy(&(_classFile ->access_flags), payload_ptr, sizeof(u2)*4);
    payload_ptr += sizeof(u2)*4;
    //Fill the interfaces array
    unsigned short interfaces_size = _classFile ->interfaces_count;
    if(interfaces_size){
        _classFile ->interfaces = (u2 *)malloc(sizeof(u2)*interfaces_size);
        if(!_classFile ->interfaces){
            throw NoMemoryError();
        }
        memcpy(_classFile ->interfaces, payload_ptr,
               sizeof(u2)*interfaces_size);
        payload_ptr += sizeof(u2)*interfaces_size;
    }else{
        _classFile ->interfaces = NULL;
    }
    /*****************************************************
     End: Interfaces fill
     ****************************************************/
    
    
    
    
    /*****************************************************
     Start: Fields fill
     ****************************************************/
    
    memcpy(&_classFile ->fields_count, payload_ptr, sizeof(u2));
    payload_ptr += sizeof(u2);
    
    //Fill the fields array
    //Reserve memory
    unsigned short fields_size = _classFile ->fields_count;
    
    if(fields_size){
        field_info **tmpFields = (field_info **)malloc(sizeof(field_info *)*
                                                       fields_size);
        //new field_info[fields_size];
        if(!tmpFields)
            throw NoMemoryError();
        payload_ptr =
        this ->_fillFieldAttribute(tmpFields,
                                   fields_size,
                  (unsigned char *)payload_ptr);
        //Copy back the fields to the vector
        std::vector<field_info *> *fields = new std::vector<field_info *>();
        if(!fields)
            throw NoMemoryError();
        for(unsigned short i=0; i != fields_size; i++){
            fields ->push_back(tmpFields[i]);
        }
        _classFile ->fields = (void *)fields;
        free(tmpFields);
        
    }else{
        std::vector<field_info *> *fields = new std::vector<field_info *>();
        if(!fields)
            throw NoMemoryError();
        _classFile ->fields = (void *)fields;
    }
    
    /*****************************************************
     End: Fields fill
     ****************************************************/
    
    
    
    /*****************************************************
     Start: Methods fill
     ****************************************************/
    
    //Fill the methods info
    //Get the methods count
    _classFile ->methods_count = u2(payload_ptr);
    payload_ptr += 2;
    unsigned short methods_count = _classFile ->methods_count;
    if(methods_count){
        //Reserve memory
        
        method_info **tmpMethods = (method_info **)malloc(sizeof(method_info *)*
                                                          methods_count);
        
        if(!tmpMethods)
            throw NoMemoryError();
        //Fill the methods
        payload_ptr =
        this ->_fillMethodAttribute(tmpMethods,
                                    methods_count,
                   (unsigned char *)payload_ptr);
        //Copy back the methods to the vector
        std::vector<method_info *> *methods = new std::vector<method_info*>();
        if(!methods)
            throw NoMemoryError();
        for(unsigned short i=0; i!= methods_count; i++){
            methods ->push_back(tmpMethods[i]);
        }
        _classFile ->methods = (void *)methods;
        free(tmpMethods);
    }else{
        _classFile ->methods = new std::vector<method_info*>();
    }
    
    /*****************************************************
     End: Methods fill
     ****************************************************/
    
    //Allocate space for modified methods
    _modifiedMethods = new CodeMod[methods_count];
    _modifiedMethodsSize = methods_count;
    
    
    /*****************************************************
     Start: Attributes fill
     ****************************************************/
    
    _classFile ->attributes_count = u2(payload_ptr);
    payload_ptr += 2;
    //Fill the attributes
    unsigned short attributes_count = _classFile ->attributes_count;
    if(!attributes_count){
        _classFile ->attributes = new std::vector<attribute_info>();
        return;
    }
    //Reserve memory
    std::vector<attribute_info> *attributes = new std::vector<attribute_info>();
    if(!attributes)
        throw NoMemoryError();
    _classFile ->attributes = (void *)attributes;
    
    for(unsigned short i = 0; i != attributes_count; i++){
        attribute_info newAttrib;
        newAttrib.attribute_name_index = u2(payload_ptr);
        payload_ptr += 2;
        newAttrib.attribute_length = u4(payload_ptr);
        payload_ptr += 4;
        unsigned int attrLen =
        newAttrib.attribute_length;
        if(!attrLen){
            newAttrib.info = NULL;
            attributes ->push_back(newAttrib);
            continue;
        }
        newAttrib.info = (u1 *)malloc(sizeof(u1)*attrLen);
        
        if(!newAttrib.info)
            throw NoMemoryError();
        
        memcpy(newAttrib.info,
               payload_ptr,
               attrLen);
        payload_ptr += attrLen;
        attributes ->push_back(newAttrib);
        free(newAttrib.info);
    }
    
    /*****************************************************
     End: Attributes fill
     ****************************************************/
    
    
}

EXPORT
void ClassFile_FP::dumpBytecode(const char *filename)
{
    FILE *outFile = fopen(filename, "w");
    if(!outFile)
        throw InvalidFilenameToDumpBytecode();
    _saveCodeModeForAllMethods();
    _classFile ->constant_pool_count = u2(_constant_pool_next_index);
    std::vector<cp_info> *cp = constantPoolHolder(this ->_classFile);
    std::vector<field_info *> *fields = fieldsHolder(this ->_classFile);
    std::vector<method_info *> *methods = methodsHolder(this ->_classFile);
    std::vector<attribute_info> *attributes =
    (std::vector<attribute_info> *)(this ->_classFile ->attributes);
    
    _classFile ->magic.writeToFile(outFile);
    _classFile ->minor_version.writeToFile(outFile);
    _classFile ->major_version.writeToFile(outFile);
    _classFile ->constant_pool_count.writeToFile(outFile);
    unsigned int cp_size = (unsigned int)cp ->size();
    for(unsigned int i=0; i != cp_size; i++){
        cp_info cp_entry = cp ->at(i);
        fwrite(&cp_entry.tag, sizeof(u1), 1, outFile);
        switch (cp_entry.tag) {
            case CONSTANT_Class:
                ((CONSTANT_Class_info *)cp_entry.info) ->writeToFile(outFile);
                break;
            case CONSTANT_Fieldref:
                ((CONSTANT_Fieldref_info *)cp_entry.info) ->
                writeToFile(outFile);
                break;
            case CONSTANT_Methodref:
                ((CONSTANT_Methodref_info *)cp_entry.info) ->
                writeToFile(outFile);
                break;
            case CONSTANT_InterfaceMethodref:
                ((CONSTANT_InterfaceMethodref_info *)cp_entry.info) ->
                writeToFile(outFile);
                break;
            case CONSTANT_String:
                ((CONSTANT_String_info *)cp_entry.info) ->
                writeToFile(outFile);
                break;
            case CONSTANT_Integer:
                ((CONSTANT_Integer_info *)cp_entry.info) ->
                writeToFile(outFile);
                break;
            case CONSTANT_Float:
                ((CONSTANT_Float_info *)cp_entry.info) ->
                writeToFile(outFile);
                break;
            case CONSTANT_Long:
                ((CONSTANT_Long_info *)cp_entry.info) ->
                writeToFile(outFile);
                i++;
                break;
            case CONSTANT_Double:
                ((CONSTANT_Double_info *)cp_entry.info) ->
                writeToFile(outFile);
                i++;
                break;
            case CONSTANT_NameAndType:
                ((CONSTANT_NameAndType_info *)cp_entry.info) ->
                writeToFile(outFile);
                break;
            case CONSTANT_Utf8:
                ((CONSTANT_Utf8_info *)cp_entry.info) ->
                writeToFile(outFile);
                break;
            case CONSTANT_MethodHandle:
                ((CONSTANT_MethodHandle_info *)cp_entry.info) ->
                writeToFile(outFile);
                break;
            case CONSTANT_MethodType:
                ((CONSTANT_MethodType_info *)cp_entry.info) ->
                writeToFile(outFile);
                break;
            case CONSTANT_InvokeDynamic:
                ((CONSTANT_InvokeDynamic_info *)cp_entry.info) ->
                writeToFile(outFile);
                break;
            default:
                throw ConstantPool_TagNotRecognized();
                break;
        }
    }
    
    _classFile ->access_flags.writeToFile(outFile);
    _classFile ->this_class.writeToFile(outFile);
    _classFile ->super_class.writeToFile(outFile);
    _classFile ->interfaces_count.writeToFile(outFile);
    for(unsigned int i=0; i != _classFile ->interfaces_count; i++){
        _classFile ->interfaces[i].writeToFile(outFile);
    }
    
    _classFile ->fields_count = fields ->size();
    _classFile ->fields_count.writeToFile(outFile);
    for(unsigned int i=0; i != _classFile ->fields_count; i++){
        field_info *curntField = fields ->at(i);
        curntField ->writeToFile(outFile);
    }
    if(methods)
        _classFile ->methods_count = methods ->size();
    else
        _classFile ->methods_count = u2((unsigned short)0);
    _classFile ->methods_count.writeToFile(outFile);
    for(unsigned int i=0; i != _classFile ->methods_count; i++){
        method_info *curntMethod = methods ->at(i);
        curntMethod ->writeToFile(outFile);
    }
    _classFile ->attributes_count.writeToFile(outFile);
    for(unsigned int i=0; i != _classFile ->attributes_count; i++){
        attribute_info curntAttr = attributes ->at(i);
        curntAttr.writeToFile(outFile);
    }
    fclose(outFile);
}
EXPORT
void ClassFile_FP::dumpBytecodeToBuffer(unsigned char *buffer, unsigned long length)
{
    if(!(length >= _size_in_bytes))
       throw InvalidOutputBufferSize();
    _saveCodeModeForAllMethods();
    _classFile ->constant_pool_count = u2(_constant_pool_next_index);
    std::vector<cp_info> *cp = constantPoolHolder(this ->_classFile);
    std::vector<field_info *> *fields = fieldsHolder(this ->_classFile);
    std::vector<method_info *> *methods = methodsHolder(this ->_classFile);
    std::vector<attribute_info> *attributes =
    (std::vector<attribute_info> *)(this ->_classFile ->attributes);
    
    unsigned char *bytecodeArray = buffer;
    
    bytecodeArray = _classFile ->magic.writeToArray(bytecodeArray);
    bytecodeArray = _classFile ->minor_version.writeToArray(bytecodeArray);
    bytecodeArray = _classFile ->major_version.writeToArray(bytecodeArray);
    bytecodeArray = _classFile ->constant_pool_count.writeToArray(bytecodeArray);
    unsigned int cp_size = (unsigned int)cp ->size();
    for(unsigned int i=0; i != cp_size; i++){
        cp_info cp_entry = cp ->at(i);
        *(bytecodeArray++) = cp_entry.tag;
        switch (cp_entry.tag) {
            case CONSTANT_Class:
                bytecodeArray = ((CONSTANT_Class_info *)cp_entry.info) ->
                                    writeToArray(bytecodeArray);
                break;
            case CONSTANT_Fieldref:
                bytecodeArray = ((CONSTANT_Fieldref_info *)cp_entry.info) ->
                                    writeToArray(bytecodeArray);
                break;
            case CONSTANT_Methodref:
                bytecodeArray = ((CONSTANT_Methodref_info *)cp_entry.info) ->
                                    writeToArray(bytecodeArray);
                break;
            case CONSTANT_InterfaceMethodref:
                bytecodeArray = ((CONSTANT_InterfaceMethodref_info *)cp_entry.info) ->
                                    writeToArray(bytecodeArray);
                break;
            case CONSTANT_String:
                bytecodeArray = ((CONSTANT_String_info *)cp_entry.info) ->
                                    writeToArray(bytecodeArray);
                break;
            case CONSTANT_Integer:
                bytecodeArray = ((CONSTANT_Integer_info *)cp_entry.info) ->
                                    writeToArray(bytecodeArray);
                break;
            case CONSTANT_Float:
                bytecodeArray = ((CONSTANT_Float_info *)cp_entry.info) ->
                                    writeToArray(bytecodeArray);
                break;
            case CONSTANT_Long:
                bytecodeArray = ((CONSTANT_Long_info *)cp_entry.info) ->
                                    writeToArray(bytecodeArray);
                i++;
                break;
            case CONSTANT_Double:
                bytecodeArray = ((CONSTANT_Double_info *)cp_entry.info) ->
                                    writeToArray(bytecodeArray);
                i++;
                break;
            case CONSTANT_NameAndType:
                bytecodeArray = ((CONSTANT_NameAndType_info *)cp_entry.info) ->
                                    writeToArray(bytecodeArray);
                break;
            case CONSTANT_Utf8:
                bytecodeArray = ((CONSTANT_Utf8_info *)cp_entry.info) ->
                                    writeToArray(bytecodeArray);
                break;
            case CONSTANT_MethodHandle:
                bytecodeArray = ((CONSTANT_MethodHandle_info *)cp_entry.info) ->
                                    writeToArray(bytecodeArray);
                break;
            case CONSTANT_MethodType:
                bytecodeArray = ((CONSTANT_MethodType_info *)cp_entry.info) ->
                                    writeToArray(bytecodeArray);
                break;
            case CONSTANT_InvokeDynamic:
                bytecodeArray = ((CONSTANT_InvokeDynamic_info *)cp_entry.info) ->
                                    writeToArray(bytecodeArray);
                break;
            default:
                throw ConstantPool_TagNotRecognized();
                break;
        }
    }
    
    bytecodeArray = _classFile ->access_flags.writeToArray(bytecodeArray);
    bytecodeArray = _classFile ->this_class.writeToArray(bytecodeArray);
    bytecodeArray = _classFile ->super_class.writeToArray(bytecodeArray);
    bytecodeArray = _classFile ->interfaces_count.writeToArray(bytecodeArray);
    for(unsigned int i=0; i != _classFile ->interfaces_count; i++){
        bytecodeArray = _classFile ->interfaces[i].writeToArray(bytecodeArray);
    }
    _classFile ->fields_count = fields ->size();
    bytecodeArray = _classFile ->fields_count.writeToArray(bytecodeArray);
    for(unsigned int i=0; i != _classFile ->fields_count; i++){
        field_info *curntField = fields ->at(i);
        bytecodeArray = curntField ->writeToArray(bytecodeArray);
    }
    _classFile ->methods_count = methods ->size();
    bytecodeArray = _classFile ->methods_count.writeToArray(bytecodeArray);
    for(unsigned int i=0; i != _classFile ->methods_count; i++){
        method_info *curntMethod = methods ->at(i);
        bytecodeArray = curntMethod ->writeToArray(bytecodeArray);
    }
    bytecodeArray = _classFile ->attributes_count.writeToArray(bytecodeArray);
    for(unsigned int i=0; i != _classFile ->attributes_count; i++){
        attribute_info curntAttr = attributes ->at(i);
        bytecodeArray = curntAttr.writeToArray(bytecodeArray);
    }

}
EXPORT
void ClassFile_FP::setClassName(const char *name)
{
    _className = (char *)malloc(strlen(name)+1);
    strcpy((char *)_className, name);
}
EXPORT
//Returns the number of elements in the
//constant pool of _classFile
unsigned int ClassFile_FP::constantPoolSize()
{
    if(_classFile && _classFile ->constant_pool)
        return (unsigned int)((std::vector<cp_info> *)
                              _classFile ->constant_pool)->size();
    return 0;
}

EXPORT
//Returns the number of fields
//Declared in _classFile
unsigned int ClassFile_FP::fieldsSize()
{
    if(_classFile && _classFile ->fields)
        return (unsigned int)((std::vector<field_info> *)
                              _classFile ->fields)->size();
    return 0;
}

EXPORT
//Returns the number of methods
//Declared in _classFile
unsigned int ClassFile_FP::methodsSize()
{
    if(_classFile && _classFile ->methods)
        return (unsigned int)methodsHolder(_classFile) ->size();
    return 0;
}

EXPORT
//Returns the number of attributes
//Declared in _classFile
unsigned int ClassFile_FP::attributesSize()
{
    if(_classFile && _classFile ->attributes)
        return (unsigned int)((std::vector<attribute_info> *)
                              _classFile ->attributes)->size();
    return 0;
}
EXPORT
//Returns the current bytecode's size
unsigned long ClassFile_FP::bytecodeSize()
{
    
    //Calculate the size in bytes of the .class file
    _saveCodeModeForAllMethods();
    _classFile ->constant_pool_count = u2(_constant_pool_next_index);
    std::vector<cp_info> *cp = constantPoolHolder(this ->_classFile);
    std::vector<field_info *> *fields = fieldsHolder(this ->_classFile);
    std::vector<method_info *> *methods = methodsHolder(this ->_classFile);
    std::vector<attribute_info> *attributes =
    (std::vector<attribute_info> *)(this ->_classFile ->attributes);

    
    _size_in_bytes = sizeof(u2)*10 + sizeof(u4);
    
    unsigned int cp_size = (unsigned int)cp ->size();
    for(unsigned int i=0; i != cp_size; i++){
        cp_info cp_entry = cp ->at(i);
        _size_in_bytes++;
        switch (cp_entry.tag) {
            case CONSTANT_Class:
                _size_in_bytes +=
                ((CONSTANT_Class_info *)cp_entry.info) ->size();
                break;
            case CONSTANT_Fieldref:
                _size_in_bytes += ((CONSTANT_Fieldref_info *)cp_entry.info) ->
                size();
                break;
            case CONSTANT_Methodref:
                _size_in_bytes += ((CONSTANT_Methodref_info *)cp_entry.info) ->
                size();
                break;
            case CONSTANT_InterfaceMethodref:
                _size_in_bytes += ((CONSTANT_InterfaceMethodref_info *)cp_entry.info) ->
                size();
                break;
            case CONSTANT_String:
                _size_in_bytes += ((CONSTANT_String_info *)cp_entry.info) ->
                size();
                break;
            case CONSTANT_Integer:
                _size_in_bytes += ((CONSTANT_Integer_info *)cp_entry.info) ->
                size();
                break;
            case CONSTANT_Float:
                _size_in_bytes += ((CONSTANT_Float_info *)cp_entry.info) ->
                size();
                break;
            case CONSTANT_Long:
                _size_in_bytes += ((CONSTANT_Long_info *)cp_entry.info) ->
                size();
                i++;
                break;
            case CONSTANT_Double:
                _size_in_bytes += ((CONSTANT_Double_info *)cp_entry.info) ->
                size();
                i++;
                break;
            case CONSTANT_NameAndType:
                _size_in_bytes += ((CONSTANT_NameAndType_info *)cp_entry.info) ->
                size();
                break;
            case CONSTANT_Utf8:
                _size_in_bytes += ((CONSTANT_Utf8_info *)cp_entry.info) ->
                size();
                break;
            case CONSTANT_MethodHandle:
                _size_in_bytes += ((CONSTANT_MethodHandle_info *)cp_entry.info) ->
                size();
                break;
            case CONSTANT_MethodType:
                _size_in_bytes += ((CONSTANT_MethodType_info *)cp_entry.info) ->
                size();
                break;
            case CONSTANT_InvokeDynamic:
                _size_in_bytes += ((CONSTANT_InvokeDynamic_info *)cp_entry.info) ->
                size();
                break;
            default:
                throw ConstantPool_TagNotRecognized();
                break;
        }
    }
    _size_in_bytes += _classFile ->interfaces_count*sizeof(u2);
    
    for(unsigned int i=0; i != fields ->size(); i++){
        field_info *curntField = fields ->at(i);
        _size_in_bytes += curntField ->size();
    }
    
    for(unsigned int i=0; i != methods ->size(); i++){
        method_info *curntMethod = methods ->at(i);
        _size_in_bytes += curntMethod ->size();
    }
    for(unsigned int i=0; i != _classFile ->attributes_count; i++){
        attribute_info curntAttr = attributes ->at(i);
        _size_in_bytes += curntAttr.size();
    }
    return _size_in_bytes;
}

#pragma mark -

#pragma mark Modifications

/******************************************************
 * Constructs a ConstantValue attribute of type Long
 * and updates the Constant Pool.
 * Returns the attribute_info
 *****************************************************/
EXPORT
attribute_info ClassFile_FP::
addConstantValue_Long(CONSTANT_Long_info value)
{
    if(_definedCPAttributes.ConstantValue == (unsigned short)-1){
        //Add ConstantValue to the CP
        CONSTANT_Utf8_info newCV(strlen("ConstantValue"),
                                 "ConstantValue");
        cp_info cp_entry;
        cp_entry.tag = CONSTANT_Utf8;
        cp_entry.info = &newCV;
        _definedCPAttributes.ConstantValue =
                        this ->_push_into_constant_pool(cp_entry,true);
    }
    attribute_info ret;
    ret.attribute_name_index    = _definedCPAttributes.ConstantValue;
    ret.attribute_length        = u4(2);
    
    //Update the size of the attribute_info
    _size_in_bytes += sizeof(u2);
    _size_in_bytes += sizeof(u4);
    
    //Add the constant long value into the CP
    cp_info long_cp;
    long_cp.tag = CONSTANT_Long;
    long_cp.info = &value;
    this ->_push_into_constant_pool(long_cp,true);
    
    
    ConstantValue_attribute attr(_constant_pool_next_index-2);
    ret.info = (u1 *)malloc(sizeof(ConstantValue_attribute));
    memcpy(ret.info, &attr, sizeof(ConstantValue_attribute));
    
    //Update the size
    _size_in_bytes += sizeof(ConstantValue_attribute);
    
    
    return ret;
}



/******************************************************
 * Adds a field to the bytecode
 * Performs some basic checks
 * Adds the name_index and descriptor_index
 * data into the constant pool
 *****************************************************/

EXPORT
void ClassFile_FP::addStaticField(
                                  u2 access_flags,
                                  const char *fieldName,
                                  const char *fieldDesc,
                                  u2 attributes_count,
                                  attribute_info  *attributes)
{
    //Check access_flags
    u2 aFlag = access_flags & u2((unsigned short)7);
    if((aFlag != ACC_PUBLIC)
       && (aFlag != ACC_PROTECTED)
       && (aFlag != ACC_PRIVATE))
    {
        throw FieldInvalidAccessFlag();
    }
    if((access_flags & ACC_FINAL) == ACC_FINAL &&
       (access_flags & ACC_VOLATILE) == ACC_VOLATILE){
        throw FieldInvalidFinalVolatileFlags();
    }
    //Set the static flag.
    access_flags |= ACC_STATIC;
    
    //Update the size of the bytecode by the access_flags
    _size_in_bytes += sizeof(u2);
    
    cp_info newFieldCP;
    newFieldCP.tag = CONSTANT_Utf8;
    newFieldCP.info = new CONSTANT_Utf8_info(strlen(fieldName),fieldName);
    u2 name_index = _push_into_constant_pool(newFieldCP,true);
    
    delete (CONSTANT_Utf8_info *)newFieldCP.info;
    newFieldCP.info = new CONSTANT_Utf8_info(strlen(fieldDesc),fieldDesc);
    u2 desc_index = _push_into_constant_pool(newFieldCP,true);
    
    delete (CONSTANT_Utf8_info *)newFieldCP.info;
    
    //Update the size of the bytecode
    //To account for: name_index, desc_index and attributes_count
    _size_in_bytes += sizeof(u2)*3;
    
    
    //Assemble the field and add it to the fields
    field_info *newField = new field_info(access_flags,name_index,desc_index,
                                          attributes_count,attributes);
    ((std::vector<field_info *> *)(_classFile ->fields)) ->push_back(newField);
    
}

/******************************************************
 * Adds a native method to the bytecode
 * Performs some basic checks
 * Adds the name_index and descriptor_index based in
 * the methodName and methodDesc
 * data into the constant pool
 *****************************************************/
EXPORT
void ClassFile_FP::addNativeMethod(u2 access_flags,
                                   const char *methodName,
                                   const char *methodDesc)
{
    //Check access_flags
    u2 aFlag = access_flags & u2((unsigned short)7);
    if((aFlag != ACC_PUBLIC)
       && (aFlag != ACC_PROTECTED)
       && (aFlag != ACC_PRIVATE))
    {
        throw FieldInvalidAccessFlag();
    }
    //Set the native flag.
    access_flags |= ACC_NATIVE;
    
    //Update the size of the bytecode to account for the
    //access_flags and the number of attributes(0)
    _size_in_bytes += sizeof(u2)*2;
    
    cp_info newFieldCP;
    newFieldCP.tag = CONSTANT_Utf8;
    newFieldCP.info = new CONSTANT_Utf8_info(strlen(methodName),methodName);
    u2 name_index = _push_into_constant_pool(newFieldCP,true);
    
    delete (CONSTANT_Utf8_info *)newFieldCP.info;
    newFieldCP.info = new CONSTANT_Utf8_info(strlen(methodDesc),methodDesc);
    u2 desc_index = _push_into_constant_pool(newFieldCP,true);
    
    delete (CONSTANT_Utf8_info *)newFieldCP.info;
    
    //Update the size of the bytecode for name_index and desc_index
    _size_in_bytes += sizeof(u2)*2;
    
    method_info *newMethod = new method_info(access_flags, name_index,desc_index,
                                             u2((unsigned short)0),NULL);
    std::vector<method_info *> *methods = methodsHolder(_classFile);
    
    //methods ->insert(methods ->begin(), newMethod);
    methods ->push_back(newMethod);
    _classFile ->methods_count += 1;
}


/***************************************************************
 * Adds a Fieldref_info structure into the CP and returns its
 * index
 * Adds fieldName, type and className to the CP.
 * If className is NULL, the name of the current class is used
 *
 ***************************************************************/
EXPORT
u2 ClassFile_FP::addFieldRef(const char *fieldName,
                                      const char *type,
                                      const char *className)
{
    CONSTANT_Fieldref_info retField;
    if(!className)
    {
        if(_definedCPAttributes.ClassName == (unsigned short)-1){
            //Use the already data in CP
            cp_info utfClassName;
            utfClassName.tag = CONSTANT_Utf8;
            CONSTANT_Utf8_info *nptr = new CONSTANT_Utf8_info(strlen(_className),
                                                              _className);
            utfClassName.info = nptr;
            u2 classNameIndex = _push_into_constant_pool(utfClassName,true);
            delete nptr;
            
            utfClassName.tag = CONSTANT_Class;
            CONSTANT_Class_info *classPtr =
                                    new CONSTANT_Class_info(classNameIndex);
            utfClassName.info = classPtr;
            utfClassName.claimInfoMemory();
            
            
            _definedCPAttributes.ClassName =
                        _push_into_constant_pool(utfClassName,true);
        }
        retField.class_index = _definedCPAttributes.ClassName;
    }else{
        //Put the name of the class into the Constant Pool
        cp_info utfClassName;
        utfClassName.tag = CONSTANT_Utf8;
        CONSTANT_Utf8_info *nptr = new CONSTANT_Utf8_info(strlen(_className),
                                                          _className);
        utfClassName.info = nptr;
        utfClassName.claimInfoMemory();
        retField.class_index = _push_into_constant_pool(utfClassName,true);
    }
    //Push the name of the field and its type into the constant pool
    CONSTANT_Utf8_info *fname = new CONSTANT_Utf8_info(strlen(fieldName),
                                                      fieldName);
    CONSTANT_Utf8_info *ftype = new CONSTANT_Utf8_info(strlen(type),
                                                      type);
    
    cp_info nameType;
    nameType.tag    = CONSTANT_Utf8;
    nameType.info   = (void *)fname;
    //Push the name into the constant pool
    u2 nameIndex    = _push_into_constant_pool(nameType,true);
    nameType.info   = (void *)ftype;
    u2 typeIndex    = _push_into_constant_pool(nameType,true);
    
    
    delete fname;
    delete ftype;
    
    //CONSTANT_NameAndType_info
    cp_info cpNameType;
    CONSTANT_NameAndType_info ntype;
    ntype.name_index        = nameIndex;
    ntype.descriptor_index  = typeIndex;
    //Create a field descriptor
    cpNameType.tag  = CONSTANT_NameAndType;
    cpNameType.info = &ntype;
    retField.name_and_type_index = _push_into_constant_pool(cpNameType,true);
    
    //Add the fieldRef to the cp
    cp_info fieldRefAddCP;
    fieldRefAddCP.tag = CONSTANT_Fieldref;
    fieldRefAddCP.info = &retField;
    
    return _push_into_constant_pool(fieldRefAddCP,true);
}

/***************************************************************
 * Adds a Methodref_info structure into the CP and returns its
 * index
 * Adds methodName, type and className to the CP.
 * If className is NULL, the name of the current class is used
 *
 ***************************************************************/
EXPORT
u2 ClassFile_FP::addMethodRef(const char *methodName,
                const char *signature,
                const char *className)
{
    CONSTANT_Methodref_info retField;
    if(!className)
    {
        if(_definedCPAttributes.ClassName == (unsigned short)-1){
            //Use the already data in CP
            cp_info utfClassName;
            utfClassName.tag = CONSTANT_Utf8;
            CONSTANT_Utf8_info *nptr = new CONSTANT_Utf8_info(strlen(_className),
                                                              _className);
            utfClassName.info = nptr;
            u2 classNameIndex = _push_into_constant_pool(utfClassName,true);
            delete nptr;
            
            utfClassName.tag = CONSTANT_Class;
            CONSTANT_Class_info *classPtr =
            new CONSTANT_Class_info(classNameIndex);
            utfClassName.info = classPtr;
            utfClassName.claimInfoMemory();
            
            
            _definedCPAttributes.ClassName =
            _push_into_constant_pool(utfClassName,true);
        }
        retField.class_index = _definedCPAttributes.ClassName;
    }else{
        //Put the name of the class into the Constant Pool
        cp_info utfClassName;
        utfClassName.tag = CONSTANT_Utf8;
        CONSTANT_Utf8_info *nptr = new CONSTANT_Utf8_info(strlen(className),
                                                          className);
        utfClassName.info = nptr;
        u2 classNameIndex = _push_into_constant_pool(utfClassName,true);
        delete nptr;
        
        utfClassName.tag = CONSTANT_Class;
        CONSTANT_Class_info *classPtr =
        new CONSTANT_Class_info(classNameIndex);
        utfClassName.info = classPtr;
        utfClassName.claimInfoMemory();
        
        retField.class_index = _push_into_constant_pool(utfClassName,true);
    }
    //Push the name of the method and its signature into the constant pool
    CONSTANT_Utf8_info *fname = new CONSTANT_Utf8_info(strlen(methodName),
                                                       methodName);
    CONSTANT_Utf8_info *ftype = new CONSTANT_Utf8_info(strlen(signature),
                                                       signature);
    
    cp_info nameType;
    nameType.tag    = CONSTANT_Utf8;
    nameType.info   = (void *)fname;
    //Push the name into the constant pool
    u2 nameIndex    = _push_into_constant_pool(nameType,true);
    nameType.info   = (void *)ftype;
    u2 typeIndex    = _push_into_constant_pool(nameType,true);
    
    
    delete fname;
    delete ftype;
    
    //CONSTANT_NameAndType_info
    cp_info cpNameType;
    CONSTANT_NameAndType_info ntype;
    ntype.name_index        = nameIndex;
    ntype.descriptor_index  = typeIndex;
    //Create a field descriptor
    cpNameType.tag  = CONSTANT_NameAndType;
    cpNameType.info = &ntype;
    retField.name_and_type_index = _push_into_constant_pool(cpNameType,true);
    
    //Add the fieldRef to the cp
    cp_info fieldRefAddCP;
    fieldRefAddCP.tag = CONSTANT_Methodref;
    fieldRefAddCP.info = &retField;
    
    return _push_into_constant_pool(fieldRefAddCP,true);
}
#pragma mark -
#pragma mark Method's Code Modifications


void ClassFile_FP::insert_opcode(unsigned short   methodIndex,
                   unsigned short   instOffset,
                   unsigned short   cpIndexOfData,
                   u1 opcode)
{
    u2 fieldRefIndex(cpIndexOfData);
    //Load the method for modification
    _loadCodeModForMethod(methodIndex);
    
    //Assemble the new instruction
    INSTRUCTION *newInst = new INSTRUCTION();
    newInst ->opcode = opcode;
    newInst ->length = 3;
    newInst ->data = (u1 *)malloc(2);
    fieldRefIndex.writeToArray(newInst ->data);
    //Temporal patch for max_stack update
    if(opcode == INSTRUCTION_getstatic)
        _modifiedMethods[methodIndex].max_stack += 2;
    _size_in_bytes +=
        _modifiedMethods[methodIndex].add_instruction(newInst, instOffset);
}

/******************************************************
 *
 * Shifts the exception table's indices by the amount
 * of given bytes
 *
 * codeAttribPtr    - A pointer to the exception_table_length
 *                    and the exception table itself.
 * amount           - The amount of bytes to add to the
 *                    indices in the exception tables
 *
 ******************************************************/
void ClassFile_FP::shiftExceptionTable(u1 *codeAttribPtr, unsigned char amount)
{
    //Get the number of exceptions
    u2 numberOfExceptions(codeAttribPtr);
    if(numberOfExceptions.uShort() == 0)
        return;
    codeAttribPtr += 2;
    for(unsigned int i=0; i != numberOfExceptions; i++){
        //Read each index and shift it
        u2 curnt_pc(codeAttribPtr); //start_pc
        curnt_pc += amount;
        curnt_pc.writeToArray(codeAttribPtr);
        codeAttribPtr += 2;
        curnt_pc = u2(codeAttribPtr); //end_pc
        curnt_pc += amount;
        curnt_pc.writeToArray(codeAttribPtr);
        codeAttribPtr += 2;
        curnt_pc = u2(codeAttribPtr); //handler_pc
        curnt_pc += amount;
        curnt_pc.writeToArray(codeAttribPtr);
        codeAttribPtr += 4;
    }
}


EXPORT
void ClassFile_FP::add_aload_n(unsigned short methodIndex,
                 unsigned short instOffset,
                 u1 index)
{
    //Get the code attribute
    //Get the method at index
    std::vector<method_info *> *methods = methodsHolder(this ->_classFile);
    if (!(methodIndex < methods ->size())) {
        throw InvalidMethodIndex();
    }
    method_info *method = methods ->at(methodIndex);
    //For the method look the Code attribute
    Code_attribute code;
    u1 *attribInfo = NULL;
    unsigned short attribIndex = -1;
    for(unsigned short i=0; i != method ->attributes_count; i++){
        if(_cpEntryMatches(method ->attributes[i].attribute_name_index,
                           "Code")){
            attribInfo = (method ->attributes[i].info);
            attribIndex = i;
            break;
        }
    }
    //Check if there is a "Code" attribe
    if(!attribInfo)
        return; //Shouldn't happen
    //Initialize code
    memcpy(&code, attribInfo, sizeof(u2)*2+sizeof(u4));
    
    if(instOffset > code.code_length)
        throw InvalidInstructionOffset();
    
    code.max_stack += 1;
    
    //Allocate a new array for the code (to be changed for a vector)
    unsigned char byteIncrease = 0;
    if(index < 4){
        code.code = (u1 *)malloc(sizeof(u1)*(code.code_length + 1));
        byteIncrease = 1;
    }else{
        code.code = (u1 *)malloc(sizeof(u1)*(code.code_length + 2));
        byteIncrease = 2;
    }
    if(!code.code)
        throw NoMemoryError();
    //Pointer to the next instruction to copy from the original
    //bytecode
    u1 *origCodePtr = attribInfo + sizeof(u2)*2+sizeof(u4);
    //Copy the code up to the index of the next instruction
    memcpy(code.code, origCodePtr,sizeof(u1)*instOffset);
    //Update codePtr
    origCodePtr += instOffset;
    
    //Pointer to the next free space in the modified bytecode
    u1 *modCodePtr = code.code + instOffset;
    //Now insert the instruction
    if(index == 0){
        *(modCodePtr++) = 0x2a;
    }else if (index == 1){
        *(modCodePtr++) = 0x2b;
    }else if (index == 2)
    {
        *(modCodePtr++) = 0x2c;
    }else if (index == 3)
    {
        *(modCodePtr++) = 0x2d;
    }else{
        *(modCodePtr++) = 0x19;
        *(modCodePtr++) = index;
    }
    //Copy the rest of the bytecode
    memcpy(modCodePtr, origCodePtr,
           sizeof(u1)*(code.code_length - instOffset));
    //Update origCodePtr
    origCodePtr += sizeof(u1)*(code.code_length - instOffset);
    //Update the code_length
    code.code_length += byteIncrease;
    
    //Now copy the new code to the attribInfo
    method ->attributes[attribIndex].attribute_length += byteIncrease;
    u1 *modifiedAttribute = (u1 *)malloc(sizeof(u1)*
                                         method ->attributes[attribIndex].attribute_length);
    
    //Pointer to the next free byte in modifiedAttribute
    u1 *curntModAttrPtr = modifiedAttribute;
    //Fill the attribute
    memcpy(curntModAttrPtr, &code, sizeof(u2)*2+sizeof(u4));
    curntModAttrPtr += sizeof(u2)*2+sizeof(u4);
    memcpy(curntModAttrPtr,
           code.code, sizeof(u1)*code.code_length);
    curntModAttrPtr += code.code_length;
    unsigned long restOfAttributeSize =
    method ->attributes[attribIndex].attribute_length-
    code.code_length-
    (sizeof(u2)*2+sizeof(u4));
    
    //Shift the exceptions table (if any)
    shiftExceptionTable(origCodePtr, byteIncrease);
    
    memcpy(curntModAttrPtr,
               origCodePtr,
               restOfAttributeSize);
    free(method ->attributes[attribIndex].info);
    method ->attributes[attribIndex].info = modifiedAttribute;
    free(code.code);
    _size_in_bytes += byteIncrease;
    /*
     //For debug write the code
     for(unsigned int i=0; i != method ->attributes[attribIndex].attribute_length; i++){
     printf("0x%02x ", method ->attributes[attribIndex].info[i] );
     }
     printf("\n");*/
    
}


EXPORT
void ClassFile_FP::add_getstatic(unsigned short   methodIndex,
                   unsigned short   instOffset,
                   const char       *fieldName,
                   const char       *fieldType,
                   const char       *className)
{
    u2 fieldRefIndex = addFieldRef(fieldName, fieldType, className);
    add_getstatic(methodIndex, instOffset, fieldRefIndex);
}
EXPORT
void ClassFile_FP::add_getstatic(unsigned short   methodIndex,
                   unsigned short   instOffset,
                   unsigned short   cpIndexOfData)
{
    insert_opcode(methodIndex, instOffset, cpIndexOfData, 0xb2);
}
EXPORT
void ClassFile_FP::add_invokestatic(unsigned short    methodIndex,
                      unsigned short    instOffset,
                      const char        *methodName,
                      const char        *methodSignature,
                      const char        *className)
{
    u2 methodRefIndex = addMethodRef(methodName, methodSignature, className);
    add_invokestatic(methodIndex, instOffset, methodRefIndex);
}
EXPORT
void ClassFile_FP::add_invokestatic(unsigned short    methodIndex,
                      unsigned short    instOffset,
                      unsigned short    cpIndexOfData)
{
    
    insert_opcode(methodIndex, instOffset, cpIndexOfData, 0xb8);
}

#pragma mark -

#pragma mark Printing
EXPORT
void ClassFile_FP::printMethodAt(unsigned short index)
{
    /*************************************************
     General output
     Name:
     Flags:
     Descriptor:
     *************************************************/
    //Get the method_info structure
    std::vector<method_info *> *methods = methodsHolder(this ->_classFile);
    std::vector<cp_info> *cp = constantPoolHolder(this ->_classFile);
    
    if(index >= methods ->size())
        return;
    
    method_info *method = methods ->at(index);
    //Find the name
    unsigned short nameInd = method ->name_index;
    //Basic checks
    if(nameInd >= cp ->size())
        throw InvalidConstantPoolIndex();
    cp_info nameCP = cp ->at(nameInd-1);
    if(nameCP.tag != CONSTANT_Utf8)
        throw UnexpectedDataTypeAtConstantPoolIndex();
    
    CONSTANT_Utf8_info *mName = (CONSTANT_Utf8_info *)nameCP.info;
    char *methodName = (char *)malloc(mName -> length + 1);
    memcpy(methodName, mName ->bytes, mName -> length);
    methodName[mName -> length] = 0;
    std::cout << "---------- Method ----------" << std::endl;
    std::cout << "Name: " << methodName << std::endl;
    /*const char *attrs = _accessModifiersToString(true, method ->access_flags);
    if(attrs)
        std::cout << "Flags: " << attrs << std::endl;
    else
        std::cout << "No Flags:" << std::endl;*/
    //Get the descriptor and print it
    cp_info descCP = cp ->at(method ->descriptor_index-1);
    CONSTANT_Utf8_info *mDesc = (CONSTANT_Utf8_info *)descCP.info;
    char *methodDesc = (char *)malloc(mDesc -> length + 1);
    memcpy(methodDesc, mDesc ->bytes, mDesc -> length);
    methodDesc[mDesc -> length] = 0;
    std::cout << "Descriptor: " << methodDesc << std::endl;
    std::cout << "-------- End: Method --------" << std::endl <<std::endl;
    mName = NULL;
    mDesc = NULL;
    free(methodName);
    free(methodDesc);
    /*if(attrs)
        free((char *)attrs);*/
}
#pragma mark -

#pragma mark Basic Queries
//Returns true if the name of the method at index methodIndex
//equals the given 'methodName'. Returns False otherwise
EXPORT
bool ClassFile_FP::methodNameEquals(unsigned short methodIndex,
                      const char *methodName)
{
    std::vector<method_info *> *methods = methodsHolder(this ->_classFile);
    if(methodIndex >= methods ->size())
        throw InvalidMethodIndex();
    method_info *method = methods ->at(methodIndex);
    //Look for the name index entry
    std::vector<cp_info> *cp = constantPoolHolder(this ->_classFile);
    cp_info meth_name = cp ->at(method ->name_index-1);
    //Check if method_name has the right tag
    if(meth_name.tag != CONSTANT_Utf8)
        throw UnexpectedDataTypeAtConstantPoolIndex();
    unsigned long argLen = strlen(methodName);
    CONSTANT_Utf8_info *name_ptr = (CONSTANT_Utf8_info *)meth_name.info;
    if(argLen != name_ptr ->length)
        return false;
    //lengths match so we continue
    for(unsigned int i=0; i != argLen; i++){
        if(methodName[i] != name_ptr ->bytes[i])
            return false;
    }
    return true;
}

//Returns true if the class has the argument access_flags
//set. False otherwise
EXPORT
bool ClassFile_FP::classHasAccess(u2 queryAccessFlags)
{
    if((_classFile->access_flags & queryAccessFlags) == queryAccessFlags)
        return true;
    return false;
}
//Returns true if the method has the argument access_flags
//set. False otherwise
EXPORT
bool ClassFile_FP::methodHasAccess(unsigned short methodIndex,
                                   u2 queryAccessFlags)
{
    std::vector<method_info *> *methods = methodsHolder(this ->_classFile);
    if(methodIndex >= methods ->size())
        throw InvalidMethodIndex();
    method_info *method = methods ->at(methodIndex);
    if((method ->access_flags & queryAccessFlags) == queryAccessFlags)
        return true;
    return false;
        
}
#pragma mark -
