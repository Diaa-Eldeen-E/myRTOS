set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/Toolchain")

# include(cmake_arm_none_eabi)

set(ProjName UART)

project(${ProjName})


# ------------------------ Building ---------------------------------------

# Soruce variables
set(SOURCES 
	src/handy.c
	src/UART.c
	)
	
set(CPU_OPTIONS "-mthumb -mcpu=cortex-m4 -march=armv7e-m")
set(FPU_OPTIONS "-mfloat-abi=soft -mfpu=fpv4-sp-d16")
set(PART_NAME	"-DPART_TM4C1294NCPDT")	# -D defines a macro to be used by preprocessor

set(CMAKE_C_FLAGS 
	"${CPU_OPTIONS} ${FPU_OPTIONS} ${PART_NAME} \
	-Wall -g -MD -std=c99")


# Executables and links
add_library(${ProjName} "${SOURCES}")

target_include_directories(${ProjName} PUBLIC
			${PROJECT_SOURCE_DIR}/include
			${PROJECT_SOURCE_DIR}/include/CMSIS
			 )
