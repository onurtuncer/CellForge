cmake_minimum_required(VERSION 3.18)
project(CellForgeTests LANGUAGES CXX)

enable_testing()

find_package(Catch2 3 REQUIRED)
find_package(tesseract REQUIRED COMPONENTS common scene_graph)

include(CTest)
include(Catch)

add_executable(test_tesseract_smoke
    integration/test_tesseract_smoke.cpp
)
target_link_libraries(test_tesseract_smoke
    PRIVATE
        Catch2::Catch2WithMain
        tesseract::common
        tesseract::scene_graph
)
target_compile_features(test_tesseract_smoke PRIVATE cxx_std_17)

catch_discover_tests(test_tesseract_smoke)
