cmake_minimum_required(VERSION 3.2)
project(POMS)
#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules")

option(USING_ZAPCC "Use ZAPCC compiler." OFF)
if (USING_ZAPCC)
  SET(CMAKE_C_COMPILER /home/ubuntu/zapcc-20170802-175507/bin/zapcc)
  SET(CMAKE_CXX_COMPILER /home/ubuntu/zapcc-20170802-175507/bin/zapcc++)
endif ()

# for Windows only
if (WIN32)
#  set(CMAKE_CXX_FLAGS_RELEASE "-DNOMINMAX ${CMAKE_CXX_FLAGS_RELEASE} /MT")
#  set(CMAKE_CXX_FLAGS_DEBUG "-DNOMINMAX ${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  set(CMAKE_CXX_FLAGS_RELEASE "-DNOMINMAX")
  set(CMAKE_CXX_FLAGS_DEBUG "-DNOMINMAX")

  # Sets the flag to disable Linux specific error handling 
  add_definitions(-D__DISABLE_CRIT_ERR=1)
endif ()

IF(APPLE)
  # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
  LINK_DIRECTORIES(/usr/local/lib)
  INCLUDE_DIRECTORIES(/usr/local/include)
ENDIF()

set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/ext)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

#include dependent libs
find_package(GSL REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(fmt CONFIG REQUIRED)

message(${YAML_CPP_LIBRARIES})

add_subdirectory(${EXT_PROJECTS_DIR}/date)
add_subdirectory(${EXT_PROJECTS_DIR}/args)

include_directories(${YAML_CPP_INCLUDE_DIR} ${DATE_INCLUDE_DIR} ${ARGS_INCLUDE_DIR} ${EXT_PROJECTS_DIR}/CLI11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

#Setup test project
option(BUILD_TESTS "Build all tests." OFF) # Makes boolean 'test' available.
if (BUILD_TESTS)
  set(PROJECT_TEST_NAME masim_test)
  add_subdirectory(test)
endif ()
#enable_testing()
#add_test(test1 ${CMAKE_BINARY_DIR}/bin/${PROJECT_TEST_NAME} --gtest_color=yes)

