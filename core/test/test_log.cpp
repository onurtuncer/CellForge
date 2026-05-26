// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <catch2/catch_test_macros.hpp>

#include <spdlog/sinks/ostream_sink.h>

#include <CellForge/Log.h>

#include <sstream>
#include <string_view>

using namespace CellForge;

// ─── Capturing fixture ────────────────────────────────────────────────────────
//
// Appends an ostream sink to each logger after Init() so tests can inspect
// what was actually written without touching the file sinks created by Init().

struct LogFixture
{
    std::ostringstream coreOut;
    std::ostringstream clientOut;

    LogFixture()
    {
        Log::Init();

        auto coreSink = std::make_shared<spdlog::sinks::ostream_sink_mt>(coreOut);
        Log::GetCoreLogger()->set_level(spdlog::level::trace);
        Log::GetCoreLogger()->flush_on(spdlog::level::trace);
        Log::GetCoreLogger()->sinks().push_back(coreSink);

        auto clientSink = std::make_shared<spdlog::sinks::ostream_sink_mt>(clientOut);
        Log::GetClientLogger()->set_level(spdlog::level::trace);
        Log::GetClientLogger()->flush_on(spdlog::level::trace);
        Log::GetClientLogger()->sinks().push_back(clientSink);
    }

    ~LogFixture() { Log::Shutdown(); }

    bool coreHas(std::string_view s)   const { return coreOut.str().find(s)   != std::string::npos; }
    bool clientHas(std::string_view s) const { return clientOut.str().find(s) != std::string::npos; }

    void setTag(std::string_view tag, bool enabled, Log::Level filter = Log::Level::Trace)
    {
        Log::EnabledTags()[std::string(tag)] = { enabled, filter };
    }
};

// ─── Lifecycle ────────────────────────────────────────────────────────────────

TEST_CASE("Log::Init creates non-null core and client loggers", "[log][lifecycle]")
{
    Log::Init();
    REQUIRE(Log::GetCoreLogger()   != nullptr);
    REQUIRE(Log::GetClientLogger() != nullptr);
    Log::Shutdown();
}

TEST_CASE("Log::Shutdown nulls core and client loggers", "[log][lifecycle]")
{
    Log::Init();
    Log::Shutdown();
    CHECK(Log::GetCoreLogger()   == nullptr);
    CHECK(Log::GetClientLogger() == nullptr);
}

TEST_CASE("Log::Init and Shutdown can be called multiple times", "[log][lifecycle]")
{
    for (int i = 0; i < 3; ++i)
    {
        Log::Init();
        REQUIRE(Log::GetCoreLogger()   != nullptr);
        REQUIRE(Log::GetClientLogger() != nullptr);
        Log::Shutdown();
    }
}

// ─── Level utilities ─────────────────────────────────────────────────────────

TEST_CASE("Log::LevelToString returns the correct string for every level", "[log][utils]")
{
    CHECK(std::string_view(Log::LevelToString(Log::Level::Trace)) == "Trace");
    CHECK(std::string_view(Log::LevelToString(Log::Level::Info))  == "Info");
    CHECK(std::string_view(Log::LevelToString(Log::Level::Warn))  == "Warn");
    CHECK(std::string_view(Log::LevelToString(Log::Level::Error)) == "Error");
    CHECK(std::string_view(Log::LevelToString(Log::Level::Fatal)) == "Fatal");
}

TEST_CASE("Log::LevelFromString roundtrips every level name", "[log][utils]")
{
    CHECK(Log::LevelFromString("Trace") == Log::Level::Trace);
    CHECK(Log::LevelFromString("Info")  == Log::Level::Info);
    CHECK(Log::LevelFromString("Warn")  == Log::Level::Warn);
    CHECK(Log::LevelFromString("Error") == Log::Level::Error);
    CHECK(Log::LevelFromString("Fatal") == Log::Level::Fatal);
}

TEST_CASE("Log::LevelFromString falls back to Trace for unknown input", "[log][utils]")
{
    CHECK(Log::LevelFromString("bogus")   == Log::Level::Trace);
    CHECK(Log::LevelFromString("")        == Log::Level::Trace);
    CHECK(Log::LevelFromString("TRACE")   == Log::Level::Trace); // case-sensitive
}

