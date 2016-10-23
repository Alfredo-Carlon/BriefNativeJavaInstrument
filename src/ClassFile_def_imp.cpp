//
//  ClassFile_def_imp.cpp
//  NativeBasicInstrument
//
//  Created by Alfredo Cruz on 10/2/16.
//  Copyright © 2016 Alfredo Cruz. All rights reserved.
//

#include <ClassFile_def.hpp>
#include <cstdio>

/******************************************************
 *            Implements the constructors             *
 *            of the structures in ClassFile_def      *
 *****************************************************/

#pragma mark Basic types

u2::u2()
{
    u[0] = 0;
    u[1] = 0;
}

u2::u2(unsigned short s)
{
    u[0] = (s&0xFF00) >> 8;
    u[1] = s&0x00FF;
}

u2::u2(const unsigned char *bytecode){
    u[0] = *bytecode;
    u[1] = *(bytecode + 1);
}

u2::u2(const u2& cpy)
{
    memcpy(u, cpy.u, sizeof(u1)*2);
}

void u2::writeToFile(void *file_ptr)
{
    fwrite(u, sizeof(u1), 2, (FILE *)file_ptr);
}

#pragma mark -

u4::u4()
{
    u[0] = 0; u[1] = 0; u[2] = 0;
    u[3] = 0;
}

u4::u4(unsigned int ui){
    u[0] = (ui & 0xFF000000) >> 3;
    u[1] = (ui & 0xFF0000) >> 2;
    u[2] = (ui & 0xFF00) >> 1;
    u[3] = (ui & 0xFF);
}

u4::u4(const unsigned char *bytecode){
    u[0] = *bytecode;
    u[1] = *(bytecode + 1);
    u[2] = *(bytecode + 2);
    u[3] = *(bytecode + 3);
}
u4::u4(const u4& cpy){
    u[0] = cpy.u[0];
    u[1] = cpy.u[1];
    u[2] = cpy.u[2];
    u[3] = cpy.u[3];
}

void u4::writeToFile(void *file_ptr)
{
    fwrite(u, sizeof(u1), 4, (FILE *)file_ptr);
}

#pragma mark -

#pragma mark cp_info struct
/****************************************************************
 *                         cp_info struct                       *
 *                              start                           *
 ****************************************************************/

