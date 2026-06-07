# CellForge first-party DLL registry.
#
# Add a new component by:
#   1. Creating <name>/CMakeLists.txt + <name>/package.xml.
#   2. Adding an option + add_subdirectory block below.
#
# Each component must end with:
#   set(SUPPORTED_COMPONENTS ${SUPPORTED_COMPONENTS} "<name>" PARENT_SCOPE)
# so the root configure_package() picks it up.
#
# Components that need a vendor package (e.g. tesseract) must add the
# corresponding vendor ExternalProject target to their own build step via a
# CMake dependency (add_dependencies) or by checking at configure time that
# the vendor headers exist.  For now, viewer only needs vcpkg packages
# (Qt6, OpenCASCADE) which are available at configure time.

option(CELLFORGE_BUILD_CORE "Build core platform-agnostic library" ON)
option(CELLFORGE_BUILD_WORKCELL "Build flecs ECS workcell library" ON)
option(CELLFORGE_BUILD_VIEWER "Build Qt6/OCCT viewer library" ON)
option(CELLFORGE_VIEWER_BUILD_EXAMPLES "Build viewer example application" OFF)
option(CELLFORGE_BUILD_PLATFORM_QT "Build Qt platform backend" ON)
option(CELLFORGE_BUILD_PERSISTENCE "Build OCAF document persistence backend" ON)
option(CELLFORGE_BUILD_PROJECT "Build offline robot programming project model" ON)
option(CELLFORGE_BUILD_ROBOT "Build RobotModel (Tesseract + Pinocchio dual backend)" OFF)

if(CELLFORGE_BUILD_CORE)
  add_subdirectory(core)
endif()

if(CELLFORGE_BUILD_WORKCELL)
  add_subdirectory(workcell)
endif()

if(CELLFORGE_BUILD_VIEWER)
  add_subdirectory(gui)
endif()

# platform must come after core (depends on CellForge::core target)
if(CELLFORGE_BUILD_PLATFORM_QT)
  add_subdirectory(platform)
endif()

# persistence must come after core (IPersistenceBackend lives in CellForge::core)
if(CELLFORGE_BUILD_PERSISTENCE)
  add_subdirectory(persistence)
endif()

# project must come after core (IPersistenceBackend interface lives in CellForge::core)
if(CELLFORGE_BUILD_PROJECT)
  add_subdirectory(project)
endif()

# robot requires pinocchio (vendor) and tesseract (vendor) — both built via ExternalProject
if(CELLFORGE_BUILD_ROBOT)
  add_subdirectory(robot)
endif()
