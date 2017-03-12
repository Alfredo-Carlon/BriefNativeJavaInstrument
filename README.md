# BriefNativeJavaInstrument

## Native library to perform 'brief' Java bytecode instrumentations.

### Introduction: 

The main goal of this project is to learn (most surely not completely) Java 8 SE
bytecode biased towards 'bytecode instrumentation' biased towards Runtime
Verification.

According to [1] Runtime Verification is 
> the discipline of computer science
that deals with the study, development, and application of those verification
techniques that allow checking whether a _run_ of a system under scrutiny
satisfies or violates a given correctness property. 

The goal is to program a 'minimal' instrumentation library. 'Minimal' in the
following sense: Given a system _S_, a Java Virtual Machine (JVM) _J_, a set
of properties _P_ of _S_ and a set of 'basic actions' _A_ (e.g. function call, object 
creation, variable assignment, basic block execution) required and sufficient
to observe _P_. Instrument the execution of _S_ on _J_ (denoted by _J->S_) 
such that every action a in _A_ can be observed and sent to an analyzing process 
to determine if _P_ are satisfied. 

The difference between _A_ and _P_ is that any property can be divided in two parts:
data and processing. The data part is provided by the implementation of the 
basic actions in _A_, i.e. the instruments in _J->S_. The processing part is done
in a separate process.

An example: Suppose we are interested in the property _p_ = 'Every call to a 
method _m_ of a class _C_ is followed by a call to a method _b_ of a class _D_'. For
sake of the argument, suppose _m_ and _b_ are not native. The data part could be
accomplished by instrumenting the start of _m_ and _b_:

```
m: 
report_execution(m.tag);
.
.
.
end_m
```

```
b:
report_execution(b.tag);
.
.
.
end_b
```
On the processing side
server:

```
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
```

The processing server could be implemented in Java or another language with a
C++ interface, e.g. Python. 

For the moment, this project will focus on the instrumentation part of _J->S_.

The current technical restrictions:

1. OS Mac OS X 10.10, 10.11
2. HotSpot VM (Oracle's official and OpenJDK (jdk 9))

The parser's datatypes are implemented the closest to [2] as possible.


### Compilation:
To compile the project just run make
```bash
$make
```
The library will be in the lib directory.
    
To compile the examples run make at each example's root directory.



### References:

1. Leucker, M. and Schallhart, C. "A Brief Account of Runtime Verification",
    The Journal of Logic and Algebraic Programming, Volume 78, Issue 5, 2009,
    Pages 293-303.

2. Lindholm, Tim, et al. The Java Virtual Machine Specification: 
    Java SE 8 Edition. Pearson Education, 2014.
