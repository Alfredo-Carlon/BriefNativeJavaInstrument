//
//  ClassFile_FirstParser.cpp
//  
//
//  Created by Alfredo Cruz on 9/30/16.
//
//

#include <ClassFile_FirstParser.hpp>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <iostream>

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

void ClassFile_FP::_push_into_constant_pool(cp_info& data)
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
}

#pragma mark -


#pragma mark Public Methods Imp


//Default constructor
ClassFile_FP::ClassFile_FP()
{
    _classFile = NULL;
    _constant_pool_next_index = 1;
}

//Destructor
ClassFile_FP::~ClassFile_FP()
{
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

void ClassFile_FP::loadBytecode(unsigned char *payload,
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
    unsigned char *payload_ptr = payload;
    
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
        _push_into_constant_pool(newInfo);
        
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
                                                 payload_ptr);
        //Copy back the fields to the vector
        std::vector<field_info *> *fields = new std::vector<field_info *>();
        if(!fields)
            throw NoMemoryError();
        for(unsigned short i=0; i != fields_size; i++){
            fields ->push_back(tmpFields[i]);
        }
        _classFile ->fields = (void *)fields;
        //free(tmpFields);
        delete[] tmpFields;
        
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
                                        payload_ptr);
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
        _classFile ->methods = NULL;
    }
    
    /*****************************************************
                    End: Methods fill
     ****************************************************/
    
    
    
    
    
    /*****************************************************
                    Start: Attributes fill
     ****************************************************/
    
    _classFile ->attributes_count = u2(payload_ptr);
    payload_ptr += 2;
    //Fill the attributes
    unsigned short attributes_count = _classFile ->attributes_count;
    if(!attributes_count){
        _classFile ->attributes = NULL;
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
    }
    
    /*****************************************************
                    End: Attributes fill
     ****************************************************/
    
    
}


void ClassFile_FP::dumpBytecode(const char *filename)
{
    FILE *outFile = fopen(filename, "w");
    if(!outFile)
        throw InvalidFilenameToDumpBytecode();
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
    _classFile ->methods_count = methods ->size();
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
    
}

//Returns the number of elements in the
//constant pool of _classFile
unsigned int ClassFile_FP::constantPoolSize()
{
    if(_classFile && _classFile ->constant_pool)
        return (unsigned int)((std::vector<cp_info> *)
                          _classFile ->constant_pool)->size();
    return 0;
}


//Returns the number of fields
//Declared in _classFile
unsigned int ClassFile_FP::fieldsSize()
{
    if(_classFile && _classFile ->fields)
        return (unsigned int)((std::vector<field_info> *)
                              _classFile ->fields)->size();
    return 0;
}


//Returns the number of methods
//Declared in _classFile
unsigned int ClassFile_FP::methodsSize()
{
    if(_classFile && _classFile ->methods)
        return (unsigned int)((std::vector<attribute_info> *)
                              _classFile ->methods)->size();
    return 0;
}


//Returns the number of attributes
//Declared in _classFile
unsigned int ClassFile_FP::attributesSize()
{
    if(_classFile && _classFile ->attributes)
        return (unsigned int)((std::vector<attribute_info> *)
                              _classFile ->attributes)->size();
    return 0;
}

#pragma mark -

#pragma mark Modifications

