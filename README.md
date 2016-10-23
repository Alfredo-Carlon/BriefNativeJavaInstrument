# BriefNativeJavaInstrument
Native library to perform 'brief' Java bytecode instrumentations.

The main goal of this project is to learn (most surely not completely) Java 8 SE
bytecode biased towards 'bytecode instrumentation' biased towards Runtime
Verification.

According to [1] Runtime Verification is "the discipline of computer science
that deals with the study, development, and application of those verification
techniques that allow checking whether a \emph{run} of a system under scrutiny
satisfies or violates a given correctness property", 

The goal is to program a 'minimal' instrumentation library. 'Minimal' in the
following sense: Given a system S, a Java Virtual Machine (JVM) J, a set
of properties P of S and a set of 'basic actions' A (e.g. function call, object 
creation, variable assignment, basic block execution) required and sufficient
to observe P. Instrument the execution of S on J (denoted by J->S) 
such that every action a in A can be observed and sent to an analyzing process 
to determine if P are satisfied. 

The difference between A and P is that any property can be divided in two parts:
data and processing. The data part is provided by the implementation of the 
basic actions in A, i.e. the instruments in J->S. The processing part is done
in a separate process.

An example: Suppose we are interested in the property p = 'Every call to a 
method m of a class C is followed by a call to a method b of a class D'. For
sake of the argument, suppose m and b are not native. The data part could be
accomplished by instrumenting the start of m and b: 

m: 
report_execution(m.tag);
.
.
.
b:
report_execution(b.tag);
.
.
.

On the processing side

server:
    prevMsg = nil;
    while(1){
        msg = get_next_msg();
        if(msg.type == 'report_execution'){
            if(msg.tag == b.tag){
                if(prevMsg != nil && prevMesg.tag == m.tag); //Property holds
                else
                    report_error(); //Property does not hold
            }
        }
        prevMsg = msg;
    }

The processing server could be implemented in Java or another language with a
C++ interface, e.g. Python. 

For the moment, this project will focus on the instrumentation part of J->S.

The current technical restrictions:

1. OS Mac OS X 10.10, 10.11
2. HotSpot VM (Oracle's official and OpenJDK (jdk 9))

The parser's datatypes are implemented the closest to [2] as possible.


[1] Leucker, M. and Schallhart, C. "A Brief Account of Runtime Verification",
The Journal of Logic and Algebraic Programming, Volume 78, Issue 5, 2009,
Pages 293-303.

[2] Lindholm, Tim, et al. The Java Virtual Machine Specification: 
Java SE 8 Edition. Pearson Education, 2014.
