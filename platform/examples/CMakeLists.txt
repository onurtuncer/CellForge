cmake_minimum_required(VERSION 3.18)

# Qt example
if(CELLFORGE_PLATFORM_QT_BUILD_EXAMPLES)
    add_subdirectory(qt)
endif()

# MFC example (Windows only)
if(WIN32 AND CELLFORGE_PLATFORM_MFC_BUILD_EXAMPLES)
    add_subdirectory(mfc)
endif()
