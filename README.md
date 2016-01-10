# Source2Gen
Source2Gen is an SDK generator for the Source 2 engine. It will generate C++ headers for many exposed classes and enumerators.

Thanks to the new Schema system within Source 2, many classes and enums are fully laid out. The Schema system exposes virtually every attribute about them. Due to this, the generated headers have an amazing amount of detail.

This is a fork of praydog's 32-bit windows version that works on OSX's 64 bit dota2.exe.

# Requirements
CMake.

A not-ancient version of clang.

# Usage
Add the built dylib to DYLD_INSERT_LIBRARIES before running dota2.exe.

# Using the Generated Headers
The headers will work out of the box as long as everything in Source2Gen/shared is included and used in the project that uses the generated headers. This is because a few of the classes have function wrappers for static members. These functions need to use the Schema system to find the static member pointers at runtime.

# Examples
The entirety of what Source2Gen will generate can be found in GeneratedExample.
