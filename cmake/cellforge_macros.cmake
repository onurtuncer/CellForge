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

  # ─── include-what-you-use (optional) ────────────────────────────────────
  # Off by default and inert unless both the option is enabled and an include-what-you-use binary is found. IWYU is
  # clang-based and does not understand MSVC cl.exe invocations, so it only takes effect when compiling with Clang.
  option(CELLFORGE_ENABLE_IWYU
         "Run include-what-you-use during compilation (requires Clang and include-what-you-use in PATH)" OFF)
  if(CELLFORGE_ENABLE_IWYU)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      find_program(CELLFORGE_IWYU_EXECUTABLE NAMES include-what-you-use iwyu)
      if(CELLFORGE_IWYU_EXECUTABLE)
        # --mapping_file suppresses umbrella-vs-detail-header noise for Boost/GTest (see cmake/iwyu/cellforge.imp);
        # --cxx17ns collapses suggested namespace blocks to C++17 nested-namespace syntax; --no_fwd_decls is set because
        # CellForge prefers full includes over forward declarations.
        set(CMAKE_CXX_INCLUDE_WHAT_USE
            "${CELLFORGE_IWYU_EXECUTABLE}"
            "-Xiwyu"
            "--mapping_file=${CMAKE_CURRENT_LIST_DIR}/iwyu/cellforge.imp"
            "-Xiwyu"
            "--cxx17ns"
            "-Xiwyu"
            "--no_fwd_decls"
            CACHE STRING "include-what-you-use invocation" FORCE)
      else()
        message(WARNING "CELLFORGE_ENABLE_IWYU is ON but include-what-you-use was not found in PATH")
      endif()
    else()
      message(WARNING "CELLFORGE_ENABLE_IWYU is ON but the compiler is not Clang (${CMAKE_CXX_COMPILER_ID}); "
                      "IWYU will not run")
    endif()
  endif()
endmacro()
