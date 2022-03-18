find_package(GTest CONFIG REQUIRED)

include(GoogleTest)

include_directories(${POMS_SOURCE_DIR}/src ${EXT_PROJECTS_DIR}/easyloggingpp-9.97.0)


add_custom_target(build_and_test
  COMMAND env GTEST_COLOR=1 ${CMAKE_CTEST_COMMAND} -V)

macro(package_add_test TESTNAME)
  # create an exectuable in which the tests will be stored
  add_executable(${TESTNAME} ${ARGN})

  # link the Google test infrastructure, mocking library, and a default main fuction to
  # the test executable.  Remove g_test_main if writing your own main function.
  target_link_libraries(${TESTNAME} MaSimCore GTest::gtest GTest::gmock)

  # gtest_discover_tests replaces gtest_add_tests,
  # see https://cmake.org/cmake/help/v3.10/module/GoogleTest.html for more options to pass to it
  gtest_discover_tests(${TESTNAME}
    # set a working directory to your project root so that you can find test data via paths relative to the project root
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/test/test_data
    PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    )
  add_dependencies(build_and_test MaSimCore ${TESTNAME})
  set_target_properties(${TESTNAME} PROPERTIES FOLDER test)
endmacro()

set(TEST_SRC_FILES
  MockRandom.h
  main.cpp test1.cpp
  RandomTest.cpp
  Parasites/GenotypeTest.cpp
  Parasites/GenotypeMatchPatternTest.cpp
  Parasites/GenotypeOverrideEC50Test.cpp
  Parasites/GenotypePerformMutationTest.cpp
  Parasites/GenotypeFreeRecombinationTest.cpp
  Spatial/CoordinateTest.cpp
  Spatial/LocationTest.cpp
  Core/StringHelpersTest.cpp
  Core/TimeHelpersTest.cpp
  Core/ConfigTest.cpp
  Core/Config/ReadGeneInfoTest.cpp
  Core/Config/ReadInitialParasiteInfoTest.cpp
  Core/Config/ReadDrugDBTest.cpp
  Core/Config/ReadOverrideEC50.cpp
  Core/Config/ReadMosquitoConfigTest.cpp
  Core/Config/UpdateMutationMaskTest.cpp
  Mosquito/MosquitoTest.cpp
  )

set(MAIN_TEST masim_test)

package_add_test(${MAIN_TEST} ${TEST_SRC_FILES})

add_custom_command(TARGET ${MAIN_TEST} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  ${POMS_SOURCE_DIR}/input/input.yml $<TARGET_FILE_DIR:${MAIN_TEST}>)
