//
//  class_file_defs.h
//
//
//  Created by Alfredo Cruz on 9/30/16.
//
//

#ifndef class_file_defs_h
#define class_file_defs_h

/*
 *  Definition of types in the ClassFile structure
 *  and its types according to:
 *  The Java Virtual Machine Specification Java SE 8 Edition
 */

#include <cstdlib>
#include <cstring>
#include <ClassFile_errs.hpp>


/* u1, u2, u4 & u8 definitions */

#pragma mark Basic Types

typedef unsigned char u1;

typedef struct __attribute__((__packed__)) u2 { u1 u[2];
    
    u2();
    
    u2(unsigned short s);
    
    u2(const unsigned char *bytecode);
    
    u2(const u2& cpy);
    
    void writeToFile(void *file_ptr);
    
    void writeToArray(unsigned char *bytecodeArray);
    
    inline unsigned short uShort() const{
        return (unsigned short)
        (u[0] << 8 | u[1]);
    }
    
    //Logical operators
    inline u2 operator&(const u2& rs) const
    {
        return u2((unsigned short)((u[0] & rs.u[0])<<8|(u[1] & rs.u[1])));
    }
    inline u2 operator|(const u2& rs) const
    {
        return u2((unsigned short)((u[0] | rs.u[0])<<8|(u[1] | rs.u[1])));
    }
    
    
    //Assignment operators
    inline void operator|= (const u2& rs)
    {
        u[0] |= rs.u[0];
        u[1] |= rs.u[1];
    }
    inline void operator += (const u2& rs)
    {
        u2 res(this ->uShort() + rs.uShort());
        u[0] = res.u[0];
        u[1] = res.u[1];
    }
    
    
    
    //Boolean operators
    inline bool operator==(const u2& rs) const
    {
        return (u[0] == rs.u[0]) && (u[1] == rs.u[1]);
    }
    
    inline bool operator!=(const u2& rs) const
    {
        return !(*this == rs);
    }
    
    inline operator unsigned short() const
    {
        return (unsigned short)(this ->uShort());
    }
} u2;

typedef struct __attribute__((__packed__)) u4 { u1 u[4];
    
    u4();
    
    u4(const u4& cpy);
    
    u4(unsigned int ui);
    
    u4(const unsigned char *bytecode);
    
    void writeToFile(void *file_ptr);
    
    void writeToArray(unsigned char *bytecodeArray);
    
    inline unsigned int uInt() const
    {
        return (unsigned int)
        (u[0] << 24 | u[1] << 16 | u[2] << 8 | u[3]);
    }
    
    inline bool operator==(const u4& rs) const
    {
        return (u[0] == rs.u[0]) && (u[1] == rs.u[1]) &&
        (u[2] == rs.u[2]) && (u[3] == rs.u[3]);
    }
    inline u4 operator&(const u4& rs) const
    {
        return u4((unsigned short)((u[0] & rs.u[0])<<24|
                                   (u[1] & rs.u[1])<<16|
                                   (u[2] & rs.u[2])<<8|
                                   (u[3] & rs.u[3])));
    }
    inline bool operator!=(const u4& rs) const
    {
        return !(*this == rs);
    }
    
    inline void operator += (const u4& rs)
    {
        u4 res(this ->uInt() + rs.uInt());
        u[0] = res.u[0];
        u[1] = res.u[1];
        u[2] = res.u[2];
        u[3] = res.u[3];
    }
    
    inline operator unsigned int() const
    {
        return (unsigned int)(this ->uInt());
    }
} u4;

#pragma mark -

#pragma mark ClassFile Structures
/*  cp_info definition   */
typedef struct cp_info {
    u1  tag;
    void  *info;
    //---- Out of specification members ----//
    
    //Flag to indicate info memory's ownership
    bool _ownsInfoMemory;
    cp_info();
    cp_info(const cp_info& cpy);
    cp_info & operator= (const cp_info& rs);
    void claimInfoMemory();
    void relinquishInfoMemory();
    ~cp_info();
}cp_info;

