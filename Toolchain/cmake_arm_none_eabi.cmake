message(STATUS ("Processing the arm-none-eabi tool chain file")

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)

set(CMAKE_ASM_COMPILER arm-none-eabi-gcc)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)


set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


# ---------------------------- Error -----------------------------------------
# nosys.specs: attempt to rename spec 'link_gcc_c_sequence' to already
# defined spec 'nosys_link_gcc_c_sequence'
# set(CMAKE_C_FLAGS "-specs=nosys.specs"   CACHE INTERNAL "")
# Caused by adding it twice in the linker flags or adding it to the compile flags
# ----------------------------------------------------------------------------

set(CMAKE_EXE_LINKER_FLAGS "-specs=nosys.specs"   CACHE INTERNAL "")

 unset(CMAKE_EXECUTABLE_SUFFIX)
set(CMAKE_EXECUTABLE_SUFFIX ".out" CACHE INTERNAL FORCE "")

set(CMAKE_ASM_COMPILER_ID GNU)
set(CMAKE_C_COMPILER_ID GNU)
set(CMAKE_CXX_COMPILER_ID GNU)

set(CMAKE_ASM_COMPILER_FORCED TRUE)
set(CMAKE_C_COMPILER_FORCED TRUE)
set(CMAKE_CXX_COMPILER_FORCED TRUE)