cp_info::cp_info(){
    tag             = 0;
    info            = NULL;
    _ownsInfoMemory = false;
}
cp_info::cp_info(const cp_info& cpy)
{
    tag = cpy.tag;
    _ownsInfoMemory = true;
    //Copy based on tag
    switch (tag) {
        case CONSTANT_Class:
            info = (void *)new CONSTANT_Class_info(
                                                   *((CONSTANT_Class_info *)cpy.info));
            break;
        case CONSTANT_Fieldref:
            info = (void *)new CONSTANT_Fieldref_info(
                                                      *((CONSTANT_Fieldref_info *)cpy.info));
            break;
        case CONSTANT_Methodref:
            info = (void *)new CONSTANT_Methodref_info(
                                                       *((CONSTANT_Methodref_info *)cpy.info));
            break;
        case CONSTANT_InterfaceMethodref:
            info =
            (void *)new CONSTANT_InterfaceMethodref_info(
                                                         *((CONSTANT_InterfaceMethodref_info *)cpy.info));
            break;
        case CONSTANT_String:
            info =
            (void *)new CONSTANT_String_info(
                                             *((CONSTANT_String_info *)cpy.info));
            break;
        case CONSTANT_Integer:
            info =
            (void *)new CONSTANT_Integer_info(
                                              *((CONSTANT_Integer_info *)cpy.info));
            break;
        case CONSTANT_Float:
            info =
            (void *)new CONSTANT_Float_info(
                                            *((CONSTANT_Float_info *)cpy.info));
            break;
        case CONSTANT_Long:
            info =
            (void *)new CONSTANT_Long_info(
                                           *((CONSTANT_Long_info *)cpy.info));
            break;
        case CONSTANT_Double:
            info =
            (void *)new CONSTANT_Double_info(
                                             *((CONSTANT_Double_info *)cpy.info));
            break;
        case CONSTANT_NameAndType:
            info =
            (void *)new CONSTANT_NameAndType_info(
                                                  *((CONSTANT_NameAndType_info *)cpy.info));
            break;
        case CONSTANT_Utf8:
            info =
            (void *)new CONSTANT_Utf8_info(
                                           *((CONSTANT_Utf8_info *)cpy.info));
            break;
        case CONSTANT_MethodHandle:
            info =
            (void *)new CONSTANT_MethodHandle_info(
                                                   *((CONSTANT_MethodHandle_info *)cpy.info));
            break;
        case CONSTANT_MethodType:
            info =
            (void *)new CONSTANT_MethodType_info(
                                                 *((CONSTANT_MethodType_info *)cpy.info));
            break;
        case CONSTANT_InvokeDynamic:
            info =
            (void *)new CONSTANT_InvokeDynamic_info(
                                                    *((CONSTANT_InvokeDynamic_info *)cpy.info));
            break;
        default:
            throw ConstantPool_TagNotRecognized();
            break;
    }
}
cp_info & cp_info::operator= (const cp_info& cpy)
{
    if(this == &cpy)
        return *this;
    this ->~cp_info();
    _ownsInfoMemory = true;
    tag = cpy.tag;
    //Copy based on tag
    switch (tag) {
        case CONSTANT_Class:
            info = (void *)new CONSTANT_Class_info(
                                                   *((CONSTANT_Class_info *)cpy.info));
            break;
        case CONSTANT_Fieldref:
            info = (void *)new CONSTANT_Fieldref_info(
                                                      *((CONSTANT_Fieldref_info *)cpy.info));
            break;
        case CONSTANT_Methodref:
            info = (void *)new CONSTANT_Methodref_info(
                                                       *((CONSTANT_Methodref_info *)cpy.info));
            break;
        case CONSTANT_InterfaceMethodref:
            info =
            (void *)new CONSTANT_InterfaceMethodref_info(
                                                         *((CONSTANT_InterfaceMethodref_info *)cpy.info));
            break;
        case CONSTANT_String:
            info =
            (void *)new CONSTANT_String_info(
                                             *((CONSTANT_String_info *)cpy.info));
            break;
        case CONSTANT_Integer:
            info =
            (void *)new CONSTANT_Integer_info(
                                              *((CONSTANT_Integer_info *)cpy.info));
            break;
        case CONSTANT_Float:
            info =
            (void *)new CONSTANT_Float_info(
                                            *((CONSTANT_Float_info *)cpy.info));
            break;
        case CONSTANT_Long:
            info =
            (void *)new CONSTANT_Long_info(
                                           *((CONSTANT_Long_info *)cpy.info));
            break;
        case CONSTANT_Double:
            info =
            (void *)new CONSTANT_Double_info(
                                             *((CONSTANT_Double_info *)cpy.info));
            break;
        case CONSTANT_NameAndType:
            info =
            (void *)new CONSTANT_NameAndType_info(
                                                  *((CONSTANT_NameAndType_info *)cpy.info));
            break;
        case CONSTANT_Utf8:
            info =
            (void *)new CONSTANT_Utf8_info(
                                           *((CONSTANT_Utf8_info *)cpy.info));
            break;
        case CONSTANT_MethodHandle:
            info =
            (void *)new CONSTANT_MethodHandle_info(
                                                   *((CONSTANT_MethodHandle_info *)cpy.info));
            break;
        case CONSTANT_MethodType:
            info =
            (void *)new CONSTANT_MethodType_info(
                                                 *((CONSTANT_MethodType_info *)cpy.info));
            break;
        case CONSTANT_InvokeDynamic:
            info =
            (void *)new CONSTANT_InvokeDynamic_info(
                                                    *((CONSTANT_InvokeDynamic_info *)cpy.info));
            break;
        default:
            throw ConstantPool_TagNotRecognized();
            break;
    }
    return *this;
}
void cp_info::claimInfoMemory()
{
    _ownsInfoMemory = true;
}
void cp_info::relinquishInfoMemory()
{
    _ownsInfoMemory = false;
}
cp_info::~cp_info(){
    if(info == NULL || !_ownsInfoMemory)
        return;
    //Delete with respect to type
    switch (tag) {
        case CONSTANT_Class:
            delete (CONSTANT_Class_info *)info;
            break;
        case CONSTANT_Fieldref:
            delete (CONSTANT_Fieldref_info *)info;
            break;
        case CONSTANT_Methodref:
            delete (CONSTANT_Methodref_info *)info;
            break;
        case CONSTANT_InterfaceMethodref:
            delete (CONSTANT_InterfaceMethodref_info *)info;
            break;
        case CONSTANT_String:
            delete (CONSTANT_String_info *)info;
            break;
        case CONSTANT_Integer:
            delete (CONSTANT_Integer_info *)info;
            break;
        case CONSTANT_Float:
            delete (CONSTANT_Float_info *)info;
            break;
        case CONSTANT_Long:
            delete (CONSTANT_Long_info *)info;
            break;
        case CONSTANT_Double:
            delete (CONSTANT_Double_info *)info;
            break;
        case CONSTANT_NameAndType:
            delete (CONSTANT_NameAndType_info *)info;
            break;
        case CONSTANT_Utf8:
            delete (CONSTANT_Utf8_info *)info;
            break;
        case CONSTANT_MethodHandle:
            delete (CONSTANT_MethodHandle_info *)info;
            break;
        case CONSTANT_MethodType:
            delete (CONSTANT_MethodType_info *)info;
            break;
        case CONSTANT_InvokeDynamic:
            delete (CONSTANT_InvokeDynamic_info *)info;
            break;
        default:
            throw ConstantPool_TagNotRecognized();
            break;
    }
    info = NULL;
}
#pragma mark -

