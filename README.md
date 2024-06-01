
# C Thread, emulated C11 and thread pool

Using [Pthreads](https://en.wikipedia.org/wiki/Pthreads), or [Pthreads4w](http://sourceforge.net/projects/pthreads4w/).

The **Windows** build in **_deps_** folder, _this fork has ABI differences,_ see original [README.md](https://github.com/GerHobbelt/pthread-win32/blob/master/README.md).

> This branch has some changes to be able to be compiled using [Tiny C compiler](https://github.com/zelang-dev/tinycc).

**CThread** is a minimal, portable implementation of basic threading classes for C. They closely mimic the functionality and naming of the C11 standard, and should be easily replaceable with the corresponding standard variants.

## Thread pool implementation

 * Starts all threads on creation of the thread pool.
 * Stops and joins all worker threads on destroy.
