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
    
    unsigned char *writeToArray(unsigned char *bytecodeArray);
    
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
    
    unsigned char *writeToArray(unsigned char *bytecodeArray);
    
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
    unsigned int size();
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
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
    unsigned int size();
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
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
    unsigned int size();
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
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
    CONSTANT_Class_info(const unsigned char *bytecode);
    CONSTANT_Class_info(const CONSTANT_Class_info& cpy);
    CONSTANT_Class_info(u2 index);
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
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
    CONSTANT_Fieldref_info(const unsigned char *bytecode);
    CONSTANT_Fieldref_info(const CONSTANT_Fieldref_info& cpy);
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
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
    CONSTANT_Methodref_info(const unsigned char *bytecode);
    CONSTANT_Methodref_info(const CONSTANT_Methodref_info& cpy);
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
    static inline unsigned int size()
    {
        return sizeof(u2)*2;
    }
}CONSTANT_Methodref_info;

typedef struct CONSTANT_InterfaceMethodref_info
{
    u2 class_index;
    u2 name_and_type_index;
    CONSTANT_InterfaceMethodref_info(const unsigned char *bytecode);
    CONSTANT_InterfaceMethodref_info(const CONSTANT_InterfaceMethodref_info&
                                     cpy);
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
    static inline unsigned int size()
    {
        return sizeof(u2)*2;
    }
}CONSTANT_InterfaceMethodref_info;

typedef struct CONSTANT_String_info
{
    u2 string_index;
    CONSTANT_String_info(const unsigned char *bytecode);
    CONSTANT_String_info(const CONSTANT_String_info& cpy);
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
    static inline unsigned int size()
    {
        return sizeof(u2);
    }
    
}CONSTANT_String_info;

typedef struct CONSTANT_Integer_info
{
    u4 bytes;
    CONSTANT_Integer_info(const unsigned char *bytecode);
    CONSTANT_Integer_info(const CONSTANT_Integer_info& cpy);
    CONSTANT_Integer_info(u4 integer);
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
    static inline unsigned int size()
    {
        return sizeof(u4);
    }
}CONSTANT_Integer_info;

typedef struct CONSTANT_Float_info
{
    u4 bytes;
    CONSTANT_Float_info(const unsigned char *bytecode);
    CONSTANT_Float_info(const CONSTANT_Float_info& cpy);
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
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
    CONSTANT_Long_info(const unsigned char *bytecode);
    CONSTANT_Long_info(const CONSTANT_Long_info& cpy);
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
    static inline unsigned int size()
    {
        return sizeof(u4)*2;
    }
}CONSTANT_Long_info;

typedef struct CONSTANT_Double_info
{
    u4 high_bytes;
    u4 low_bytes;
    CONSTANT_Double_info(const unsigned char *bytecode);
    CONSTANT_Double_info(const CONSTANT_Double_info& cpy);
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
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
    CONSTANT_NameAndType_info(const unsigned char *bytecode);
    CONSTANT_NameAndType_info(const CONSTANT_NameAndType_info& cpy);
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
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
    CONSTANT_Utf8_info(const unsigned char *bytecode);
    CONSTANT_Utf8_info(const CONSTANT_Utf8_info& cpy);
    CONSTANT_Utf8_info & operator= (const CONSTANT_Utf8_info& rs);
    CONSTANT_Utf8_info(unsigned short len, const char *string);
    ~CONSTANT_Utf8_info();
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
    inline unsigned int size()
    {
        return sizeof(u2) + sizeof(u1)*length;
    }
}CONSTANT_Utf8_info;

typedef struct CONSTANT_MethodHandle_info
{
    u1 reference_kind;
    u2 reference_index;
    
    CONSTANT_MethodHandle_info(const unsigned char *bytecode);
    CONSTANT_MethodHandle_info(const CONSTANT_MethodHandle_info& cpy);
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
    static inline unsigned int size()
    {
        return sizeof(u2)+sizeof(u1);
    }
    
}CONSTANT_MethodHandle_info;

typedef struct CONSTANT_MethodType_info
{
    u2 descriptor_index;
    CONSTANT_MethodType_info(const unsigned char *bytecode);
    CONSTANT_MethodType_info(const CONSTANT_MethodType_info& cpy);
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
    static inline unsigned int size()
    {
        return sizeof(u2);
    }
    
}CONSTANT_MethodType_info;