/****************************************************************
 *                         cp_info struct                       *
 *                               end                            *
 ****************************************************************/

#pragma mark CONSTANT*_info structs

/****************************************************************
 *                     CONSTANT*_info structs                   *
 *                             start                            *
 ****************************************************************/
CONSTANT_Class_info::CONSTANT_Class_info(unsigned char *bytecode)
{
    name_index = u2(bytecode);
}
CONSTANT_Class_info::CONSTANT_Class_info(const CONSTANT_Class_info& cpy)
{
    name_index = cpy.name_index;
}
void CONSTANT_Class_info::writeToFile(void *file_ptr)
{
    name_index.writeToFile(file_ptr);
}
#pragma mark -

CONSTANT_Fieldref_info::CONSTANT_Fieldref_info(unsigned char *bytecode)
{
    class_index = u2(bytecode);
    name_and_type_index = u2(bytecode+2);
}
CONSTANT_Fieldref_info::CONSTANT_Fieldref_info(
                                               const CONSTANT_Fieldref_info& cpy)
{
    class_index         = cpy.class_index;
    name_and_type_index = cpy.name_and_type_index;
}
void CONSTANT_Fieldref_info::writeToFile(void *file_ptr)
{
    class_index.writeToFile(file_ptr);
    name_and_type_index.writeToFile(file_ptr);
}
#pragma mark -

CONSTANT_Methodref_info::CONSTANT_Methodref_info(unsigned char *bytecode)
{
    class_index = u2(bytecode);
    name_and_type_index = u2(bytecode+2);
}
CONSTANT_Methodref_info::CONSTANT_Methodref_info(
                                                 const CONSTANT_Methodref_info& cpy)
{
    class_index         = cpy.class_index;
    name_and_type_index = cpy.name_and_type_index;
}
void CONSTANT_Methodref_info::writeToFile(void *file_ptr)
{
    class_index.writeToFile(file_ptr);
    name_and_type_index.writeToFile(file_ptr);
}
#pragma mark -

CONSTANT_InterfaceMethodref_info::
CONSTANT_InterfaceMethodref_info(unsigned char *bytecode)
{
    class_index = u2(bytecode);
    name_and_type_index = u2(bytecode+2);
}
CONSTANT_InterfaceMethodref_info::CONSTANT_InterfaceMethodref_info(
                                                                   const CONSTANT_InterfaceMethodref_info& cpy)
{
    class_index         = cpy.class_index;
    name_and_type_index = cpy.name_and_type_index;
}
void CONSTANT_InterfaceMethodref_info::writeToFile(void *file_ptr)
{
    class_index.writeToFile(file_ptr);
    name_and_type_index.writeToFile(file_ptr);
}
#pragma mark -

