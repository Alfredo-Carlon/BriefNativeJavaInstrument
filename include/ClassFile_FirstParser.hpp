//
//  ClassFile_FirstParser.hpp
//  
//
//  Created by Alfredo Cruz on 9/30/16.
//
//

#ifndef ClassFile_FirstParser_hpp
#define ClassFile_FirstParser_hpp

#include <ClassFile_def.hpp>
#include <CodeMod_def.hpp>
#include <unordered_map>

/**
 * First 'quick intended' interface for reading and parsing
 * portions of a .class file
 **/

class ClassFile_FP {

private:
    ClassFile *_classFile;
    
    //Struct to hold defined names already in the Constant Pool.
    struct _dCPa {
        unsigned short ConstantValue;
        unsigned short ClassName;
        _dCPa(){
            ConstantValue = -1;
            ClassName = -1;
        }
    }_definedCPAttributes;
    
    const char *_className;
    
    
    //Counter for the next constant pool entry
    unsigned int _constant_pool_next_index;
    
    //Holds the size in bytes of the current bytecode
    unsigned long _size_in_bytes;
    
    //Array for modified versions of methods
    CodeMod *_modifiedMethods;
    
    //Size of modifiedMethods
    unsigned int _modifiedMethodsSize;
    
    //Fill the field_info & method_info structures array
    //structures.
    //fi        is a pointer to a field_info array
    //fi_count  is the number of fields/methods to fill
    //bytecode  is a pointer to the bytecode
    //returns   a pointer to the next byte in bytecode
    unsigned char * _fillFieldAttribute(field_info **,
                                              unsigned short,
                                              unsigned char *);
    
    
    //Fill the method_info structures array
    //structures.
    //fi        is a pointer to a field_info array
    //fi_count  is the number of fields/methods to fill
    //bytecode  is a pointer to the bytecode
    //returns   a pointer to the next byte in bytecode
    unsigned char * _fillMethodAttribute(method_info **,
                                              unsigned short,
                                              unsigned char *);
    
    //Pushes a value into _classFile's constant pool
    u2 _push_into_constant_pool(cp_info& data, bool updateSize);
    
    
    //Looks if a given Utf8 CP index matches the given name
    bool _cpEntryMatches(unsigned short entry, const char *str);
    
    //Loads an original method's code version into _modifiedMethods
    void _loadCodeModForMethod(unsigned short methodIndex);
    
    //Saves the modified versions of methods back to the methods array
    void _saveCodeModeForAllMethods();
public:
    
    //Default constructor
    ClassFile_FP();
    //Destructor
    ~ClassFile_FP();
    
    
    //Load the bytecode into _classFile
    //payload       is a pointer to the bytecode
    //payloadSize   is the size in bytes of the bytecode
    void loadBytecode(const unsigned char *payload, unsigned int payloadSize);
    
    //Dumps the bytecode to disk at 'filename'
    void dumpBytecode(const char *filename);
    
    //Dumps the bytecode to the buffer
    void dumpBytecodeToBuffer(unsigned char *buffer, unsigned long length);
    
    //Sets the name of the class for the bytecode
    void setClassName(const char *name);
    
    
    //Returns the number of elements in the
    //constant pool of _classFile
    unsigned int constantPoolSize();
    
    //Returns the number of fields
    //Declared in _classFile
    unsigned int fieldsSize();
    
    //Returns the number of methods
    //Declared in _classFile
    unsigned int methodsSize();
    
    //Returns the number of attributes
    //Declared in _classFile
    unsigned int attributesSize();
    
    //Returns the current bytecode's size
    unsigned long bytecodeSize();
    
    
    
    
    
    
#pragma mark Modifications
    
    /******************************************************
     * Constructs a ConstantValue attribute of type Long
     * and updates the Constant Pool.
     * Returns the attribute_info
     *****************************************************/
    attribute_info addConstantValue_Long(CONSTANT_Long_info value);
    
    
    /******************************************************
     * Adds a field to the bytecode
     * Performs some basic checks
     * Adds the name_index and descriptor_index based in
     * the fieldName and fieldDesc
     * data into the constant pool
     *****************************************************/
    void addStaticField(u2 access_flags,
                  const char *fieldName,
                  const char *fieldDesc,
                  u2 attributes_count,
                  attribute_info  *attributes);
    
    /******************************************************
     * Adds a native method to the bytecode
     * Performs some basic checks
     * Adds the name_index and descriptor_index based in
     * the methodName and methodDesc
     * data into the constant pool
     *****************************************************/
    
