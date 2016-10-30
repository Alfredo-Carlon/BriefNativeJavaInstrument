#include <say_hello.hpp>
#include <cstdio>

JNIEXPORT void JNICALL
Java_java_lang_Object_sayHello(JNIEnv *env, jobject obj,jlong tag)
{
    fprintf(stderr,"Hello from constructor for class with tag: %ld",tag);
}