TEST_CASE("Log::LevelToString and LevelFromString roundtrip is stable", "[log][utils]")
{
    for (auto lvl : { Log::Level::Trace, Log::Level::Info, Log::Level::Warn,
                      Log::Level::Error, Log::Level::Fatal })
    {
        CHECK(Log::LevelFromString(Log::LevelToString(lvl)) == lvl);
    }
}

// ─── Tag management ──────────────────────────────────────────────────────────

TEST_CASE("Log::SetDefaultTagSettings populates the built-in tag set", "[log][tags]")
{
    Log::Init();
    Log::SetDefaultTagSettings();

    CHECK(Log::HasTag("Animation"));
    CHECK(Log::HasTag("Core"));
    CHECK(Log::HasTag("Memory"));
    CHECK(Log::HasTag("Project"));
    CHECK(Log::HasTag("Renderer"));
    CHECK(Log::HasTag("Workcell"));
    CHECK(Log::HasTag("Timer"));

    Log::Shutdown();
}

TEST_CASE("Log::HasTag returns false for an unknown tag", "[log][tags]")
{
    Log::Init();
    CHECK_FALSE(Log::HasTag("__no_such_tag_xyz__"));
    Log::Shutdown();
}

TEST_CASE("Log::EnabledTags map is mutable by callers", "[log][tags]")
{
    Log::Init();

    Log::EnabledTags()["MyTag"] = { true, Log::Level::Warn };

    REQUIRE(Log::HasTag("MyTag"));
    CHECK(Log::EnabledTags()["MyTag"].Enabled     == true);
    CHECK(Log::EnabledTags()["MyTag"].LevelFilter == Log::Level::Warn);

    Log::Shutdown();
}

TEST_CASE("Log::SetDefaultTagSettings replaces any caller-added tags", "[log][tags]")
{
    Log::Init();

    Log::EnabledTags()["UserTag"] = { true, Log::Level::Trace };
    REQUIRE(Log::HasTag("UserTag"));

    Log::SetDefaultTagSettings();

    CHECK_FALSE(Log::HasTag("UserTag")); // wiped by reset

    Log::Shutdown();
}

// ─── Output routing ──────────────────────────────────────────────────────────

TEST_CASE_METHOD(LogFixture, "PrintMessageTag (string) routes to core logger only", "[log][output]")
{
    setTag("T", true);
    Log::PrintMessageTag(Log::Type::Core, Log::Level::Info, "T", "core-side");

    CHECK(coreHas("core-side"));
    CHECK_FALSE(clientHas("core-side"));
}

TEST_CASE_METHOD(LogFixture, "PrintMessageTag (string) routes to client logger only", "[log][output]")
{
    setTag("T", true);
    Log::PrintMessageTag(Log::Type::Client, Log::Level::Warn, "T", "client-side");

    CHECK(clientHas("client-side"));
    CHECK_FALSE(coreHas("client-side"));
}

TEST_CASE_METHOD(LogFixture, "PrintMessageTag includes the tag name in the output line", "[log][output]")
{
    setTag("Renderer", true);
    Log::PrintMessageTag(Log::Type::Core, Log::Level::Info, "Renderer", "frame ready");

    CHECK(coreHas("Renderer"));
    CHECK(coreHas("frame ready"));
}

TEST_CASE_METHOD(LogFixture, "PrintMessageTag (format) interpolates arguments correctly", "[log][output]")
{
    setTag("Fmt", true);
    Log::PrintMessageTag(Log::Type::Core, Log::Level::Info, "Fmt", "n={} s={}", 7, "hello");

    CHECK(coreHas("n=7"));
    CHECK(coreHas("s=hello"));
}

TEST_CASE_METHOD(LogFixture, "PrintMessage routes to core logger only", "[log][output]")
{
    Log::PrintMessage(Log::Type::Core, Log::Level::Info, "plain-core");

    CHECK(coreHas("plain-core"));
    CHECK_FALSE(clientHas("plain-core"));
}

TEST_CASE_METHOD(LogFixture, "PrintMessage routes to client logger only", "[log][output]")
{
    Log::PrintMessage(Log::Type::Client, Log::Level::Warn, "plain-client");

    CHECK(clientHas("plain-client"));
    CHECK_FALSE(coreHas("plain-client"));
}

