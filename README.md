SCP
======

Synchronization Control Protocol

SCP is a synchronization protocol that is designed to synchronize threads/tasks on multi/core processors.
It works for single core processors, with multithreading support, as well. It is available for Linux, VxWorks
and Windows. Linux version is being a mainstream version and most development work in concentrated on it.

SCP is a protocol that contains special synchronization algorithm for synchronization of threads/tasks
in complex situations. SCP is mainly designed to be used on multi-core CPUs, thats where the most performance
can be achieved. SCP can also run on single core CPUs with multithreading. SCP is written in C++, and represents
a class named "Concurrent", which contains all synchronization functionalities. SCP protocol can be used in any C++
environment. Synchronization protocol is capable of synchronizing any number of threads/tasks that access any amount
of shared resources at once. This is a unique feature of the protocol. SCP provides convenient API functionality for
dealing with synchronization without having to know much about synchronization itself. With SCP, synchronization is
seen as a black box, where programmer has to know only what to input; output of this black box is proper
synchronization and no headache from deadlocks, starvation, inefficient CPU usage, semaphores and so forth.
Synchronization with SCP can be applied to many situations where mutual exclusion is required. Protocol manages
mutual exclusion in heavily loaded parallel environment. One of the key features of SCP is that it is a dynamic
algorithm. It provides dynamic management of parallel objects and shared resources. It can adapt to parallel
environment just as programmer wants. During run-time, parallel objects can play with number of resources, i.e. add
some more resources to access, or abandon some resources. They can also add new resources at run-time and delete them.
New parallel objects can also arrive and existing objects can leave execution at any time. All API functionality
ensure mutual exclusion while allowing multiple objects to access multiple resources at the same time. There are
existing synchronization algorithms and protocols that solve concrete synchronization problems. SCP contains a 
general purpose synchronization algorithm that handles many types of synchronization problems, including dining
philosophers problem, readers and writers problem and similar. But the most important thing is that it has been
designed to solve more complex situations with multiple objects and multiple resources.

