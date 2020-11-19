# Java Profiling and Monitoring tools

There are many Java development tools available for analyzing Java application
run time performance. Eclipse has a set of plug-ins called TPTP which provides
Java application profiling/monitoring framework. However, TPTP is very slow and
I experienced frequent crash while profiling ICU4J codes. For ICU4J development,
I recommend several tools described below.

#### VisualVM

VisualVM is coming with Sun JDK 6 since update 7. You can download the latest
version from here - <https://visualvm.dev.java.net/ >
There is an Eclipse plug-in, which allow you to launch VisualVM when you run a
Java app on Eclipse. You can monitor CPU usage of the Java app, Memory usage
(heap/permgen), classes loaded, etc in GUI. You can also get basic profiling
information, such as CPU usage by class, memory allocations and generate heap
dump, force GC etc.

#### NetBeans profiler

See the page [Profiling ICU4J code with NetBeans profiler](../netbeans/index.md)

NetBeans profiler (formerly known as JFluid) is a great tool for analyzing Java
application performance. In the past, when I need to profile ICU4J classes, I
created a new NetBeans project and use the profiler to analyze issues. The
profiler included in VisualVM is actually a lightweight version of this. I was
wondering if we can use the profiler without creating a NetBeans Java project.
It looks there are many people wanting to use this profiler from Eclipse IDE. It
looks the easiest way to use NetBeans profiler as a standalone app is -
1. Start NetBeans IDE
2. Create a run configuration for a Java application on Eclipse IDE
3. Specify JVM argument to attach NetBeans profiler to the Java process
For example, I use NetBeans 6.7.1. The following JVM argument is specified in
the run configuration in my Eclipse workspace.
java -agentpath:"C:\\Program Files\\NetBeans
6.7.1\\profiler3\\lib\\deployed\\jdk16\\windows\\profilerinterface.dll"="C:\\Program
Files\\NetBeans 6.7.1\\profiler3\\lib,5140"
4. Run the Java Application on Eclipse IDE - You will see "Profiler Agent:
Waiting for connection on port 5140 (Protocol version: 9)" in the console.
5. Go back to NetBeans IDE, then select \[Profile\] - \[Attach Profiler\] from
the menu. You can select one from Monitor / CPU / Memory.
