#include <jni.h>

extern "C"
{
    JNIEXPORT void JNICALL
    Java_java_lang_Object_sayHello(JNIEnv *env, jclass cls, jlong tag);
}

