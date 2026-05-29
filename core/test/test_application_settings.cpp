// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <CellForge/ApplicationSettings.h>

#include <filesystem>
#include <string>

using namespace CellForge;
namespace fs = std::filesystem;

// ─── Helpers ──────────────────────────────────────────────────────────────────

struct TempFile {
    fs::path path;
    explicit TempFile(std::string_view name)
        : path(fs::temp_directory_path() / name) {}
    ~TempFile() { std::error_code ec; fs::remove(path, ec); }
};

// ApplicationSettings constructed with a path that does not exist — starts empty.
static ApplicationSettings makeEmpty() {
    return ApplicationSettings(fs::temp_directory_path() / "__cf_settings_nonexistent__.yaml");
}

// ─── Construction ─────────────────────────────────────────────────────────────

TEST_CASE("ApplicationSettings: construction with missing file produces empty settings", "[settings]") {
    auto s = makeEmpty();
    CHECK_FALSE(s.HasKey("anything"));
}

TEST_CASE("ApplicationSettings: Deserialize returns false for a missing file", "[settings]") {
    ApplicationSettings s(fs::temp_directory_path() / "__no_such_file__.yaml");
    CHECK_FALSE(s.Deserialize());
}

// ─── HasKey ───────────────────────────────────────────────────────────────────

TEST_CASE("ApplicationSettings: HasKey returns false before any Set", "[settings]") {
    auto s = makeEmpty();
    CHECK_FALSE(s.HasKey("key"));
}

TEST_CASE("ApplicationSettings: HasKey returns true after Set", "[settings]") {
    auto s = makeEmpty();
    s.Set("key", "value");
    CHECK(s.HasKey("key"));
}

TEST_CASE("ApplicationSettings: HasKey is case-sensitive", "[settings]") {
    auto s = makeEmpty();
    s.Set("Key", "value");
    CHECK(s.HasKey("Key"));
    CHECK_FALSE(s.HasKey("key"));
    CHECK_FALSE(s.HasKey("KEY"));
}

// ─── Get / Set ────────────────────────────────────────────────────────────────

TEST_CASE("ApplicationSettings: Get returns default when key is absent", "[settings]") {
    auto s = makeEmpty();
    CHECK(s.Get("missing", "default") == "default");
}

TEST_CASE("ApplicationSettings: Get returns empty string as default when not specified", "[settings]") {
    auto s = makeEmpty();
    CHECK(s.Get("missing") == "");
}

TEST_CASE("ApplicationSettings: Get returns stored value after Set", "[settings]") {
    auto s = makeEmpty();
    s.Set("greeting", "hello");
    CHECK(s.Get("greeting") == "hello");
}

TEST_CASE("ApplicationSettings: Set overwrites an existing value", "[settings]") {
    auto s = makeEmpty();
    s.Set("key", "first");
    s.Set("key", "second");
    CHECK(s.Get("key") == "second");
}

TEST_CASE("ApplicationSettings: multiple keys are independent", "[settings]") {
    auto s = makeEmpty();
    s.Set("a", "alpha");
    s.Set("b", "beta");
    CHECK(s.Get("a") == "alpha");
    CHECK(s.Get("b") == "beta");
}

// ─── GetFloat / SetFloat ──────────────────────────────────────────────────────

TEST_CASE("ApplicationSettings: GetFloat returns default when key is absent", "[settings]") {
    auto s = makeEmpty();
    CHECK(s.GetFloat("missing", 3.14f) == Catch::Approx(3.14f));
}

TEST_CASE("ApplicationSettings: GetFloat returns 0.0 as default when not specified", "[settings]") {
    auto s = makeEmpty();
    CHECK(s.GetFloat("missing") == Catch::Approx(0.0f));
}

TEST_CASE("ApplicationSettings: SetFloat / GetFloat round-trip", "[settings]") {
    auto s = makeEmpty();
    s.SetFloat("pi", 3.14159f);
    CHECK(s.GetFloat("pi") == Catch::Approx(3.14159f).epsilon(1e-4f));
}

TEST_CASE("ApplicationSettings: SetFloat makes HasKey return true", "[settings]") {
    auto s = makeEmpty();
    s.SetFloat("x", 1.0f);
    CHECK(s.HasKey("x"));
}

