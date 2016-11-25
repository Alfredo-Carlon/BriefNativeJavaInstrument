
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

#include <ClassFile_FirstParser.hpp>
#include <vector>
#include <string>
#include <unordered_map>


class StaticClassTagger {
private:
    
    ClassFile_FP    *_bytecodeParser;
    unsigned int    _curntTag;
    bool            _classTagged;
    bool            _canTag;
    const char      *_tagName;
    const char      *_reportMethodName;
    const char      *_reportMethodSignature;
    const char      *_reportMethodClass;
    const char      *_className;
    unsigned short  _startByteOffset;
    
    std::vector<std::string> *_classNames;
    std::unordered_map<unsigned long, unsigned int> *_threadsInfo;
    
public:
    
    //Creates a ClassTagger
    StaticClassTagger();
    
    //Creates a ClassTagger with an initial tag value
    StaticClassTagger(int32_t startTag);
    
    //Load the given class into the 'working memory'
    void loadClass(const unsigned char *originalBytecode,
                   unsigned int len,
                   const char *className);
    //Returns wheter the current loaded class can be tagged
    bool canTagClass();
    
    //Adds a native method to the loaded class with the given
    //description and access flags
    void addNativeMethod(const char *methodName, const char *signature,
                         u2 access_flags);
                         
    
    //Tags the current class with a public static final field
    void tagClass();
    
    //Inserts a report of method called to all methods with name 'methodsName'
    void insertReportToMethods(const char *methodsName);
    
    //Returns the current size in bytes of the loaded bytecode
    unsigned long bytecodeSize();
    
    //Fills array of length len with the current loaded bytecode
    //len >= this ->bytecodeSize();
    void dumpBytecodeToArray(unsigned char *array, unsigned long len);
    
    //Writes the .class to a file in disk
    void dumpBytecodeToFile(const char *filename);
    
    
    //Returns the name of the class with tag
    std::string classNameForTag(unsigned int tag);
    
    //Adds a new allocation to a given thread
    void threadAllocatedNewObject(unsigned long threadId);
    
    //Prints threads info
    void printThreadsInfo();
    
};





