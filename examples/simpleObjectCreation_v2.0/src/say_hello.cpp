#include <say_hello.hpp>
#include <native_agent.hpp>
#include <iostream>
#include <libkern/OSAtomic.h>
#include <time.h>


JNIEXPORT void JNICALL
Java_java_lang_Object_sayHello(JNIEnv *env, jclass cls, jlong tag)
{
    
    creationLog ->addNewEntry((int64_t)env, (uint32_t)tag);

}
/*
Java_java_lang_Object_sayHello(JNIEnv *env, jclass cls)
{
    //fprintf(stderr, "Hey there\n");
}

*/