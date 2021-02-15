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
  set(CMAKE_CXX_FLAGS_RELEASE "-DNOMINMAX ${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "-DNOMINMAX ${CMAKE_CXX_FLAGS_DEBUG} /MTd")

  # Sets the flag to disable Linux specific error handling 
  add_definitions(-D__DISABLE_CRIT_ERR=1)
endif ()

IF(APPLE)
  # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
  LINK_DIRECTORIES(/usr/local/lib)
  INCLUDE_DIRECTORIES(/usr/local/include)
ENDIF()


#if(UNIX)
#    set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#endif()

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
#add_subdirectory(${EXT_PROJECTS_DIR}/fmt)

include_directories(${YAML_CPP_INCLUDE_DIR} ${DATE_INCLUDE_DIR} ${ARGS_INCLUDE_DIR} ${EXT_PROJECTS_DIR}/CLI11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

#Setup test project
option(BUILD_TESTS "Build all tests." OFF) # Makes boolean 'test' available.
if (BUILD_TESTS)
#  find_package(Catch2 CONFIG REQUIRED)
  add_subdirectory(${EXT_PROJECTS_DIR}/catch)

  set(PROJECT_TEST_NAME masim_test)
  add_subdirectory(test)
endif ()
#enable_testing()
#add_test(test1 ${CMAKE_BINARY_DIR}/bin/${PROJECT_TEST_NAME} --gtest_color=yes)


#add_custom_target(run
#    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/bin/hello_2
#    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/bin/hello_2
#    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#)

#cmake_minimum_required(VERSION 3.14)
#
#project(Google_tests)
#Test setup
#set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp)
add_executable(src_run ${SOURCE_FILES})

#include_directories(src)
#add_subdirectory(src)

#target_link_libraries(src_run src_lib)



# Google_tests

#cmake_minimum_required(VERSION 2.8.2)

#project(DP_Example)

include(CTest)

if (CMAKE_VERSION VERSION_LESS 3.2)
  set(UPDATE_DISCONNECTED_IF_AVAILABLE "")
else()
  set(UPDATE_DISCONNECTED_IF_AVAILABLE "UPDATE_DISCONNECTED 1")
endif()

include(DownloadProject.cmake)
download_project(PROJ                googletest
        GIT_REPOSITORY      https://github.com/google/googletest.git
        GIT_TAG             master
        ${UPDATE_DISCONNECTED_IF_AVAILABLE}
        )

# Prevent GoogleTest from overriding our compiler/linker options
# when building with Visual Studio
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})

# When using CMake 2.8.11 or later, header path dependencies
# are automatically added to the gtest and gmock targets.
# For earlier CMake versions, we have to explicitly add the
# required directories to the header search path ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include"
          "${gmock_SOURCE_DIR}/include")
endif()

# Trivial example using gtest and gmock
#add_executable(example example.cpp)
add_subdirectory(Google_tests)
#add_executable(Google_Tests_run Converter_tests.cpp AbsoluteDateTest.cpp)

target_link_libraries(Google_Tests_run gtest gmock_main)
add_test(NAME example_test COMMAND Google_Tests_run)