/*  attribute_info definition   */
typedef struct attribute_info
{
    u2  attribute_name_index;
    u4  attribute_length;
    u1  *info;
    //Flag to indicate info memory's ownership
    bool _ownsInfoMemory;
    attribute_info();
    attribute_info(const attribute_info& cpy);
    attribute_info & operator= (const attribute_info& rs);
    void writeToFile(void *file_ptr);
    ~attribute_info();
} attribute_info;


/*  field_info definition   */
typedef struct field_info {
    u2              access_flags;
    u2              name_index;
    u2              descriptor_index;
    u2              attributes_count;
    attribute_info  *attributes;
    field_info();
    field_info(const field_info& cpy);
    field_info & operator= (const field_info& rs);
    field_info(u2,u2,u2,u2,attribute_info *);
    void writeToFile(void *file_ptr);
    ~field_info();
    //Adds a new attribute to attributes
    void addNewAttribute(attribute_info newAttr, unsigned short index);
    
}field_info;


/*  method_info definition    */
typedef struct method_info
{
    u2              access_flags;
    u2              name_index;
    u2              descriptor_index;
    u2              attributes_count;
    attribute_info  *attributes;
    method_info();
    method_info(const method_info& cpy);
    method_info & operator= (const method_info& rs);
    method_info(u2,u2,u2,u2,attribute_info *);
    void writeToFile(void *file_ptr);
    ~method_info();
    //Adds a new attribute to attributes
    void addNewAttribute(attribute_info newAttr, unsigned short index);
} method_info;


//We use void * for the constant_pool, fields, methods and
//attribute_info to use implementation dependent
//variable containers

typedef struct ClassFile {
    u4      magic;
    u2      minor_version;
    u2      major_version;
    u2      constant_pool_count;
    void    *constant_pool;
    u2      access_flags;
    u2      this_class;
    u2      super_class;
    u2      interfaces_count;
    u2      *interfaces;
    u2      fields_count;
    void    *fields;
    u2      methods_count;
    void    *methods;
    u2      attributes_count;
    void    *attributes;
    
    
} ClassFile;
#pragma mark -

#pragma mark Constant pool definitions

/************************************************************
 *                 Constant pool definitions                *
 *                           Start                          *
 ***********************************************************/
const u1 CONSTANT_Class                 =   7;
const u1 CONSTANT_Fieldref              =   9;
const u1 CONSTANT_Methodref             =   10;
const u1 CONSTANT_InterfaceMethodref    =   11;
const u1 CONSTANT_String                =   8;
const u1 CONSTANT_Integer               =   3;
const u1 CONSTANT_Float                 =   4;
const u1 CONSTANT_Long                  =   5;
const u1 CONSTANT_Double                =   6;
const u1 CONSTANT_NameAndType           =   12;
const u1 CONSTANT_Utf8                  =   1;
const u1 CONSTANT_MethodHandle          =   15;
const u1 CONSTANT_MethodType            =   16;
const u1 CONSTANT_InvokeDynamic         =   18;


//Definitions of the constant pool structures and their
//construction from bytecode.
//The structs do not contain the field tag as in
//The Java Virtual Machine Specification Java SE 8 Edition
//because its a 'merge' with cp_info
//All structs are packed just in case of padding


typedef struct CONSTANT_Class_info
{
    u2 name_index;
    CONSTANT_Class_info(unsigned char *bytecode);
    CONSTANT_Class_info(const CONSTANT_Class_info& cpy);
    CONSTANT_Class_info(u2 index);
    void writeToFile(void *file_ptr);
    static inline unsigned int size()
    {
        return sizeof(u2);
    }
}CONSTANT_Class_info;

typedef struct CONSTANT_Fieldref_info
{
    u2 class_index;
    u2 name_and_type_index;
    CONSTANT_Fieldref_info();
    CONSTANT_Fieldref_info(unsigned char *bytecode);
    CONSTANT_Fieldref_info(const CONSTANT_Fieldref_info& cpy);
    void writeToFile(void *file_ptr);
    static inline unsigned int size()
    {
        return sizeof(u2)*2;
    }
}CONSTANT_Fieldref_info;

