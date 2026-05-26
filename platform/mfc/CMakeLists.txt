# CMAKE_MFC_FLAG must be set in its own subdirectory so the VS generator applies
# UseOfMfc=Dynamic only to the targets defined here.  Setting it in the parent
# directory would propagate it to the Qt library, which doesn't need MFC headers.
set(CMAKE_MFC_FLAG 2)   # 2 = Use MFC in a Shared DLL

add_library(cellforge_platform_mfc SHARED
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/mfc/MfcApplicationPlatform.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/mfc/MfcViewportWnd.cpp
)
add_library(CellForge::platform_mfc ALIAS cellforge_platform_mfc)
set_target_properties(cellforge_platform_mfc PROPERTIES OUTPUT_NAME cellforge_platform_mfc)

target_cxx_version(cellforge_platform_mfc PUBLIC VERSION ${CELLFORGE_CXX_VERSION})
target_compile_options(cellforge_platform_mfc PRIVATE ${CELLFORGE_COMPILE_OPTIONS_PRIVATE})
target_compile_options(cellforge_platform_mfc PUBLIC  ${CELLFORGE_COMPILE_OPTIONS_PUBLIC})
target_compile_definitions(cellforge_platform_mfc PUBLIC ${CELLFORGE_COMPILE_DEFINITIONS})

target_compile_definitions(cellforge_platform_mfc PUBLIC
    CF_PLATFORM_WINDOWS
    _AFXDLL     # shared MFC DLL
    UNICODE
    _UNICODE
)

target_compile_definitions(cellforge_platform_mfc PUBLIC
    $<$<CONFIG:Debug>:CF_DEBUG>
    $<$<CONFIG:Release>:CF_RELEASE>
    $<$<CONFIG:RelWithDebInfo>:CF_RELEASE>
    $<$<CONFIG:MinSizeRel>:CF_DIST>
)

target_include_directories(cellforge_platform_mfc
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../include/CellForge
)

target_link_libraries(cellforge_platform_mfc
    PUBLIC
        CellForge::core
)

set(SUPPORTED_COMPONENTS ${SUPPORTED_COMPONENTS} "platform_mfc" PARENT_SCOPE)
