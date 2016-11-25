#include <say_hello.hpp>
#include <cstdio>



JNIEXPORT void JNICALL
Java_java_lang_Object_sayHello(JNIEnv *env, jobject obj,jlong tag)
{
    static unsigned int objectsCreated = 0;
    static unsigned int constructorsCalled = 0;
    
    if(tag == (unsigned long)-1){
        fprintf(stderr, "Number of constructors called: %d\n", constructorsCalled);
        fprintf(stderr, "Number of objects created: %d\n", objectsCreated);
        return;
    }
    
    if(tag == 0){
        objectsCreated++;
    }else{
        fprintf(stderr,"\t");
    }
    fprintf(stderr,"Hello World from constructor for class with tag: %ld\n",tag);
    constructorsCalled++;
    
}