typedef struct CONSTANT_Methodref_info
{
    u2 class_index;
    u2 name_and_type_index;
    CONSTANT_Methodref_info();
    CONSTANT_Methodref_info(unsigned char *bytecode);
    CONSTANT_Methodref_info(const CONSTANT_Methodref_info& cpy);
    void writeToFile(void *file_ptr);
    static inline unsigned int size()
    {
        return sizeof(u2)*2;
    }
}CONSTANT_Methodref_info;

typedef struct CONSTANT_InterfaceMethodref_info
{
    u2 class_index;
    u2 name_and_type_index;
    CONSTANT_InterfaceMethodref_info(unsigned char *bytecode);
    CONSTANT_InterfaceMethodref_info(const CONSTANT_InterfaceMethodref_info&
                                     cpy);
    void writeToFile(void *file_ptr);
    static inline unsigned int size()
    {
        return sizeof(u2)*2;
    }
}CONSTANT_InterfaceMethodref_info;

typedef struct CONSTANT_String_info
{
    u2 string_index;
    CONSTANT_String_info(unsigned char *bytecode);
    CONSTANT_String_info(const CONSTANT_String_info& cpy);
    void writeToFile(void *file_ptr);
    static inline unsigned int size()
    {
        return sizeof(u2);
    }
    
}CONSTANT_String_info;

typedef struct CONSTANT_Integer_info
{
    u4 bytes;
    CONSTANT_Integer_info(unsigned char *bytecode);
    CONSTANT_Integer_info(const CONSTANT_Integer_info& cpy);
    CONSTANT_Integer_info(u4 integer);
    void writeToFile(void *file_ptr);
    static inline unsigned int size()
    {
        return sizeof(u4);
    }
}CONSTANT_Integer_info;

typedef struct CONSTANT_Float_info
{
    u4 bytes;
    CONSTANT_Float_info(unsigned char *bytecode);
    CONSTANT_Float_info(const CONSTANT_Float_info& cpy);
    void writeToFile(void *file_ptr);
    static inline unsigned int size()
    {
        return sizeof(u4);
    }
}CONSTANT_Float_info;

typedef struct CONSTANT_Long_info
{
    u4 high_bytes;
    u4 low_bytes;
    CONSTANT_Long_info();
    CONSTANT_Long_info(unsigned char *bytecode);
    CONSTANT_Long_info(const CONSTANT_Long_info& cpy);
    void writeToFile(void *file_ptr);
    static inline unsigned int size()
    {
        return sizeof(u4)*2;
    }
}CONSTANT_Long_info;

typedef struct CONSTANT_Double_info
{
    u4 high_bytes;
    u4 low_bytes;
    CONSTANT_Double_info(unsigned char *bytecode);
    CONSTANT_Double_info(const CONSTANT_Double_info& cpy);
    void writeToFile(void *file_ptr);
    static inline unsigned int size()
    {
        return sizeof(u4)*2;
    }
}CONSTANT_Double_info;

typedef struct CONSTANT_NameAndType_info
{
    u2 name_index;
    u2 descriptor_index;
    CONSTANT_NameAndType_info();
    CONSTANT_NameAndType_info(unsigned char *bytecode);
    CONSTANT_NameAndType_info(const CONSTANT_NameAndType_info& cpy);
    void writeToFile(void *file_ptr);
    static inline unsigned int size()
    {
        return sizeof(u2)*2;
    }
}CONSTANT_NameAndType_info;

typedef struct CONSTANT_Utf8_info
{
    u2 length;
    u1 *bytes;
    CONSTANT_Utf8_info();
    CONSTANT_Utf8_info(unsigned char *bytecode);
    CONSTANT_Utf8_info(const CONSTANT_Utf8_info& cpy);
    CONSTANT_Utf8_info & operator= (const CONSTANT_Utf8_info& rs);
    CONSTANT_Utf8_info(unsigned short len, const char *string);
    ~CONSTANT_Utf8_info();
    void writeToFile(void *file_ptr);
    inline unsigned int size()
    {
        return sizeof(u2) + sizeof(u1)*length;
    }
}CONSTANT_Utf8_info;

