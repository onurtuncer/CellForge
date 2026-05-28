# ------------------------------------------------------------------------------------
# Project: CellForge
# Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
# Author: Onur Tuncer, PhD
#
# SPDX-License-Identifier: LGPL-2.1-only
# License-Filename: LICENSE
# ------------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.18)

# Qt example
if(CELLFORGE_PLATFORM_QT_BUILD_EXAMPLES)
    add_subdirectory(qt)
endif()

# MFC example (Windows only)
if(WIN32 AND CELLFORGE_PLATFORM_MFC_BUILD_EXAMPLES)
    add_subdirectory(mfc)
endif()
