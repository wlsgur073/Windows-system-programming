# Be a Better Software Developer

you can search all WinAPI functions on the [vergiliusproject](https://www.vergiliusproject.com/)

### We need to know about OS system programming
- Understanding how the OS operates helps in becoming **a better software developer**.
- It improves debugging skills by allowing deeper insights into system-level behavior.
- It provides knowledge of how resources such as memory, CPU, and I/O are managed.
- It enables the development of optimized and efficient software by understanding system constraints.
- It lays the foundation for working on advanced fields like embedded systems, real-time computing, and distributed systems.

### What I learned
* [x] How to communicate between user mode and kernel mode.
* [x] Create and control threads.
    * [x] What is difference between `CreateThread()` and `_beginthreadex()` in WinAPI32.
    * [x] What is Thread swiching.
    * [x] `Sleep()` function
* [x] Sync Threads
    * [x] Critical section
    * [x] Synchronize event
    * [x] Semaphore
    * [x] simple control affinity
* [x] Win32 file I/O
    * [x] Read/WirteFile() vs Read/WirteFileEx()
    * [x] Asynchronous file input/output with event
        * [x] OVERLAPPED, offset, pending
    * [x] Asynchronous file input/output with callback
    * [x] `MapViewOfFile()` - `memcpy()`
* [x] Memory system
    * [x] Virtual Memory Basic Operating Principles
    * [x] Heap memory utilization
* [x] Create and control processes
    * [x] IPC - multi process - Mutex
* [x] DLL
    * [x] Static library vs Dynamic library
    * [x] Cautions for creating DLLs
