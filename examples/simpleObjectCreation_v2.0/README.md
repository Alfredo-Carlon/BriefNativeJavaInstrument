#simpleObjectCreation_v3.0

This example is a kind of Hello World example of bytecode instrumentation.
It reports every call to a constructor of a loaded class.

Compile/Run:
	To Compile the example just run
	   $make
	inside the example's root directory (simpleObjectCreation_v2.0). Not that
	libBNI.dylib must be already constructed.
	
	To Run the example:
	   $cd bin
	   $java -agentlib:NativeAgent Test

Notes:
	A thread safe class that keeps track of loaded classes and created objects
	is implemented. 
	
	To know which thread allocated the object, we keep track of the JNIEnv
	pointer sent to say_hello. This may not be accurate because two threads
	may share the same JNIEnv address if one executes after the other's deallocation.
	
	

	