    void addNativeMethod(u2 access_flags,
                        const char *methodName,
                        const char *methodDesc);
    
    /***************************************************************
     * Adds a Fieldref_info structure into the CP and returns its
     * index
     * Adds fieldName, type and className to the CP.
     * If className is NULL, the name of the current class is used
     *
     ***************************************************************/
    u2 addFieldRef(const char *fieldName,
                         const char *type,
                         const char *className);
    
    /***************************************************************
     * Adds a Methodref_info structure into the CP and returns its
     * index
     * Adds methodName, type and className to the CP.
     * If className is NULL, the name of the current class is used
     *
     ***************************************************************/
    u2 addMethodRef(const char *methodName,
                    const char *signature,
                    const char *className);
    
    
    
    
    
    
#pragma mark -
#pragma mark Method's Code Modifications
    
    /******************************************************
     *
     *  The convention for bytecodes instruction is:
     *  add_<instructionName>(args);
     *  
     *  As much as possible, if <instructionName> depends
     *  On an index of a data in the Constant Pool, then
     *  two versions should be available: 1. args are all
     *  the arguments required for adding the data into the CP
     *  2. args is the index of the data in the CP
     *
     *****************************************************/
private:
    
    /******************************************************
     *
     * Method that inserts an opcode and a CP index into
     * the bytecode.
     * methodIndex      - The index of the method.
     * instOffset       - The offset byte in the method's bytecode
     * cpIndexOfData    - The index in the Constant Pool
     * opcode           - The opcode to insert
     *
     ******************************************************/
    void insert_opcode(unsigned short   methodIndex,
                       unsigned short   instOffset,
                       unsigned short   cpIndexOfData,
                       u1 opcode);
    
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
    void shiftExceptionTable(u1 *codeAttribPtr, unsigned char amount);
    
public:

    
    /******************************************************
     * Adds a aload_n instruction into the bytecode
     * at the offset 'instOffset'
     *
     *****************************************************/
    
    void add_aload_n(unsigned short methodIndex,
                     unsigned short instOffset,
                     u1 index);
    
    
    
    /******************************************************
     * Adds a getstatic bytecode instruction
     * to the method at 'methodIndex'
     * For the field with name 'fieldName'
     * The field type given by 'fieldType'
     * And the class name given by 'className'
     * If className is NULL, the name of the current class is used
     * Push all the required data into the CP
     * The instruction is put at the byte with index byteIndex
     * This instruction calls addStaticFieldRef method
     *****************************************************/
    
    void add_getstatic(unsigned short   methodIndex,
                       unsigned short   instOffset,
                       const char       *fieldName,
                       const char       *fieldType,
                       const char       *className);
    
    void add_getstatic(unsigned short   methodIndex,
                       unsigned short   instOffset,
                       unsigned short   cpIndexOfData);
    
    
    /******************************************************
     * Adds an invokestatic bytecode instruction
     * to the method at 'methodIndex'
     * To the method with name 'methodName'
     * 'methodSignature' is the signature of the method
     * And the class name given by 'className'
     * If className is NULL, the name of the current class is used
     * Push all the required data into the CP
     * The instruction is put at the byte with index byteIndex
     * This instruction calls addMethoddRef method
     *****************************************************/
    
    void add_invokestatic(unsigned short    methodIndex,
                          unsigned short    instOffset,
                          const char        *methodName,
                          const char        *methodSignature,
                          const char        *className);
    
    void add_invokestatic(unsigned short    methodIndex,
                          unsigned short    instOffset,
                          unsigned short    cpIndexOfData);
    
    
    
#pragma mark -
    
#pragma mark Static methods
public:
    
    
    /************************************************************************
     *                                                                      *
     *  Return the bytecode representations of Java types                   *
     *                                                                      *
     ************************************************************************/
    
    static const char *voidTypeToBytecode()     {return "V";}
    
    static const char *byteTypeToBytecode()     {return "B";}
    
    static const char *charTypeToBytecode()     {return "C";}
    
    static const char *doubleTypeToBytecode()   {return "D";}
    
    static const char *floatTypeToBytecode()    {return "F";}
    
    static const char *intTypeToBytecode()      {return "I";}
    
    static const char *longTypeToBytecode()     {return "J";}
    
    static const char *shortTypeToBytecode()    {return "S";}
    
    static const char *booleanTypeToBytecode()  {return "Z";}
    
    static const char *arrayTypeToBytecode()    {return "[";}
    
