find_package(Catch2 CONFIG REQUIRED)
include_directories(${PROJECT_SOURCE_DIR}/src ${EXT_PROJECTS_DIR}/easyloggingpp-9.97.0)

set(PROJECT_TEST_NAME masim_test)

#add_custom_target(build_and_test
#        COMMAND env GTEST_COLOR=1 ${CMAKE_CTEST_COMMAND} -V)
add_custom_target(build_and_test
        COMMAND masim_test
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/misc
        )

macro(package_add_test TESTNAME)
    # create an exectuable in which the tests will be stored
    add_executable(${TESTNAME} ${ARGN})

    # link the Google test infrastructure, mocking library, and a default main fuction to
    # the test executable.  Remove g_test_main if writing your own main function.
    # target_link_libraries(${TESTNAME} GTest::gtest GTest::gmock GTest::gtest_main)

    target_link_libraries(${TESTNAME} MaSimCore Catch2::Catch2)
    # gtest_discover_tests replaces gtest_add_tests,
    # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it

#    gtest_discover_tests(${TESTNAME}
#            # set a working directory to your project root so that you can find test data via paths relative to the project root
#            WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/misc
#            PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
#            )
    add_dependencies(build_and_test MaSimCore ${TESTNAME})
    set_target_properties(${TESTNAME} PROPERTIES FOLDER test)
endmacro()


set(TEST_SRC_FILES
        main.cpp
        sample_catch_test.cpp
        sample_yaml_cpp_test.cpp
        #SimpleFakeItTest.cpp
        Spatial/CoordinateTest.cpp
        Spatial/LocationTest.cpp
        Core/RandomTest.cpp
        Core/TimeHelpersTest.cpp
        Core/StringHelpersTest.cpp
        Core/Config/ConfigTest.cpp
        )

package_add_test(${PROJECT_TEST_NAME} ${TEST_SRC_FILES})

#target_compile_features(${PROJECT_TEST_NAME} PRIVATE cxx_range_for)
