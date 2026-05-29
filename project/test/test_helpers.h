// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include <CellForge/OcafPersistenceBackend.h>
#include <CellForge/Project.h>

#include <filesystem>
#include <string_view>

namespace fs = std::filesystem;

// Deletes a temp file on destruction.
struct TempFile {
    fs::path path;
    explicit TempFile(std::string_view name)
        : path(fs::temp_directory_path() / name) {}
    ~TempFile() { std::error_code ec; fs::remove(path, ec); }
};

// Opens a fresh project named "Test" — shared by items and undo tests.
struct ProjectFixture {
    CellForge::OcafPersistenceBackend backend;
    CellForge::Project project{ backend };
    ProjectFixture() { project.newProject("Test"); }
};