CONSTANT_String_info::CONSTANT_String_info(unsigned char *bytecode)
{
    string_index = u2(bytecode);
}
CONSTANT_String_info::CONSTANT_String_info(const CONSTANT_String_info& cpy)
{
    string_index = cpy.string_index;
}
void CONSTANT_String_info::writeToFile(void *file_ptr)
{
    string_index.writeToFile(file_ptr);
}
#pragma mark -

CONSTANT_Integer_info::CONSTANT_Integer_info(unsigned char *bytecode)
{
    bytes = u4(bytecode);
}
CONSTANT_Integer_info::CONSTANT_Integer_info(const CONSTANT_Integer_info& cpy)
{
    bytes = cpy.bytes;
}
void CONSTANT_Integer_info::writeToFile(void *file_ptr)
{
    bytes.writeToFile(file_ptr);
}
#pragma mark -

CONSTANT_Float_info::CONSTANT_Float_info(unsigned char *bytecode)
{
    bytes = u4(bytecode);
}
CONSTANT_Float_info::CONSTANT_Float_info(const CONSTANT_Float_info& cpy)
{
    bytes = cpy.bytes;
}
void CONSTANT_Float_info::writeToFile(void *file_ptr)
{
    bytes.writeToFile(file_ptr);
}
#pragma mark -
CONSTANT_Long_info::CONSTANT_Long_info()
{
    high_bytes  = u4((unsigned int)0);
    low_bytes   = u4((unsigned int)0);
}
CONSTANT_Long_info::CONSTANT_Long_info(unsigned char *bytecode)
{
    high_bytes  = u4(bytecode);
    low_bytes   = u4(bytecode + 4);
}
CONSTANT_Long_info::CONSTANT_Long_info(const CONSTANT_Long_info& cpy)
{
    high_bytes  = cpy.high_bytes;
    low_bytes   = cpy.low_bytes;
}
void CONSTANT_Long_info::writeToFile(void *file_ptr)
{
    high_bytes.writeToFile(file_ptr);
    low_bytes.writeToFile(file_ptr);
}
#pragma mark -

CONSTANT_Double_info::CONSTANT_Double_info(unsigned char *bytecode)
{
    high_bytes  = u4(bytecode);
    low_bytes   = u4(bytecode + 4);
}
CONSTANT_Double_info::CONSTANT_Double_info(const CONSTANT_Double_info& cpy)
{
    high_bytes  = cpy.high_bytes;
    low_bytes   = cpy.low_bytes;
}
void CONSTANT_Double_info::writeToFile(void *file_ptr)
{
    high_bytes.writeToFile(file_ptr);
    low_bytes.writeToFile(file_ptr);
}
#pragma mark -

CONSTANT_NameAndType_info::CONSTANT_NameAndType_info(unsigned char *bytecode)
{
    name_index          = u2(bytecode);
    descriptor_index    = u2(bytecode+2);
}
CONSTANT_NameAndType_info::CONSTANT_NameAndType_info(const
                                                     CONSTANT_NameAndType_info& cpy)
{
    name_index          =cpy.name_index;
    descriptor_index    =cpy.descriptor_index;
}
void CONSTANT_NameAndType_info::writeToFile(void *file_ptr)
{
    name_index.writeToFile(file_ptr);
    descriptor_index.writeToFile(file_ptr);
}
#pragma mark -

