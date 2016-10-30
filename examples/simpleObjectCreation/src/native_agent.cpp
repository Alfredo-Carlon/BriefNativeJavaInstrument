#include <jvmti.h>
#include <say_hello.hpp>
#include <ClassFile_FirstParser.hpp>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


void JNICALL
nPA_ClassFileLoadHook(jvmtiEnv *,
                      JNIEnv*,
                      jclass,
                      jobject,
                      const char*,
                      jobject,
                      jint,
                      const unsigned char*,
                      jint*,
                      unsigned char**);


unsigned int __inst_Class_tag;


JNIEXPORT jint JNICALL
Agent_OnLoad(JavaVM *vm, char *options, void *reserved)
{
    
    //Get the jvmti environment
    jvmtiEnv *jvmti;
    vm->GetEnv((void **)&jvmti, JVMTI_VERSION_1_0);
    
    
    //Now set the native prefix
    
    //We add the capability
    jvmtiCapabilities* nativeCap = (jvmtiCapabilities *)
    malloc(sizeof(jvmtiCapabilities));
    memset(nativeCap, 0,sizeof(jvmtiCapabilities));
    nativeCap ->can_generate_all_class_hook_events = 1;
    jvmti ->AddCapabilities(nativeCap);
    
    
    jvmtiError retCode = jvmti -> SetEventNotificationMode(JVMTI_ENABLE,
                                                   JVMTI_EVENT_CLASS_FILE_LOAD_HOOK,
                                                   NULL);
    if(retCode == JVMTI_ERROR_NONE){
        printf("ClassFileLoadHook enabled\n");
    }
    
    jvmtiEventCallbacks callbacks;
    memset(&callbacks,0, sizeof(callbacks));
    
    callbacks.ClassFileLoadHook = &nPA_ClassFileLoadHook;
    retCode = jvmti->SetEventCallbacks(&callbacks, (jint)sizeof(callbacks));
    if(retCode == JVMTI_ERROR_NONE){
        printf("ClassFileLoadHook loaded\n");
    }
    __inst_Class_tag = 0;
    return 0;
}

void JNICALL
nPA_ClassFileLoadHook(jvmtiEnv *jvmti_env,
                      JNIEnv* jni_env,
                      jclass class_being_redefined,
                      jobject loader,
                      const char* name,
                      jobject protection_domain,
                      jint class_data_len,
                      const unsigned char* class_data,
                      jint* new_class_data_len,
                      unsigned char** new_class_data)
{
    printf("Class about to load: %s\n",name);
    
    
}


