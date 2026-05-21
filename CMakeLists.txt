cmake_minimum_required(VERSION 3.18)

include(cmake/Vcpkg.cmake)

project(CellForge VERSION 0.1.0 LANGUAGES C CXX)

include(cmake/VendorDependencies.cmake)
include(cmake/CellForgeComponents.cmake)
include(cmake/IntegrationTests.cmake)
