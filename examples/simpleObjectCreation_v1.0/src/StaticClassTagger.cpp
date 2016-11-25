

/*******************************************************************************
 *                                                                             *
 *                                                                             *
 *                          Simple class tagger                                *
 *                                                                             *
 * Tags every loaded (into a given JVM) class with a static tag and            *
 * reports (fprintf) every constructor called                                  *
 *                                                                             *
 *                                                                             *
 * ****************************************************************************/


#include <StaticClassTagger.hpp>
#include <iostream>
#include <cstdio>


StaticClassTagger::StaticClassTagger()
{
    _bytecodeParser         = NULL;
    _curntTag               = 0;
    _classTagged            = false;
    _canTag                 = true;
    _tagName                = "__ClassTag__";
    _reportMethodName       = "sayHello";
    _reportMethodSignature  = "(J)V";
    _reportMethodClass      = "java/lang/Object";
}


//Load the given class into the 'working memory'
void StaticClassTagger::loadClass(const unsigned char *originalBytecode,
               unsigned int len,
               const char *className)
{
    if(_bytecodeParser)
        delete _bytecodeParser;
    _className = className;
    //std::cout << "Loading Class: " << _className << std::endl;
    _bytecodeParser = new ClassFile_FP();
    _bytecodeParser ->loadBytecode(originalBytecode, len);
    _bytecodeParser ->setClassName(className);
    if(_bytecodeParser ->classHasAccess(ACC_ABSTRACT))
        _canTag = false;
    else
        _canTag = true;
    _startByteOffset = 0;
}

//Adds a native method to the loaded class with the given
//description and access flags
void StaticClassTagger::addNativeMethod(const char *methodName,
                                        const char *description, u2 access_flags)
{
    _bytecodeParser ->addNativeMethod(access_flags, methodName, description);
}

//Returns wheter the current loaded class can be tagged
bool StaticClassTagger::canTagClass()
{
    return _canTag;
}

//Tags the current class with a public static final field
void StaticClassTagger::tagClass()
{
    if(!_canTag)
        return;
    CONSTANT_Long_info classTag;
    classTag.high_bytes = u4((unsigned int)0);
    unsigned int nextTag = _curntTag;
    _curntTag++;
    classTag.low_bytes = u4(nextTag);
    attribute_info longAttr =  _bytecodeParser->addConstantValue_Long(classTag);
    _bytecodeParser->addStaticField(ACC_PRIVATE|ACC_FINAL
                                  , _tagName,
                                  _bytecodeParser->longTypeToBytecode(),
                                  1, &longAttr);
    std::cout << "\tClass tagged: " << _className << " with tag: "<<
    _curntTag - 1 <<std::endl;
}
//Inserts a report of method called to all methods with name 'methodsName'
void StaticClassTagger::insertReportToMethods(const char *methodsName)
{
    if(!_canTag)
        return;
    //Check all methods
    unsigned int nMethods = _bytecodeParser -> methodsSize();
    for(unsigned int i=0; i != nMethods; i++){
        if(_bytecodeParser ->methodHasAccess(i, ACC_ABSTRACT))
            continue;
        if(_bytecodeParser ->methodHasAccess(i, ACC_NATIVE))
            continue;
        if(_bytecodeParser ->methodNameEquals(i, methodsName)){
            _bytecodeParser -> add_getstatic(i, _startByteOffset, _tagName,
                                         _bytecodeParser->longTypeToBytecode(),
                                         NULL);
            _bytecodeParser -> add_invokestatic(i, _startByteOffset + 2,
                                                _reportMethodName,
                                            _reportMethodSignature,
                                            _reportMethodClass);
        }
    }
}

//Returns the current size in bytes of the loaded bytecode
unsigned long StaticClassTagger::bytecodeSize()
{
    return _bytecodeParser ->bytecodeSize();
}

//Fills array of length len with the current loaded bytecode
//len >= this ->bytecodeSize();
void StaticClassTagger::dumpBytecodeToArray(unsigned char *array,
                                            unsigned long len)
{
    _bytecodeParser ->dumpBytecodeToBuffer(array, len);
}

//Writes the .class to a file in disk
void StaticClassTagger::dumpBytecodeToFile(const char *filename)
{
    _bytecodeParser ->dumpBytecode(filename);
}