# SimpleObjectCreation_v1.0

This example is a kind of Hello World example of bytecode instrumentation.
It reports every call to a constructor of a loaded class.

### Compile/Run:
To Compile the example just run
```bash
$make
```
inside the example's root directory (simpleObjectCreation_v1.0). Not that
libBNI.dylib must be already constructed.
	
To Run the example:
```bash
$cd bin
$java -agentlib:NativeAgent Test
```
### Design:

1. Every object O of class C must call any of C's constructors.
2. Every constructor of C has the special compiler given name ```<init>```.
3. Every constructor c, calls the constructor of the class Object.
  To report a constructor (c) call we do:
  ```
  c:
     dir_0: //Report first inst
      .
      .
      .
      dir_i: //Report last byte
      dir_i+1: c's first instruction
  ```
  Where dir_j is a byte index. dir_0 = 0.

4. To insert the reporting code, we intersect when C is loaded into the 
   Java Virtual Machine (JVM) with a native agent using ClassFileLoadHook.
5. Before the JVM is fully initialized, the Java Native Interface (JNI) is 
   initialized. Hence JNI's RegisterNatives is up before the JVM is fully
   initialized.
6. By (5) we can enrich the class Object with a native method (say_hello) 
   that would do the reporting. Hence we can report events 'as soon' as JNI
   is up and ready without worrying about cyclic events, class dependencies, etc.
7. For the reports to be 'unique' we can tag each class with a final and
   static tag and feed that tag to the reporting method.

### Implementation:
	
1. The native agent is implemented in the file native_agent.cpp it performs
   the following actions:
  1. It adds the capabilities: JVMTI_EVENT_CLASS_FILE_LOAD_HOOK, to
     intersect the loading of every class; JVMTI_EVENT_VM_START, to
     use JNI's RegisterNatives to register the implementation of
     the reporting method; JVMTI_EVENT_VM_DEATH, to make a last
     call to the reporting method.
  2. For each loaded class C:
    1. It tags C with a long type tag. 
    2. For each of C's constructors it adds:
      ```
      aload_0
      getstatic ...
      invokestatic ...
      ```
    3. If C is Object then it also adds say_hello.
    
  3. At VM's death it calls say_hello to report the number of
     constructors called and objects created.

2. say_hello.cpp implements the reporting method say_hello. For each call
   it checks the tag of the caller. If it is 0 then it was done by Object's
   constructor, hence we have a new object. Each time say_hello is called
   it 'reports' by writing to stderr.

3. StaticClassTagger.cpp takes care of the tagging and bytecode insertion
	
	

	