/******************************************************
 * Constructs a ConstantValue attribute of type Long
 * and updates the Constant Pool.
 * Returns the attribute_info
 *****************************************************/

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
        this ->_push_into_constant_pool(cp_entry);
        _definedCPAttributes.ConstantValue = _constant_pool_next_index - 1;
        
        //Update the size of the bytecode
        _size_in_bytes += sizeof(u1);
        _size_in_bytes += newCV.size();
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
    this ->_push_into_constant_pool(long_cp);
    
    //Update the size of the long entry in the CP
    _size_in_bytes += sizeof(u1);
    _size_in_bytes += value.size();
    
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
    _push_into_constant_pool(newFieldCP);
    
    //Update the size of the bytecode
    _size_in_bytes += sizeof(u1);
    _size_in_bytes += ((CONSTANT_Utf8_info *)newFieldCP.info) ->size();
    
    u2 name_index(_constant_pool_next_index - 1);
    delete (CONSTANT_Utf8_info *)newFieldCP.info;
    newFieldCP.info = new CONSTANT_Utf8_info(strlen(fieldDesc),fieldDesc);
    _push_into_constant_pool(newFieldCP);
    
    //Update the size of the bytecode
    _size_in_bytes += sizeof(u1);
    _size_in_bytes += ((CONSTANT_Utf8_info *)newFieldCP.info) ->size();
    
    
    u2 desc_index(_constant_pool_next_index - 1);
    delete (CONSTANT_Utf8_info *)newFieldCP.info;
    
    //Update the size of the bytecode
    //To account for: name_index, desc_index and attributes_count
    _size_in_bytes += sizeof(u2)*3;
    
    //In the attributes list we check if we have the ConstantValue
    /*unsigned short attrib_size = attributes_count;
    bool foundConstantValue = false;
    for(unsigned short i=0; i != attrib_size; i++){
        u2 att_index = attributes[i].attribute_name_index;
        if(constant_pool ->at(att_index).tag == CONSTANT_Utf8){
            //For the moment use strcmp assuming all changes are made
            //within ASCII world
            CONSTANT_Utf8_info *utf8_ptr =
            (CONSTANT_Utf8_info *)constant_pool ->at(att_index).info;
            if(!strcmp((const char *)(utf8_ptr->bytes),
                       "ConstantValue")){
                //We found the ConstantValue attribute
                foundConstantValue = true;
                break;
            }
        }
    }
    if(!foundConstantValue)
        throw NoConstantValueAttributeForStaticField(); */
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
    
    //Update the size of the bytecode
    _size_in_bytes += access_flags;
    
    cp_info newFieldCP;
    newFieldCP.tag = CONSTANT_Utf8;
    newFieldCP.info = new CONSTANT_Utf8_info(strlen(methodName),methodName);
    _push_into_constant_pool(newFieldCP);
    
    //Update the size of the bytecode
    _size_in_bytes += sizeof(u1);
    _size_in_bytes += ((CONSTANT_Utf8_info *)(newFieldCP.info)) ->size();
    
    u2 name_index(_constant_pool_next_index - 1);
    delete (CONSTANT_Utf8_info *)newFieldCP.info;
    newFieldCP.info = new CONSTANT_Utf8_info(strlen(methodDesc),methodDesc);
    _push_into_constant_pool(newFieldCP);
    
    //Update the size of the bytecode
    _size_in_bytes += sizeof(u1);
    _size_in_bytes += ((CONSTANT_Utf8_info *)(newFieldCP.info)) ->size();
    
    u2 desc_index(_constant_pool_next_index - 1);
    delete (CONSTANT_Utf8_info *)newFieldCP.info;
    
    //Update the size of the bytecode for name_index and desc_index
    _size_in_bytes += sizeof(u2)*2;
    
    method_info *newMethod = new method_info(access_flags, name_index,desc_index,
                                             u2((unsigned short)0),NULL);
    ((std::vector<method_info *> *)
                (_classFile ->methods)) ->push_back(newMethod);
}

#pragma mark -

#pragma mark Printing

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
    const char *attrs = _accessModifiersToString(true, method ->access_flags);
    std::cout << "Flags: " << attrs << std::endl;
    //Get the descriptor and print it
    cp_info descCP = cp ->at(method ->descriptor_index-1);
    CONSTANT_Utf8_info *mDesc = (CONSTANT_Utf8_info *)descCP.info;
    char *methodDesc = (char *)malloc(mDesc -> length + 1);
    memcpy(methodDesc, mDesc ->bytes, mDesc -> length);
    methodName[mDesc -> length] = 0;
    std::cout << "Descriptor: " << methodDesc << std::endl;
    std::cout << "-------- End: Method --------" << std::endl <<std::endl;
    free(methodName);
    free(methodDesc);
    free((char *)attrs);
}

#pragma mark -