TEST_CASE("ApplicationSettings: SetFloat overwrites a previous float value", "[settings]") {
    auto s = makeEmpty();
    s.SetFloat("v", 1.0f);
    s.SetFloat("v", 9.0f);
    CHECK(s.GetFloat("v") == Catch::Approx(9.0f));
}

// ─── GetInt / SetInt ──────────────────────────────────────────────────────────

TEST_CASE("ApplicationSettings: GetInt returns default when key is absent", "[settings]") {
    auto s = makeEmpty();
    CHECK(s.GetInt("missing", 42) == 42);
}

TEST_CASE("ApplicationSettings: GetInt returns 0 as default when not specified", "[settings]") {
    auto s = makeEmpty();
    CHECK(s.GetInt("missing") == 0);
}

TEST_CASE("ApplicationSettings: SetInt / GetInt round-trip", "[settings]") {
    auto s = makeEmpty();
    s.SetInt("count", 100);
    CHECK(s.GetInt("count") == 100);
}

TEST_CASE("ApplicationSettings: SetInt with negative value", "[settings]") {
    auto s = makeEmpty();
    s.SetInt("offset", -7);
    CHECK(s.GetInt("offset") == -7);
}

TEST_CASE("ApplicationSettings: SetInt makes HasKey return true", "[settings]") {
    auto s = makeEmpty();
    s.SetInt("n", 0);
    CHECK(s.HasKey("n"));
}

TEST_CASE("ApplicationSettings: SetInt overwrites a previous int value", "[settings]") {
    auto s = makeEmpty();
    s.SetInt("n", 1);
    s.SetInt("n", 2);
    CHECK(s.GetInt("n") == 2);
}

// ─── Serialize / Deserialize round-trip ───────────────────────────────────────
// NOTE: These tests document the INTENDED behavior.
// Two bugs currently prevent them from passing:
//   1. Serialize writes "CellForge Application Settings" but Deserialize reads
//      "Hazel Application Settings" — key mismatch from an incomplete rename.
//   2. Serialize emits YAML::EndSeq instead of YAML::EndMap, producing
//      malformed YAML output.
// Once both are fixed these tests should pass without modification.

TEST_CASE("ApplicationSettings: round-trip preserves string value", "[settings][roundtrip]") {
    TempFile tmp("cf_settings_rt_str.yaml");

    {
        ApplicationSettings w(tmp.path);
        w.Set("theme", "dark");
        w.Serialize();
    }

    ApplicationSettings r(tmp.path);
    REQUIRE(r.HasKey("theme"));
    CHECK(r.Get("theme") == "dark");
}

TEST_CASE("ApplicationSettings: round-trip preserves float value", "[settings][roundtrip]") {
    TempFile tmp("cf_settings_rt_float.yaml");

    {
        ApplicationSettings w(tmp.path);
        w.SetFloat("zoom", 1.5f);
        w.Serialize();
    }

    ApplicationSettings r(tmp.path);
    REQUIRE(r.HasKey("zoom"));
    CHECK(r.GetFloat("zoom") == Catch::Approx(1.5f).epsilon(1e-4f));
}

TEST_CASE("ApplicationSettings: round-trip preserves int value", "[settings][roundtrip]") {
    TempFile tmp("cf_settings_rt_int.yaml");

    {
        ApplicationSettings w(tmp.path);
        w.SetInt("width", 1920);
        w.Serialize();
    }

    ApplicationSettings r(tmp.path);
    REQUIRE(r.HasKey("width"));
    CHECK(r.GetInt("width") == 1920);
}

TEST_CASE("ApplicationSettings: round-trip preserves multiple keys", "[settings][roundtrip]") {
    TempFile tmp("cf_settings_rt_multi.yaml");

    {
        ApplicationSettings w(tmp.path);
        w.Set("name",      "CellForge");
        w.SetInt("version", 1);
        w.SetFloat("scale", 2.0f);
        w.Serialize();
    }

    ApplicationSettings r(tmp.path);
    CHECK(r.Get("name")       == "CellForge");
    CHECK(r.GetInt("version") == 1);
    CHECK(r.GetFloat("scale") == Catch::Approx(2.0f));
}

TEST_CASE("ApplicationSettings: Deserialize returns false after Serialize to missing-dir path", "[settings]") {
    // Constructing with a path that never existed — Deserialize returns false.
    ApplicationSettings s(fs::temp_directory_path() / "subdir_xyz" / "settings.yaml");
    CHECK_FALSE(s.Deserialize());
}
