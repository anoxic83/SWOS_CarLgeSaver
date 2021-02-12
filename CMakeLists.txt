cmake_minimum_required(VERSION 3.1)

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR})

project(SWOS_ASI_TEMP)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/plugins)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)


if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  MESSAGE(FATAL_ERROR "64 compiling not allowed!")
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

set(CCSRC ${PROJECT_SOURCE_DIR}/src)
set(CCINC ${PROJECT_SOURCE_DIR}/include)


set(CCDEPS ${PROJECT_SOURCE_DIR}/deps)
set(CCASISWOS ${CCDEPS}/swosasi)


set(CMAKE_CXX_FLAGS "-Wall -Wextra -masm=intel")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG -DDLLBUILD")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -s -DNDEBUG -DDLLBUILD")
set(CMAKE_CXX_FLAGS_MINSIZEREL "-O3 -s -DNDEBUG -DDLLBUILD")
set(CMAKE_CXx_FLAGS_RELWITHDEBINFO "-O2 -g -DNDEBUG -DDLLBUILD")

include_directories(${CCSRC})
include_directories(${CCINC})

include_directories(${CCASISWOS})


set(CMAKE_SHARED_LIBRARY_PREFIX "")


#set(CCASI_TEMPFILES
#  ${CCSRC}/main.cc
#  )
#
#add_library(carlgesav SHARED ${CCASI_TEMPFILES})
#set_target_properties(carlgesav PROPERTIES SUFFIX ".asi")
#target_link_options(carlgesav PRIVATE -static)
#

set(CCASI_LGESAVFILES
  ${CCSRC}/savcar.c
)

add_library(carsav SHARED ${CCASI_LGESAVFILES})
set_target_properties(carsav PROPERTIES SUFFIX ".asi")
target_link_options(carsav PRIVATE -static)

