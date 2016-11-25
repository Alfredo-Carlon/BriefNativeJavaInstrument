#include <jvmti.h>
#include <say_hello.hpp>
#include <StaticClassTagger.hpp>
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
void JNICALL
nPA_VMStart(jvmtiEnv *,
            JNIEnv* );

void JNICALL
nPA_VMDeath(jvmtiEnv *jvmti_env,
            JNIEnv* jni_env);


StaticClassTagger *tagger;

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
    
    retCode = jvmti -> SetEventNotificationMode(JVMTI_ENABLE,
                                                JVMTI_EVENT_VM_START,
                                                NULL);
    if(retCode == JVMTI_ERROR_NONE){
        printf("VMStart enabled\n");
    }
    
    retCode = jvmti -> SetEventNotificationMode(JVMTI_ENABLE,
                                                JVMTI_EVENT_VM_DEATH,
                                                NULL);
    if(retCode == JVMTI_ERROR_NONE){
        printf("VMDeath enabled\n");
    }
    
    
    jvmtiEventCallbacks callbacks;
    memset(&callbacks,0, sizeof(callbacks));
    
    callbacks.ClassFileLoadHook = &nPA_ClassFileLoadHook;
    callbacks.VMStart = &nPA_VMStart;
    callbacks.VMDeath = &nPA_VMDeath;
    retCode = jvmti->SetEventCallbacks(&callbacks, (jint)sizeof(callbacks));
    if(retCode == JVMTI_ERROR_NONE){
        printf("ClassFileLoadHook loaded\n");
    }
    tagger = new StaticClassTagger();
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
    try {
        //Write as last loaded
        FILE *fptr = fopen("lastLoaded.class", "w");
        fwrite(class_data, sizeof(unsigned char), class_data_len, fptr);
        fclose(fptr);
        
        tagger ->loadClass(class_data, class_data_len,name);
        if(!tagger ->canTagClass())
            return;
        tagger ->tagClass();
        tagger ->insertReportToMethods("<init>");
        if(!strcmp(name, "java/lang/Object")){
            tagger ->addNativeMethod("sayHello", "(J)V", ACC_PUBLIC|ACC_STATIC);
            tagger ->dumpBytecodeToFile("Object.class");
        }
        //Now allocate the space and feed the modified bytecode
        *new_class_data_len = tagger ->bytecodeSize();
        jvmti_env -> Allocate(
                 *new_class_data_len,
                 new_class_data);
        tagger ->dumpBytecodeToArray(*new_class_data, *new_class_data_len);
        
        
        fptr = fopen("MlastLoaded.class", "w");
        fwrite(*new_class_data, sizeof(unsigned char), *new_class_data_len, fptr);
        fclose(fptr);
        
    } catch (InvalidMethodIndex e) {
        //Dump the data of the class to disk.
        //Change / for _
        char *tmpFilename = (char *)malloc(strlen(name)+1);
        strcpy(tmpFilename, name);
        for(unsigned int i=0; i != strlen(name); i++){
            if(tmpFilename[i] == '/')
                tmpFilename[i] = '_';
        }
        FILE *outBytecode = fopen(tmpFilename, "w");
        fwrite(class_data, 1, class_data_len, outBytecode);
        free(tmpFilename);
    }
    
}

void JNICALL
nPA_VMStart(jvmtiEnv *jvmti_env,
        JNIEnv* jni_env)
{
    printf("Registering natives\n");
    //Register the native method for Object Class
    JNINativeMethod sayHelloRegister;
    sayHelloRegister.name       = (char *)"sayHello";
    sayHelloRegister.signature  = (char *)"(J)V";
    sayHelloRegister.fnPtr      = (void *)&Java_java_lang_Object_sayHello;
    
    
    //Get the jclass for Object
    jclass ObjectClass = jni_env ->FindClass("java/lang/Object");
    jint ret = jni_env ->RegisterNatives(ObjectClass,&sayHelloRegister,1);
    if(!ret){
        printf("\n\n\n\n#######################\n\n\n\n");
    }else{
        printf("Error code: %d\n", ret);
    }
    
}

void JNICALL
nPA_VMDeath(jvmtiEnv *jvmti_env,
        JNIEnv* jni_env)
{
    Java_java_lang_Object_sayHello(jni_env, NULL, -1);
}