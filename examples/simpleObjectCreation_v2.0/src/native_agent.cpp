#include <jvmti.h>
#include <say_hello.hpp>
#include <native_agent.hpp>
#include <StaticClassTagger.hpp>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libkern/OSAtomic.h>

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


ObjCreationHistory *creationLog;
volatile int32_t tagCounter;

/*********** Native method definition for reporting ************/

const char *_reportClass = "java/lang/Object";
const char *_reportMethod = "sayHello";
const char *_reportSignature = "(J)V";



/*********** Native method definition for reporting ************/
unsigned int anonClasses;
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
    anonClasses = 0;
    tagCounter = -1;
    creationLog = new ObjCreationHistory();
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
        if(!name){
            //The name of the class is null so its anonymous
            name = "null";
            anonClasses++;
        }else{
            //printf("%s\n", name);
        }
        
        //Increase the tagCounter
        int32_t curntTag = OSAtomicIncrement32(&tagCounter);
        StaticClassTagger *tagger = new StaticClassTagger(curntTag);
        
        
        tagger ->loadClass(class_data, class_data_len,name);
        
        
        if(!tagger ->canTagClass())
            return;
        
        if(!strcmp(name, _reportClass)){
            tagger ->addNativeMethod(_reportMethod, _reportSignature, ACC_PUBLIC|ACC_STATIC);
        }
        tagger ->tagClass();
        tagger ->insertReportToMethods("<init>");
        
        creationLog ->addNewLoadingEntry((int64_t)jni_env, curntTag, name);
        
        //Now allocate the space and feed the modified bytecode
        *new_class_data_len = tagger ->bytecodeSize();
        jvmti_env -> Allocate(
                 *new_class_data_len,
                 new_class_data);
        
        tagger ->dumpBytecodeToArray(*new_class_data, *new_class_data_len);
        /*if(!strcmp(name, "sun/reflect/GeneratedMethodAccessor1")){
            tagger ->dumpBytecodeToFile("sunReflectGMA.class");
        }
        fptr = fopen("MlastLoaded.class", "w");
        fwrite(*new_class_data, sizeof(unsigned char), *new_class_data_len, fptr);
        fclose(fptr);
        */
        
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
    sayHelloRegister.name       = (char *)_reportMethod;
    sayHelloRegister.signature  = (char *)_reportSignature;
    sayHelloRegister.fnPtr      = (void *)&Java_java_lang_Object_sayHello;
    
    
    //Get the jclass for Object
    jclass ObjectClass = jni_env ->FindClass(_reportClass);
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
    creationLog ->writeLogToFile("CreationHistory.txt");
    printf("Anon classes: %d\n", anonClasses);
}