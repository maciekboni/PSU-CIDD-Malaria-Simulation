# 'Google_test' is the subproject name
find_package(fmt CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(GSL REQUIRED)
#project(Google_tests)

include_directories(${POMS_SOURCE_DIR}/src ${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4)


set(SRC_TEST main.cpp test1.cpp Spatial/CoordinateTest.cpp Spatial/LocationTest.cpp Core/StringHelpersTest.cpp
        Core/TimeHelpersTest.cpp RandomTest.cpp Core/ConfigTest.cpp)

add_executable(main_test ${SRC_TEST})
add_dependencies(main_test date args MaSimCore)

#include_directories (${POMS_SOURCE_DIR}/src)
#include_directories(${EXT_PROJECTS_DIR}/easyloggingpp.9.96.4)

target_link_libraries(main_test PRIVATE MaSimCore yaml-cpp fmt::fmt-header-only
        GSL::gsl GSL::gslcblas GTest::gtest GTest::gtest_main)

target_compile_features(main_test PRIVATE cxx_range_for)


add_custom_command(TARGET main_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${POMS_SOURCE_DIR}/test/test_data/input.yml $<TARGET_FILE_DIR:main_test>)