typedef struct CONSTANT_MethodHandle_info
{
    u1 reference_kind;
    u2 reference_index;
    
    CONSTANT_MethodHandle_info(unsigned char *bytecode);
    CONSTANT_MethodHandle_info(const CONSTANT_MethodHandle_info& cpy);
    void writeToFile(void *file_ptr);
    static inline unsigned int size()
    {
        return sizeof(u2)+sizeof(u1);
    }
    
}CONSTANT_MethodHandle_info;

typedef struct CONSTANT_MethodType_info
{
    u2 descriptor_index;
    CONSTANT_MethodType_info(unsigned char *bytecode);
    CONSTANT_MethodType_info(const CONSTANT_MethodType_info& cpy);
    void writeToFile(void *file_ptr);
    static inline unsigned int size()
    {
        return sizeof(u2);
    }
    
}CONSTANT_MethodType_info;

typedef struct CONSTANT_InvokeDynamic_info
{
    u2 bootstrap_method_attr_index;
    u2 name_and_type_index;
    CONSTANT_InvokeDynamic_info(unsigned char *bytecode);
    CONSTANT_InvokeDynamic_info(const CONSTANT_InvokeDynamic_info& cpy);
    void writeToFile(void *file_ptr);
    static inline unsigned int size()
    {
        return sizeof(u2)*2;
    }
}CONSTANT_InvokeDynamic_info;
/************************************************************
 *                 Constant pool definitions                *
 *                            End                           *
 ***********************************************************/
#pragma mark -

#pragma mark Fields/Methods access definitions

/************************************************************
 *                  Fields/Methods definitions              *
 *                           Start                          *
 ***********************************************************/

const u2 ACC_PUBLIC         (0x0001);
const u2 ACC_PRIVATE        (0x0002);
const u2 ACC_PROTECTED      (0x0004);
const u2 ACC_STATIC         (0x0008);
const u2 ACC_FINAL          (0x0010);
const u2 ACC_SYNCHRONIZED   (0x0020);
const u2 ACC_BRIDGE         (0x0040);
const u2 ACC_VOLATILE       (0x0040);
const u2 ACC_TRANSIENT      (0x0080);
const u2 ACC_VARARGS        (0x0080);
const u2 ACC_NATIVE         (0x0100);
const u2 ACC_ENUM           (0x4000);
const u2 ACC_ABSTRACT       (0x0400);
const u2 ACC_SYNTHETIC      (0x1000);

/************************************************************
 *                  Fields/Methods definitions              *
 *                            End                           *
 ***********************************************************/
#pragma mark -

#pragma mark Attributes structures definitions
/************************************************************
 *                    Attributes definitions                *
 *                            Start                         *
 ***********************************************************/

typedef struct __attribute__((__packed__)) ConstantValue_attribute
{
    u2 constantvalue_index;
    //Constructs a ConstantValue attribute with index.
    ConstantValue_attribute();
    ConstantValue_attribute(const ConstantValue_attribute& rs);
    ConstantValue_attribute(unsigned short index);
    
}ConstantValue_attribute;

typedef struct __attribute__((__packed__)) Code_attribute
{
    u2 max_stack;
    u2 max_locals;
    u4 code_length;
    u1 *code;
    u2 exception_table_length;
    typedef struct
    {
        u2 start_pc;
        u2 end_pc;
        u2 handler_pc;
        u2 catch_type;
    }*exception_table;
    u2 attributes_count;
    attribute_info *attributes;
}Code_attribute;


/*typedef union stack_map_frame
 {
 same_frame,
 same_locals_1_stack_item_frame,
 same_locals_1_stack_item_frame_extended,
 chop_frame,
 same_frame_extended,
 append_frame,
 full_frame
 }stack_map_frame;*/

typedef struct StackMapTable_attribute
{
    u2              number_of_entries;
    //stack_map_frame *entries;
}StackMapTable_attribute;

/************************************************************
 *                    Attributes definitions                *
 *                             End                          *
 ***********************************************************/
#pragma mark -

#endif /* class_file_defs_h */