// ─── Tag-level filtering ─────────────────────────────────────────────────────

TEST_CASE_METHOD(LogFixture, "Messages below a tag LevelFilter are suppressed", "[log][filtering]")
{
    setTag("Noisy", true, Log::Level::Warn);

    Log::PrintMessageTag(Log::Type::Core, Log::Level::Trace, "Noisy", "suppressed-trace");
    Log::PrintMessageTag(Log::Type::Core, Log::Level::Info,  "Noisy", "suppressed-info");
    Log::PrintMessageTag(Log::Type::Core, Log::Level::Warn,  "Noisy", "passed-warn");
    Log::PrintMessageTag(Log::Type::Core, Log::Level::Error, "Noisy", "passed-error");
    Log::PrintMessageTag(Log::Type::Core, Log::Level::Fatal, "Noisy", "passed-fatal");

    CHECK_FALSE(coreHas("suppressed-trace"));
    CHECK_FALSE(coreHas("suppressed-info"));
    CHECK(coreHas("passed-warn"));
    CHECK(coreHas("passed-error"));
    CHECK(coreHas("passed-fatal"));
}

TEST_CASE_METHOD(LogFixture, "A disabled tag suppresses all messages regardless of level", "[log][filtering]")
{
    setTag("Silent", false);

    Log::PrintMessageTag(Log::Type::Core, Log::Level::Fatal, "Silent", "silenced-fatal");
    Log::PrintMessageTag(Log::Type::Core, Log::Level::Error, "Silent", "silenced-error");

    CHECK_FALSE(coreHas("silenced-fatal"));
    CHECK_FALSE(coreHas("silenced-error"));
}

TEST_CASE_METHOD(LogFixture, "Re-enabling a disabled tag restores output", "[log][filtering]")
{
    setTag("Toggle", false);
    Log::PrintMessageTag(Log::Type::Core, Log::Level::Info, "Toggle", "while-disabled");

    Log::EnabledTags()["Toggle"].Enabled = true;
    Log::PrintMessageTag(Log::Type::Core, Log::Level::Info, "Toggle", "after-enabled");

    CHECK_FALSE(coreHas("while-disabled"));
    CHECK(coreHas("after-enabled"));
}

TEST_CASE_METHOD(LogFixture, "Raising LevelFilter mid-stream filters subsequent messages", "[log][filtering]")
{
    setTag("Dyn", true, Log::Level::Trace);
    Log::PrintMessageTag(Log::Type::Core, Log::Level::Info, "Dyn", "before-raise");

    Log::EnabledTags()["Dyn"].LevelFilter = Log::Level::Error;
    Log::PrintMessageTag(Log::Type::Core, Log::Level::Info, "Dyn", "after-raise-info");
    Log::PrintMessageTag(Log::Type::Core, Log::Level::Error, "Dyn", "after-raise-error");

    CHECK(coreHas("before-raise"));
    CHECK_FALSE(coreHas("after-raise-info"));
    CHECK(coreHas("after-raise-error"));
}

// ─── Assert messages ─────────────────────────────────────────────────────────

TEST_CASE_METHOD(LogFixture, "PrintAssertMessage (prefix only) writes to core logger", "[log][assert]")
{
    Log::PrintAssertMessage(Log::Type::Core, "CoreAssertPrefix");

    CHECK(coreHas("CoreAssertPrefix"));
    CHECK_FALSE(clientHas("CoreAssertPrefix"));
}

TEST_CASE_METHOD(LogFixture, "PrintAssertMessage (prefix only) writes to client logger", "[log][assert]")
{
    Log::PrintAssertMessage(Log::Type::Client, "ClientAssertPrefix");

    CHECK(clientHas("ClientAssertPrefix"));
    CHECK_FALSE(coreHas("ClientAssertPrefix"));
}

TEST_CASE_METHOD(LogFixture, "PrintAssertMessage (format) includes both prefix and message", "[log][assert]")
{
    Log::PrintAssertMessage(Log::Type::Core, "Prefix", "expected={} got={}", 1, 2);

    CHECK(coreHas("Prefix"));
    CHECK(coreHas("expected=1"));
    CHECK(coreHas("got=2"));
}
