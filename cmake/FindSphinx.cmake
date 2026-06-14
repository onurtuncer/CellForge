#[=======================================================================[.rst:
FindSphinx
----------

Locates the ``sphinx-build`` executable used to render the CellForge HTML
documentation.

Result Variables
^^^^^^^^^^^^^^^^^

``Sphinx_FOUND``
  True if ``sphinx-build`` was found.
``SPHINX_EXECUTABLE``
  Path to the ``sphinx-build`` executable.

#]=======================================================================]

find_program(SPHINX_EXECUTABLE NAMES sphinx-build sphinx-build.exe DOC "Path to the sphinx-build executable")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Sphinx REQUIRED_VARS SPHINX_EXECUTABLE)

mark_as_advanced(SPHINX_EXECUTABLE)
