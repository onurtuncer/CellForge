// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <catch2/catch_test_macros.hpp>

#include <CellForge/Base.h>

#include <thread>
#include <vector>

using namespace CellForge;

// ─── Flag ─────────────────────────────────────────────────────────────────────

TEST_CASE("Flag: default constructed is clean", "[flag]") {
    Flag f;
    CHECK(f.IsDirty() == false);
}

TEST_CASE("Flag: CheckAndResetIfDirty returns false when clean", "[flag]") {
    Flag f;
    CHECK(f.CheckAndResetIfDirty() == false);
}

TEST_CASE("Flag: IsDirty is true after SetDirty", "[flag]") {
    Flag f;
    f.SetDirty();
    CHECK(f.IsDirty() == true);
}

TEST_CASE("Flag: CheckAndResetIfDirty returns true when dirty", "[flag]") {
    Flag f;
    f.SetDirty();
    CHECK(f.CheckAndResetIfDirty() == true);
}

TEST_CASE("Flag: CheckAndResetIfDirty resets the flag to clean", "[flag]") {
    Flag f;
    f.SetDirty();
    f.CheckAndResetIfDirty();
    CHECK(f.IsDirty() == false);
}

TEST_CASE("Flag: second CheckAndResetIfDirty returns false after reset", "[flag]") {
    Flag f;
    f.SetDirty();
    f.CheckAndResetIfDirty();
    CHECK(f.CheckAndResetIfDirty() == false);
}

TEST_CASE("Flag: SetDirty is idempotent", "[flag]") {
    Flag f;
    f.SetDirty();
    f.SetDirty();
    CHECK(f.IsDirty() == true);
    CHECK(f.CheckAndResetIfDirty() == true);
}

TEST_CASE("Flag: can be dirtied again after reset", "[flag]") {
    Flag f;
    f.SetDirty();
    f.CheckAndResetIfDirty();
    f.SetDirty();
    CHECK(f.IsDirty() == true);
    CHECK(f.CheckAndResetIfDirty() == true);
    CHECK(f.IsDirty() == false);
}

TEST_CASE("Flag: copy-constructed flag is independent", "[flag]") {
    Flag original;
    original.SetDirty();

    Flag copy = original;
    // Mutating the copy does not affect the original.
    copy.CheckAndResetIfDirty();
    CHECK(original.IsDirty() == true);
}

// ─── AtomicFlag ───────────────────────────────────────────────────────────────

TEST_CASE("AtomicFlag: default constructed is clean", "[atomicflag]") {
    AtomicFlag f;
    CHECK(f.CheckAndResetIfDirty() == false);
}

TEST_CASE("AtomicFlag: CheckAndResetIfDirty returns true after SetDirty", "[atomicflag]") {
    AtomicFlag f;
    f.SetDirty();
    CHECK(f.CheckAndResetIfDirty() == true);
}

TEST_CASE("AtomicFlag: CheckAndResetIfDirty resets to clean", "[atomicflag]") {
    AtomicFlag f;
    f.SetDirty();
    f.CheckAndResetIfDirty();
    CHECK(f.CheckAndResetIfDirty() == false);
}

TEST_CASE("AtomicFlag: second CheckAndResetIfDirty returns false after reset", "[atomicflag]") {
    AtomicFlag f;
    f.SetDirty();
    f.CheckAndResetIfDirty(); // consumes
    CHECK(f.CheckAndResetIfDirty() == false);
}

TEST_CASE("AtomicFlag: can be dirtied again after reset", "[atomicflag]") {
    AtomicFlag f;
    f.SetDirty();
    f.CheckAndResetIfDirty();
    f.SetDirty();
    CHECK(f.CheckAndResetIfDirty() == true);
    CHECK(f.CheckAndResetIfDirty() == false);
}

TEST_CASE("AtomicFlag: copy-constructed flag starts clean (independent of source)", "[atomicflag]") {
    AtomicFlag original;
    original.SetDirty();

    AtomicFlag copy = original;
    // The copy starts from its own default-constructed state, not inherited from original.
    // Per the class comment: "the copied flag is going to be reset."
    CHECK(copy.CheckAndResetIfDirty() == false);
}

TEST_CASE("AtomicFlag: copy-assignment produces an independent flag", "[atomicflag]") {
    AtomicFlag a;
    a.SetDirty();
    AtomicFlag b;
    b = a;

    // b starts clean regardless of a's state.
    CHECK(b.CheckAndResetIfDirty() == false);
    // a's state is unaffected by the assignment.
    CHECK(a.CheckAndResetIfDirty() == true);
}

TEST_CASE("AtomicFlag: single writer, single reader — dirty survives across threads", "[atomicflag]") {
    AtomicFlag f;

    std::thread writer([&f] { f.SetDirty(); });
    writer.join();

    CHECK(f.CheckAndResetIfDirty() == true);
}

TEST_CASE("AtomicFlag: CheckAndResetIfDirty is consumed exactly once under concurrent readers", "[atomicflag]") {
    // One writer marks dirty; N readers race to consume. Exactly one should win.
    constexpr int N = 8;
    AtomicFlag f;
    f.SetDirty();

    std::vector<bool> results(N, false);
    std::vector<std::thread> threads;
    threads.reserve(N);

    for (int i = 0; i < N; ++i)
        threads.emplace_back([&f, &results, i] {
            results[i] = f.CheckAndResetIfDirty();
        });

    for (auto& t : threads) t.join();

    int trueCount = 0;
    for (bool r : results) if (r) ++trueCount;

    CHECK(trueCount == 1);
}
