INCLUDE(CMakeForceCompiler)

cmake_minimum_required (VERSION 2.8)
SET(CMAKE_SYSTEM_NAME Generic)
 
project(nanoStackLibServic)


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-libservice/)


FILE(GLOB NANO_STACK_LIB_SERVICE_SRC
	"${CMAKE_CURRENT_SOURCE_DIR}/source/nsdynmemLIB/*.c"
	"${CMAKE_CURRENT_SOURCE_DIR}/mbed-client-libservice/*.*"
	
# 	"${CMAKE_CURRENT_SOURCE_DIR}/source/libCoap/src/*.c"
# 	"${CMAKE_CURRENT_SOURCE_DIR}/source/libCoap/src/include/*.h"
	)

CREATE_LIBRARY(nanoStackLibService "${NANO_STACK_LIB_SERVICE_SRC}" "") 


