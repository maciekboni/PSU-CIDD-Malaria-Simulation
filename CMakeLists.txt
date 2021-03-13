cmake_minimum_required(VERSION 3.2)
set(CMAKE_TOOLCHAIN_FILE
        ${CMAKE_CURRENT_SOURCE_DIR}/ext/vcpkg/scripts/buildsystems/vcpkg.cmake  CACHE
        STRING "Vcpkg toolchain file")

project(POMS)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)

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
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(src)

#Setup test project
option(BUILD_TESTS "Build all tests." ON) # Makes boolean 'test' available.
if (BUILD_TESTS)
    add_subdirectory(test)
endif ()

#enable_testing()
#add_test(test1 ${CMAKE_BINARY_DIR}/bin/${PROJECT_TEST_NAME} --gtest_color=yes)