CONSTANT_Utf8_info::CONSTANT_Utf8_info()
{
    length  = u2((unsigned short)0);
    bytes   = NULL;
}
CONSTANT_Utf8_info::CONSTANT_Utf8_info(unsigned char *bytecode)
{
    length = u2(bytecode);
    bytes = (u1 *)malloc(sizeof(u1)*length);
    if(!bytes)
        throw NoMemoryError();
    memcpy(bytes, bytecode + 2, sizeof(u1)*length);
}
CONSTANT_Utf8_info::CONSTANT_Utf8_info(const CONSTANT_Utf8_info& cpy)
{
    length = cpy.length;
    bytes = (u1 *)malloc(sizeof(u1)*length);
    if(!bytes)
        throw NoMemoryError();
    memcpy(bytes, cpy.bytes, sizeof(u1)*length);
}
CONSTANT_Utf8_info & CONSTANT_Utf8_info::operator= (const CONSTANT_Utf8_info& rs)
{
    if(this == &rs)
        return *this;
    this ->~CONSTANT_Utf8_info();
    length = rs.length;
    bytes = (u1 *)malloc(sizeof(u1)*length);
    if(!bytes)
        throw NoMemoryError();
    memcpy(bytes, rs.bytes, sizeof(u1)*length);
    return *this;
}
CONSTANT_Utf8_info::CONSTANT_Utf8_info(unsigned short len, const char *string)
{
    length = u2(len);
    bytes = (u1 *)malloc(sizeof(u1)*len);
    if(!bytes)
        throw NoMemoryError();
    memcpy(bytes, string, sizeof(u1)*len);
}
CONSTANT_Utf8_info::~CONSTANT_Utf8_info()
{
    if(bytes != NULL)
        free(bytes);
}
void CONSTANT_Utf8_info::writeToFile(void *file_ptr)
{
    length.writeToFile(file_ptr);
    fwrite(bytes, sizeof(u1), length, (FILE *)file_ptr);
}

#pragma mark -

CONSTANT_MethodHandle_info::CONSTANT_MethodHandle_info(unsigned char *bytecode)
{
    reference_kind = *bytecode;
    reference_index = u2(bytecode+1);
}
CONSTANT_MethodHandle_info::CONSTANT_MethodHandle_info(
                                                       const CONSTANT_MethodHandle_info &cpy)
{
    reference_kind  = cpy.reference_kind;
    reference_index = cpy.reference_index;
}
void CONSTANT_MethodHandle_info::writeToFile(void *file_ptr)
{
    fwrite(&reference_kind, sizeof(u1), 1, (FILE *)file_ptr);
    reference_index.writeToFile(file_ptr);
}
#pragma mark -
CONSTANT_MethodType_info::CONSTANT_MethodType_info(unsigned char *bytecode)
{
    descriptor_index = u2(bytecode);
}
CONSTANT_MethodType_info::CONSTANT_MethodType_info(
                                                   const CONSTANT_MethodType_info& cpy)
{
    descriptor_index = cpy.descriptor_index;
}
void CONSTANT_MethodType_info::writeToFile(void *file_ptr)
{
    descriptor_index.writeToFile(file_ptr);
}
#pragma mark -

CONSTANT_InvokeDynamic_info::
CONSTANT_InvokeDynamic_info(unsigned char *bytecode)
{
    bootstrap_method_attr_index = u2(bytecode);
    name_and_type_index         = u2(bytecode + 2);
}
CONSTANT_InvokeDynamic_info::
CONSTANT_InvokeDynamic_info(const CONSTANT_InvokeDynamic_info& cpy)
{
    bootstrap_method_attr_index = cpy.bootstrap_method_attr_index;
    name_and_type_index         = cpy.name_and_type_index;
}
void CONSTANT_InvokeDynamic_info::writeToFile(void *file_ptr)
{
    bootstrap_method_attr_index.writeToFile(file_ptr);
    name_and_type_index.writeToFile(file_ptr);
}
/****************************************************************
 *                     CONSTANT*_info structs                   *
 *                             end                              *
 ****************************************************************/
#pragma mark -

#pragma mark field_info struct

/****************************************************************
 *                      field_info struct                       *
 *                            start                             *
 ****************************************************************/

