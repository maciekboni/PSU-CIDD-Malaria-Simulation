find_package(CLI11 CONFIG REQUIRED)
file(GLOB DXG_SRC_FILES *.cpp *.h)

add_executable(DxGGenerator ${DXG_SRC_FILES})
add_dependencies(DxGGenerator  MaSimCore)
target_link_libraries(DxGGenerator PRIVATE MaSimCore CLI11::CLI11)
target_compile_features(DxGGenerator PRIVATE cxx_range_for)