    //Returns the field type of a class
    static const char *classRefTypeToBytecode(
                      const char *reference)    {
        
        unsigned int ref_len = (unsigned int)strlen(reference);
        char *ref = (char *)malloc(sizeof(char)*
                                         (ref_len + 2));
        
        ref[0] = 'L';
        memcpy(ref+1, reference, sizeof(char)*ref_len);
        ref[ref_len+2] = ';';
        return ref;
    }
    
private:
    //Returns the string of the access & properties flags
    //If the method flag is set, then access_flags belongs to a method
    //Else it belongs to a field
    static const char *_accessModifiersToString(bool method,u2 access_flags)
    {
        if(access_flags == u2((unsigned short)0))
            return NULL;
        char *flags[14];
        bzero(flags, sizeof(char *)*14);
        unsigned char index = 0; //Next free index in flags
        unsigned long size = 0;
        if(access_flags & ACC_PUBLIC){
            flags[index] = (char *)malloc(7);
            strcpy(flags[index], "PUBLIC");
            size += 7;
            index++;
        }
        if(access_flags & ACC_PRIVATE)
        {
            flags[index] = (char *)malloc(8);
            strcpy(flags[index], "PRIVATE");
            size += 8;
            index++;
        }
        if(access_flags & ACC_PROTECTED)
        {
            flags[index] = (char *)malloc(10);
            strcpy(flags[index], "PROTECTED");
            size += 10;
            index++;
        }
        if(access_flags & ACC_STATIC)
        {
            flags[index] = (char *)malloc(10);
            strcpy(flags[index], "PROTECTED");
            size += 10;
            index++;
        }
        if(access_flags & ACC_FINAL)
        {
            flags[index] = (char *)malloc(6);
            strcpy(flags[index], "FINAL");
            size += 6;
            index++;
        }
        if(access_flags & ACC_SYNCHRONIZED)
        {
            flags[index] = (char *)malloc(13);
            strcpy(flags[index], "SYNCHRONIZED");
            size += 13;
            index++;
        }
        if((access_flags & ACC_VOLATILE) && !method){
            flags[index] = (char *)malloc(9);
            strcpy(flags[index], "VOLATILE");
            size += 9;
            index++;
        }
        if((access_flags & ACC_BRIDGE) && method){
            flags[index] = (char *)malloc(7);
            strcpy(flags[index], "BRIDGE");
            size += 7;
            index++;
        }
        if((access_flags & ACC_TRANSIENT) && !method){
            flags[index] = (char *)malloc(10);
            strcpy(flags[index], "TRANSIENT");
            size += 10;
            index++;
        }
        if((access_flags & ACC_VARARGS) && method){
            flags[index] = (char *)malloc(8);
            strcpy(flags[index], "VARARGS");
            size += 8;
            index++;
        }
        if(access_flags & ACC_NATIVE)
        {
            flags[index] = (char *)malloc(7);
            strcpy(flags[index], "NATIVE");
            size += 8;
            index++;
        }
        if(access_flags & ACC_ENUM)
        {
            flags[index] = (char *)malloc(5);
            strcpy(flags[index], "ENUM");
            size += 5;
            index++;
        }
        if(access_flags & ACC_ABSTRACT)
        {
            flags[index] = (char *)malloc(9);
            strcpy(flags[index], "ABSTRACT");
            size += 9;
            index++;
        }
        if(access_flags & ACC_SYNTHETIC)
        {
            flags[index] = (char *)malloc(10);
            strcpy(flags[index], "SYNTHETIC");
            size += 10;
            index++;
        }
        char *retChar = (char *)malloc(size);
        unsigned short offset = 0;
        for(unsigned char i=0; i != 14; i++){
            strcpy(retChar+offset, flags[i]);
            offset += strlen(flags[i]);
            if(i != 13 && !flags[i+1])
                break;
            retChar[offset++] = ',';
        }
        retChar[offset] = 0;
        return retChar;
    }
    
#pragma mark -
    
#pragma mark Printing
public:
    void printMethodAt(unsigned short index);
#pragma mark -
    
#pragma mark Basic Queries
    //Returns true if the name of the method at index methodIndex
    //equals the given 'methodName'. Returns False otherwise
    bool methodNameEquals(unsigned short methodIndex,
                          const char *methodName);
    
    //Returns true if the class has the argument access_flags
    //set. False otherwise
    bool classHasAccess(u2 queryAccessFlags);
    
    //Returns true if the method has the argument access_flags
    //set. False otherwise
    bool methodHasAccess(unsigned short methodIndex, u2 queryAccessFlags);
    
    
#pragma mark -
};




#endif /* ClassFile_FirstParser_hpp */