field_info::field_info()
{
    access_flags        = u2((unsigned short)0);
    name_index          = u2((unsigned short)0);
    descriptor_index    = u2((unsigned short)0);
    attributes_count    = u2((unsigned short)0);
    attributes          = NULL;
}
field_info::field_info(const field_info& cpy)
{
    access_flags        = cpy.access_flags;
    name_index          = cpy.name_index;
    descriptor_index    = cpy.descriptor_index;
    attributes_count    = cpy.attributes_count;
    if(!attributes_count){
        attributes  = NULL;
        return;
    }
    attributes = new attribute_info[attributes_count];
    unsigned short limit = attributes_count;
    for(unsigned short i=0; i != limit; i++){
        attributes[i] = attribute_info(cpy.attributes[i]);
    }
}
field_info & field_info::operator= (const field_info& rs)
{
    if(this == &rs)
        return *this;
    this ->~field_info();
    access_flags        = rs.access_flags;
    name_index          = rs.name_index;
    descriptor_index    = rs.descriptor_index;
    attributes_count    = rs.attributes_count;
    if(!attributes_count){
        attributes  = NULL;
        return *this;
    }
    attributes = new attribute_info[attributes_count];
    unsigned short limit = attributes_count;
    for(unsigned short i=0; i != limit; i++){
        attributes[i] = attribute_info(rs.attributes[i]);
    }
    return *this;
}
field_info::field_info(u2 af,u2 ni,u2 di,u2 ac,attribute_info *attrs)
{
    access_flags        = af;
    name_index          = ni;
    descriptor_index    = di;
    attributes_count    = ac;
    if(!attributes_count){
        attributes  = NULL;
        return;
    }
    attributes = new attribute_info[attributes_count];
    unsigned short limit = attributes_count;
    for(unsigned short i=0; i != limit; i++){
        attributes[i] = attribute_info(attrs[i]);
    }
}
void field_info::writeToFile(void *file_ptr)
{
    access_flags.writeToFile(file_ptr);
    name_index.writeToFile(file_ptr);
    descriptor_index.writeToFile(file_ptr);
    attributes_count.writeToFile(file_ptr);
    for(unsigned int i=0; i != attributes_count; i++){
        attributes[i].writeToFile(file_ptr);
    }
}
field_info::~field_info()
{
    if(attributes != NULL){
        delete[] attributes;
        attributes = NULL;
    }
}
void field_info::addNewAttribute(attribute_info newAttr, unsigned short index)
{
    if(attributes == NULL && !attributes_count){
        //We shouldn't have any attributes
        throw FieldInfoDoesNotAdmitAttributes();
    }
    if(attributes == NULL && attributes_count)
    {
        //Create a new attributes array
        attributes = new attribute_info[attributes_count];
        if(!attributes)
            throw NoMemoryError();
    }
    //If we reach here we have an attributes array, we make sure
    //That index is within range
    if(index >= attributes_count)
        throw FieldInfoNewAttributeIndexGraterThanCapacity();
    attributes[index] = newAttr;
    
}
/****************************************************************
 *                      field_info struct                       *
 *                             end                              *
 ****************************************************************/

#pragma mark -

#pragma mark method_info structure

/****************************************************************
 *                      method_info struct                      *
 *                            start                             *
 ****************************************************************/

