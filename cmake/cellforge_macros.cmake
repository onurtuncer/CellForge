# CellForge-wide build settings, mirroring the tesseract_variables() pattern.
# Include this file in every CellForge component CMakeLists.txt, then call
# cellforge_variables() immediately after project().
#
# Provides:
#   CELLFORGE_CXX_VERSION              — C++ standard (currently 17)
#   CELLFORGE_COMPILE_OPTIONS_PRIVATE  — warnings, only on CellForge sources
#   CELLFORGE_COMPILE_OPTIONS_PUBLIC   — options exported to consumers
#   CELLFORGE_COMPILE_DEFINITIONS      — preprocessor definitions

macro(cellforge_variables)
  set(CELLFORGE_CXX_VERSION 20)
  set(CELLFORGE_COMPILE_DEFINITIONS "")

  if(MSVC)
    set(CELLFORGE_COMPILE_OPTIONS_PRIVATE
        /W4 # high warning level
        /WX- # warnings are not errors (relax during early development)
        /wd4251 # suppress DLL-interface warnings from STL members
    )
    set(CELLFORGE_COMPILE_OPTIONS_PUBLIC "")
  else()
    set(CELLFORGE_COMPILE_OPTIONS_PRIVATE
        -Wall
        -Wextra
        -Wshadow
        -Wnon-virtual-dtor
        -Wno-unused-parameter)
    set(CELLFORGE_COMPILE_OPTIONS_PUBLIC "")
  endif()
endmacro()
