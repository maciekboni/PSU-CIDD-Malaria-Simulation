# 'Google_test' is the subproject name
find_package(fmt CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
#project(Google_tests)

set(SRC ModelDataCollector.cpp ModelDataCollector.h )

set(SRC_TEST test1.cpp test2.cpp)
add_executable(main_test ${SRC_TEST})
include_directories (${POMS_SOURCE_DIR}/src)

# 'lib' is the folder with Google Test sources
#add_subdirectory(lib)
#include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

# 'Google_Tests_run' is the target name
# 'test1.cpp tests2.cpp' are source files with tests
#add_executable(Google_Tests_run test1.cpp tests2.cpp)
#target_link_libraries(Google_Tests_run gtest gtest_main)
#target_link_libraries(Google_Tests_run PSU-CIDD-Malaria-Simulation)

target_link_libraries(main_test PRIVATE fmt::fmt-header-only GTest::gtest GTest::gtest_main)

