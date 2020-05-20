# Building FFNx

## Configuration
FFNx needs to be configured with with [cmake]

### CMake Options
Build Options:
   Option  |            Description                                        | Default Value |
:---------:|:-------------------------------------------------------------:|:-------------:|
FORCEHEAP  | Force all allication to our heap                              | OFF
TRACEHEAP  | Trace and keep count of every allocation made by this program | OFF
PROFILING  | Enable Profiling                                              | OFF
SUPERBUILD | Build ALL                                                     | ON
BUILDDOCS  | Build Documentation, [doxygen] is required                    | OFF

Example cmake command.

`cmake -DOPTIONNAME=VALUE CMakeLists.txt`

## Building

After Configuring you Should be able to run make to build all targets.
`make`

[doxygen]:http://www.stack.nl/~dimitri/doxygen/
[cmake]:https://cmake.org/