typedef struct CONSTANT_InvokeDynamic_info
{
    u2 bootstrap_method_attr_index;
    u2 name_and_type_index;
    CONSTANT_InvokeDynamic_info(const unsigned char *bytecode);
    CONSTANT_InvokeDynamic_info(const CONSTANT_InvokeDynamic_info& cpy);
    void writeToFile(void *file_ptr);
    unsigned char *writeToArray(unsigned char *array);
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
#pragma mark Opcodes definitions


/************************************************************
 *                     Opcodes definitions                  *
 *                           Start                          *
 ***********************************************************/

#pragma mark Constants
const u1 INSTRUCTION_nop        =0x00;
const u1 INSTRUCTION_aconst_null=0x01;
const u1 INSTRUCTION_iconst_m1  =0x02;
const u1 INSTRUCTION_iconst_0   =0x03;
const u1 INSTRUCTION_iconst_1   =0x04;
const u1 INSTRUCTION_iconst_2   =0x05;
const u1 INSTRUCTION_iconst_3   =0x06;
const u1 INSTRUCTION_iconst_4   =0x07;
const u1 INSTRUCTION_iconst_5   =0x08;
const u1 INSTRUCTION_lconst_0   =0x09;
const u1 INSTRUCTION_lconst_1   =0x0a;
const u1 INSTRUCTION_fconst_0   =0x0b;
const u1 INSTRUCTION_fconst_1   =0x0c;
const u1 INSTRUCTION_fconst_2   =0x0d;
const u1 INSTRUCTION_dconst_0   =0x0e;
const u1 INSTRUCTION_dconst_1   =0x0f;
const u1 INSTRUCTION_bipush     =0x10;
const u1 INSTRUCTION_sipush     =0x11;
const u1 INSTRUCTION_ldc        =0x12;
const u1 INSTRUCTION_ldc_w      =0x13;
const u1 INSTRUCTION_ldc2_w     =0x14;
#pragma mark -
#pragma mark Loads
const u1 INSTRUCTION_iload  =0x15;
const u1 INSTRUCTION_lload  =0x16;
const u1 INSTRUCTION_fload  =0x17;
const u1 INSTRUCTION_dload  =0x18;
const u1 INSTRUCTION_aload  =0x19;
const u1 INSTRUCTION_iload_0=0x1a;
const u1 INSTRUCTION_iload_1=0x1b;
const u1 INSTRUCTION_iload_2=0x1c;
const u1 INSTRUCTION_iload_3=0x1d;
const u1 INSTRUCTION_lload_0=0x1e;
const u1 INSTRUCTION_lload_1=0x1f;
const u1 INSTRUCTION_lload_2=0x20;
const u1 INSTRUCTION_lload_3=0x21;
const u1 INSTRUCTION_fload_0=0x22;
const u1 INSTRUCTION_fload_1=0x23;
const u1 INSTRUCTION_fload_2=0x24;
const u1 INSTRUCTION_fload_3=0x25;
const u1 INSTRUCTION_dload_0=0x26;
const u1 INSTRUCTION_dload_1=0x27;
const u1 INSTRUCTION_dload_2=0x28;
const u1 INSTRUCTION_dload_3=0x29;
const u1 INSTRUCTION_aload_0=0x2a;
const u1 INSTRUCTION_aload_1=0x2b;
const u1 INSTRUCTION_aload_2=0x2c;
const u1 INSTRUCTION_aload_3=0x2d;
const u1 INSTRUCTION_iaload =0x2e;
const u1 INSTRUCTION_laload =0x2f;
const u1 INSTRUCTION_faload =0x30;
const u1 INSTRUCTION_daload =0x31;
const u1 INSTRUCTION_aaload =0x32;
const u1 INSTRUCTION_baload =0x33;
const u1 INSTRUCTION_caload =0x34;
const u1 INSTRUCTION_saload =0x35;
#pragma mark -
#pragma mark Stores
const u1 INSTRUCTION_istore     =0x36;
const u1 INSTRUCTION_lstore     =0x37;
const u1 INSTRUCTION_fstore     =0x38;
const u1 INSTRUCTION_dstore     =0x39;
const u1 INSTRUCTION_astore     =0x3a;
const u1 INSTRUCTION_istore_0   =0x3b;
const u1 INSTRUCTION_istore_1   =0x3c;
const u1 INSTRUCTION_istore_2   =0x3d;
const u1 INSTRUCTION_istore_3   =0x3e;
const u1 INSTRUCTION_lstore_0   =0x3f;
const u1 INSTRUCTION_lstore_1   =0x40;
const u1 INSTRUCTION_lstore_2   =0x41;
const u1 INSTRUCTION_lstore_3   =0x42;
const u1 INSTRUCTION_fstore_0   =0x43;
const u1 INSTRUCTION_fstore_1   =0x44;
const u1 INSTRUCTION_fstore_2   =0x45;
const u1 INSTRUCTION_fstore_3   =0x46;
const u1 INSTRUCTION_dstore_0   =0x47;
const u1 INSTRUCTION_dstore_1   =0x48;
const u1 INSTRUCTION_dstore_2   =0x49;
const u1 INSTRUCTION_dstore_3   =0x4a;
const u1 INSTRUCTION_astore_0   =0x4b;
const u1 INSTRUCTION_astore_1   =0x4c;
const u1 INSTRUCTION_astore_2   =0x4d;
const u1 INSTRUCTION_astore_3   =0x4e;
const u1 INSTRUCTION_iastore    =0x4f;
const u1 INSTRUCTION_lastore    =0x50;
const u1 INSTRUCTION_fastore    =0x51;
const u1 INSTRUCTION_dastore    =0x52;
const u1 INSTRUCTION_aastore    =0x53;
const u1 INSTRUCTION_bastore    =0x54;
const u1 INSTRUCTION_castore    =0x55;
const u1 INSTRUCTION_sastore    =0x56;
#pragma mark -
#pragma mark Stack
const u1 INSTRUCTION_pop    =0x57;
const u1 INSTRUCTION_pop2   =0x58;
const u1 INSTRUCTION_dup    =0x59;
const u1 INSTRUCTION_dup_x1 =0x5a;
const u1 INSTRUCTION_dup_x2 =0x5b;
const u1 INSTRUCTION_dup2   =0x5c;
const u1 INSTRUCTION_dup2_x1=0x5d;
const u1 INSTRUCTION_dup2_x2=0x5e;
const u1 INSTRUCTION_swap   =0x5f;
#pragma mark -
#pragma mark Math
const u1 INSTRUCTION_iadd   =0x60;
const u1 INSTRUCTION_ladd   =0x61;
const u1 INSTRUCTION_fadd   =0x62;
const u1 INSTRUCTION_dadd   =0x63;
const u1 INSTRUCTION_isub   =0x64;
const u1 INSTRUCTION_lsub   =0x65;
const u1 INSTRUCTION_fsub   =0x66;
const u1 INSTRUCTION_dsub   =0x67;
const u1 INSTRUCTION_imul   =0x68;
const u1 INSTRUCTION_lmul   =0x69;
const u1 INSTRUCTION_fmul   =0x6a;
const u1 INSTRUCTION_dmul   =0x6b;
const u1 INSTRUCTION_idiv   =0x6c;
const u1 INSTRUCTION_ldiv   =0x6d;
const u1 INSTRUCTION_fdiv   =0x6e;
const u1 INSTRUCTION_ddiv   =0x6f;
const u1 INSTRUCTION_irem   =0x70;
const u1 INSTRUCTION_lrem   =0x71;
const u1 INSTRUCTION_frem   =0x72;
const u1 INSTRUCTION_drem   =0x73;
const u1 INSTRUCTION_ineg   =0x74;
const u1 INSTRUCTION_lneg   =0x75;
const u1 INSTRUCTION_fneg   =0x76;
const u1 INSTRUCTION_dneg   =0x77;
const u1 INSTRUCTION_ishl   =0x78;
const u1 INSTRUCTION_lshl   =0x79;
const u1 INSTRUCTION_ishr   =0x7a;
const u1 INSTRUCTION_lshr   =0x7b;
const u1 INSTRUCTION_iushr  =0x7c;
const u1 INSTRUCTION_lushr  =0x7d;
const u1 INSTRUCTION_iand   =0x7e;
const u1 INSTRUCTION_land   =0x7f;
const u1 INSTRUCTION_ior    =0x80;
const u1 INSTRUCTION_lor    =0x81;
const u1 INSTRUCTION_ixor   =0x82;
const u1 INSTRUCTION_lxor   =0x83;
const u1 INSTRUCTION_iinc   =0x84;
#pragma mark -
#pragma mark Conversions
const u1 INSTRUCTION_i2l=0x85;
const u1 INSTRUCTION_i2f=0x86;
const u1 INSTRUCTION_i2d=0x87;
const u1 INSTRUCTION_l2i=0x88;
const u1 INSTRUCTION_l2f=0x89;
const u1 INSTRUCTION_l2d=0x8a;
const u1 INSTRUCTION_f2i=0x8b;
const u1 INSTRUCTION_f2l=0x8c;
const u1 INSTRUCTION_f2d=0x8d;
const u1 INSTRUCTION_d2i=0x8e;
const u1 INSTRUCTION_d2l=0x8f;
const u1 INSTRUCTION_d2f=0x90;
const u1 INSTRUCTION_i2b=0x91;
const u1 INSTRUCTION_i2c=0x92;
const u1 INSTRUCTION_i2s=0x93;
#pragma mark -
#pragma mark Comparisons
const u1 INSTRUCTION_lcmp       =0x94;
const u1 INSTRUCTION_fcmpl      =0x95;
const u1 INSTRUCTION_fcmpg      =0x96;
const u1 INSTRUCTION_dcmpl      =0x97;
const u1 INSTRUCTION_dcmpg      =0x98;
const u1 INSTRUCTION_ifeq       =0x99;
const u1 INSTRUCTION_ifne       =0x9a;
const u1 INSTRUCTION_iflt       =0x9b;
const u1 INSTRUCTION_ifge       =0x9c;
const u1 INSTRUCTION_ifgt       =0x9d;
const u1 INSTRUCTION_ifle       =0x9e;
const u1 INSTRUCTION_if_icmpeq  =0x9f;
const u1 INSTRUCTION_if_icmpne  =0xa0;
const u1 INSTRUCTION_if_icmplt  =0xa1;
const u1 INSTRUCTION_if_icmpge  =0xa2;
const u1 INSTRUCTION_if_icmpgt  =0xa3;
const u1 INSTRUCTION_if_icmple  =0xa4;
const u1 INSTRUCTION_if_acmpeq  =0xa5;
const u1 INSTRUCTION_if_acmpne  =0xa6;
#pragma mark -
#pragma mark Control
const u1 INSTRUCTION_goto           =0xa7;
const u1 INSTRUCTION_jsr            =0xa8;
const u1 INSTRUCTION_ret            =0xa9;
const u1 INSTRUCTION_tableswitch    =0xaa;
const u1 INSTRUCTION_lookupswitch   =0xab;
const u1 INSTRUCTION_ireturn        =0xac;
const u1 INSTRUCTION_lreturn        =0xad;
const u1 INSTRUCTION_freturn        =0xae;
const u1 INSTRUCTION_dreturn        =0xaf;
const u1 INSTRUCTION_areturn        =0xb0;
const u1 INSTRUCTION_return         =0xb1;
#pragma mark -
#pragma mark References
const u1 INSTRUCTION_getstatic          =0xb2;
const u1 INSTRUCTION_putstatic          =0xb3;
const u1 INSTRUCTION_getfield           =0xb4;
const u1 INSTRUCTION_putfield           =0xb5;
const u1 INSTRUCTION_invokevirtual      =0xb6;
const u1 INSTRUCTION_invokespecial      =0xb7;
const u1 INSTRUCTION_invokestatic       =0xb8;
const u1 INSTRUCTION_invokeinterface    =0xb9;
const u1 INSTRUCTION_invokedynamic      =0xba;
const u1 INSTRUCTION_new                =0xbb;
const u1 INSTRUCTION_newarray           =0xbc;
const u1 INSTRUCTION_anewarray          =0xbd;
const u1 INSTRUCTION_arraylength        =0xbe;
const u1 INSTRUCTION_athow              =0xbf;
const u1 INSTRUCTION_checkcast          =0xc0;
const u1 INSTRUCTION_instanceof         =0xc1;
const u1 INSTRUCTION_monitorenter       =0xc2;
const u1 INSTRUCTION_monitorexit        =0xc3;
#pragma mark -
#pragma mark Extended
const u1 INSTRUCTION_wide           =0xc4;
const u1 INSTRUCTION_multianewarray =0xc5;
const u1 INSTRUCTION_ifnull         =0xc6;
const u1 INSTRUCTION_ifnonnull      =0xc7;
const u1 INSTRUCTION_goto_w         =0xc8;
const u1 INSTRUCTION_jsr_w          =0xc9;
#pragma mark -
/************************************************************
 *                     Opcodes definitions                  *
 *                            End                           *
 ***********************************************************/


#endif /* class_file_defs_h */