method_info::method_info()
{
    access_flags        = u2((unsigned short)0);
    name_index          = u2((unsigned short)0);
    descriptor_index    = u2((unsigned short)0);
    attributes_count    = u2((unsigned short)0);
    attributes          = NULL;
}
method_info::method_info(const method_info& cpy)
{
    access_flags        = cpy.access_flags;
    name_index          = cpy.name_index;
    descriptor_index    = cpy.descriptor_index;
    attributes_count    = cpy.attributes_count;
    if(!attributes_count){
        attributes  = NULL;
        return;
    }
    attributes = new attribute_info[attributes_count];
    unsigned short limit = attributes_count;
    for(unsigned short i=0; i != limit; i++){
        attributes[i] = attribute_info(cpy.attributes[i]);
    }
}
method_info & method_info::operator= (const method_info& rs)
{
    if(this == &rs)
        return *this;
    this ->~method_info();
    access_flags        = rs.access_flags;
    name_index          = rs.name_index;
    descriptor_index    = rs.descriptor_index;
    attributes_count    = rs.attributes_count;
    if(!attributes_count){
        attributes  = NULL;
        return *this;
    }
    attributes = new attribute_info[attributes_count];
    unsigned short limit = attributes_count;
    for(unsigned short i=0; i != limit; i++){
        attributes[i] = attribute_info(rs.attributes[i]);
    }
    return *this;
}
method_info::method_info(u2 af,u2 ni,u2 di,u2 ac,attribute_info *attrs)
{
    access_flags        = af;
    name_index          = ni;
    descriptor_index    = di;
    attributes_count    = ac;
    if(!attributes_count){
        attributes  = NULL;
        return;
    }
    attributes = new attribute_info[attributes_count];
    unsigned short limit = attributes_count;
    for(unsigned short i=0; i != limit; i++){
        attributes[i] = attribute_info(attrs[i]);
    }
}
void method_info::writeToFile(void *file_ptr)
{
    access_flags.writeToFile(file_ptr);
    name_index.writeToFile(file_ptr);
    descriptor_index.writeToFile(file_ptr);
    attributes_count.writeToFile(file_ptr);
    for(unsigned int i=0; i != attributes_count; i++){
        attributes[i].writeToFile(file_ptr);
    }
}
method_info::~method_info()
{
    if(attributes != NULL){
        delete[] attributes;
        attributes = NULL;
    }
}
void method_info::addNewAttribute(attribute_info newAttr, unsigned short index)
{
    if(attributes == NULL && !attributes_count){
        //We shouldn't have any attributes
        throw FieldInfoDoesNotAdmitAttributes();
    }
    if(attributes == NULL && attributes_count)
    {
        //Create a new attributes array
        attributes = new attribute_info[attributes_count];
        if(!attributes)
            throw NoMemoryError();
    }
    //If we reach here we have an attributes array, we make sure
    //That index is within range
    if(index >= attributes_count)
        throw FieldInfoNewAttributeIndexGraterThanCapacity();
    attributes[index] = newAttr;
    
}
/****************************************************************
 *                      method_info struct                      *
 *                             end                              *
 ****************************************************************/

#pragma mark -

/****************************************************************
 *                  attribute_info struct                       *
 *                           start                              *
 ****************************************************************/

#pragma mark attribute_info struct

attribute_info::attribute_info(){
    attribute_name_index = u2((unsigned short)0);
    attribute_length = u4((unsigned int)0);
    info = NULL;
    _ownsInfoMemory = false;
}
attribute_info::attribute_info(const attribute_info& cpy)
{
    attribute_name_index    = cpy.attribute_name_index;
    attribute_length        = cpy.attribute_length;
    if(attribute_length){
        info = (u1 *)malloc(sizeof(u1)*attribute_length);
        memcpy(info, cpy.info,sizeof(u1)*attribute_length);
    }else{
        info = NULL;
    }
    _ownsInfoMemory = true;
}
attribute_info & attribute_info::operator= (const attribute_info& rs)
{
    if(this == &rs)
        return *this;
    this ->~attribute_info();
    attribute_name_index    = rs.attribute_name_index;
    attribute_length        = rs.attribute_length;
    if(attribute_length){
        info = (u1 *)malloc(sizeof(u1)*attribute_length);
        memcpy(info, rs.info,sizeof(u1)*attribute_length);
    }else{
        info = NULL;
    }
    _ownsInfoMemory = true;
    return *this;
}
void attribute_info::writeToFile(void *file_ptr)
{
    attribute_name_index.writeToFile(file_ptr);
    attribute_length.writeToFile(file_ptr);
    fwrite(info, sizeof(u1), attribute_length, (FILE *)file_ptr);
}
attribute_info::~attribute_info()
{
    if(_ownsInfoMemory && info != NULL){
        free(info);
        info = NULL;
    }
}

/****************************************************************
 *                  attribute_info struct                       *
 *                           end                                *
 ****************************************************************/

#pragma mark -

#pragma mark Attributes definitions
/************************************************************
 *                    Attributes definitions                *
 *                            Start                         *
 ***********************************************************/
ConstantValue_attribute::ConstantValue_attribute(unsigned short index)
{
    constantvalue_index = u2(index);
}

ConstantValue_attribute::
ConstantValue_attribute(const ConstantValue_attribute& rs)
{
    constantvalue_index = rs.constantvalue_index;
}
