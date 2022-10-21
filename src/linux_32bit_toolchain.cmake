set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR "i386")

# compilers to use for C and C++
set(CMAKE_C_COMPILER gcc)
set(CMAKE_C_FLAGS -m32)
set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS -m32)
set(CMAKE_SHARED_LINKER_FLAGS -m32)

# here is the target environment location 
set(CMAKE_FIND_ROOT_PATH  /usr/i386-linux-gnu)
set(CMAKE_LIBRARY_PATH "/usr/lib/i386-linux-gnu")
include_directories(BEFORE /usr/include/i386-linux-gnu)

