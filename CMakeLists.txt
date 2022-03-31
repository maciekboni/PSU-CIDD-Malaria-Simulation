cmake_minimum_required(VERSION 3.12)

set(CMAKE_TOOLCHAIN_FILE
  ${CMAKE_CURRENT_SOURCE_DIR}/ext/vcpkg/scripts/buildsystems/vcpkg.cmake CACHE
  STRING "Vcpkg toolchain file")

project(POMS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

if (MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNOMINMAX -D__DISABLE_CRIT_ERR=1")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DNOMINMAX -D__DISABLE_CRIT_ERR=1")
endif ()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DELPP_NO_DEFAULT_LOG_FILE")

# the next 2 flags is used for profiler with perf
#set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")

IF (APPLE)
  # Fix linking on 10.14+. See https://stackoverflow.com/questions/54068035
  LINK_DIRECTORIES(/usr/local/lib)
  INCLUDE_DIRECTORIES(/usr/local/include)
ENDIF ()

set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/ext)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

#Setup test project
option(BUILD_TESTS "Build all tests." ON) # Makes boolean 'test' available.
if (BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif ()

#enable_testing()
#add_test(test1 ${CMAKE_BINARY_DIR}/bin/${PROJECT_TEST_NAME} --gtest_color=yes)

