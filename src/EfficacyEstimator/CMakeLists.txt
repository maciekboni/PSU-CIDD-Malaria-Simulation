find_package(CLI11 CONFIG REQUIRED)
set(EfficacyEstimator_SRC_FILES main.cpp PkPdReporter.cpp PkPdReporter.h)

add_executable(EfficacyEstimator ${EfficacyEstimator_SRC_FILES})
add_dependencies(EfficacyEstimator MaSimCore)
target_link_libraries(EfficacyEstimator PRIVATE MaSimCore CLI11::CLI11)
target_compile_features(EfficacyEstimator PRIVATE cxx_range_for)
