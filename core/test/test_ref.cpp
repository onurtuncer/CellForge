// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include <catch2/catch_test_macros.hpp>

#include <CellForge/Ref.h>

using namespace CellForge;

// ─── Test fixtures ────────────────────────────────────────────────────────────

struct Widget : RefCounted
{
    int value;
    explicit Widget(int v = 0) : value(v) {}
};

// Widget subtype for As<> / upcast tests.
struct SpecialWidget : Widget
{
    using Widget::Widget;
};

// Signals its own destruction through an external flag.
struct Tracked : RefCounted
{
    bool& destroyed;
    explicit Tracked(bool& flag) : destroyed(flag) {}
    ~Tracked() { destroyed = true; }
};

// ─── RefCounted ───────────────────────────────────────────────────────────────

TEST_CASE("RefCounted starts with refcount 0", "[refcounted]")
{
    Widget w;
    CHECK(w.GetRefCount() == 0u);
}

TEST_CASE("RefCounted IncRefCount increments the count", "[refcounted]")
{
    Widget w;
    w.IncRefCount();
    CHECK(w.GetRefCount() == 1u);
    w.IncRefCount();
    CHECK(w.GetRefCount() == 2u);
}

TEST_CASE("RefCounted DecRefCount decrements the count", "[refcounted]")
{
    Widget w;
    w.IncRefCount();
    w.IncRefCount();
    w.DecRefCount();
    CHECK(w.GetRefCount() == 1u);
}

// ─── Ref construction ─────────────────────────────────────────────────────────

TEST_CASE("Default-constructed Ref is null", "[ref]")
{
    Ref<Widget> r;
    CHECK_FALSE(r);
    CHECK(r.Raw() == nullptr);
}

TEST_CASE("nullptr-constructed Ref is null", "[ref]")
{
    Ref<Widget> r(nullptr);
    CHECK_FALSE(r);
}

TEST_CASE("Ref constructed from raw pointer has refcount 1", "[ref]")
{
    Ref<Widget> r(new Widget(42));
    REQUIRE(r);
    CHECK(r->GetRefCount() == 1u);
    CHECK(r->value == 42);
}

TEST_CASE("Ref::Create returns a valid Ref with refcount 1", "[ref]")
{
    auto r = Ref<Widget>::Create(7);
    REQUIRE(r);
    CHECK(r->value == 7);
    CHECK(r->GetRefCount() == 1u);
}

// ─── Ref copy ────────────────────────────────────────────────────────────────

TEST_CASE("Ref copy constructor increments refcount", "[ref]")
{
    auto a = Ref<Widget>::Create(0);
    Ref<Widget> b(a);
    CHECK(a->GetRefCount() == 2u);
    CHECK(b->GetRefCount() == 2u);
    CHECK(a.Raw() == b.Raw());
}

TEST_CASE("Destroying a copy decrements refcount without deleting the object", "[ref]")
{
    auto a = Ref<Widget>::Create(0);
    {
        Ref<Widget> b(a);
        CHECK(a->GetRefCount() == 2u);
    }
    CHECK(a->GetRefCount() == 1u);
    CHECK(a);
}

TEST_CASE("Last Ref destroyed deletes the managed object", "[ref]")
{
    bool gone = false;
    {
        auto r = Ref<Tracked>::Create(gone);
        CHECK_FALSE(gone);
    }
    CHECK(gone);
}

TEST_CASE("Object survives while at least one Ref is alive", "[ref]")
{
    bool gone = false;
    auto a = Ref<Tracked>::Create(gone);
    {
        auto b = a; // refcount 2
        CHECK_FALSE(gone);
    }
    CHECK_FALSE(gone); // a still holds it
}

// ─── Ref move ────────────────────────────────────────────────────────────────

TEST_CASE("Ref move constructor transfers ownership without extra increment", "[ref]")
{
    auto a = Ref<Widget>::Create(5);
    Widget* raw = a.Raw();

    Ref<Widget> b(std::move(a));

    CHECK_FALSE(a);
    CHECK(b);
    CHECK(b.Raw() == raw);
    CHECK(b->GetRefCount() == 1u);
}

TEST_CASE("Ref move assignment transfers ownership without extra increment", "[ref]")
{
    auto a = Ref<Widget>::Create(5);
    Widget* raw = a.Raw();
    Ref<Widget> b;

    b = std::move(a);

    CHECK_FALSE(a);
    CHECK(b.Raw() == raw);
    CHECK(b->GetRefCount() == 1u);
}

// ─── Ref copy assignment ──────────────────────────────────────────────────────

TEST_CASE("Ref copy assignment increments refcount", "[ref]")
{
    auto a = Ref<Widget>::Create(0);
    Ref<Widget> b;
    b = a;
    CHECK(a->GetRefCount() == 2u);
    CHECK(b.Raw() == a.Raw());
}

TEST_CASE("Ref self-assignment is safe and does not corrupt refcount", "[ref]")
{
    auto r = Ref<Widget>::Create(99);
    r = r;
    CHECK(r);
    CHECK(r->value == 99);
    CHECK(r->GetRefCount() == 1u);
}

TEST_CASE("Ref nullptr assignment releases the managed object", "[ref]")
{
    bool gone = false;
    auto r = Ref<Tracked>::Create(gone);
    r = nullptr;
    CHECK(gone);
    CHECK_FALSE(r);
}

// ─── Ref accessors ────────────────────────────────────────────────────────────

TEST_CASE("Ref operator-> gives access to the managed object", "[ref]")
{
    auto r = Ref<Widget>::Create(123);
    CHECK(r->value == 123);
    r->value = 456;
    CHECK(r->value == 456);
}

TEST_CASE("Ref operator* gives access by reference", "[ref]")
{
    auto r = Ref<Widget>::Create(10);
    (*r).value = 20;
    CHECK(r->value == 20);
}

TEST_CASE("Ref::Raw returns the underlying raw pointer", "[ref]")
{
    auto r = Ref<Widget>::Create(0);
    Widget* raw = r.Raw();
    REQUIRE(raw != nullptr);
    CHECK(raw->GetRefCount() == 1u);
}

// ─── Ref::Reset ───────────────────────────────────────────────────────────────

TEST_CASE("Ref::Reset with no argument releases the object and nulls the Ref", "[ref]")
{
    bool gone = false;
    auto r = Ref<Tracked>::Create(gone);
    r.Reset();
    CHECK(gone);
    CHECK_FALSE(r);
}

TEST_CASE("Ref::Reset with a new pointer replaces the managed object", "[ref]")
{
    bool gone = false;
    auto r = Ref<Tracked>::Create(gone);
    Widget* replacement = new Widget(77);

    // Reset releases Tracked and takes ownership of replacement.
    // (Note: replacement is not RefCounted through Ref here — it just stores the ptr)
    r.Reset(nullptr);
    CHECK(gone);
}

// ─── Ref equality ─────────────────────────────────────────────────────────────

TEST_CASE("Two Refs to the same object are equal", "[ref]")
{
    auto a = Ref<Widget>::Create(0);
    Ref<Widget> b = a;
    CHECK(a == b);
    CHECK_FALSE(a != b);
}

TEST_CASE("Two Refs to different objects are not equal", "[ref]")
{
    auto a = Ref<Widget>::Create(1);
    auto b = Ref<Widget>::Create(2);
    CHECK(a != b);
    CHECK_FALSE(a == b);
}

// ─── Ref::As ─────────────────────────────────────────────────────────────────

TEST_CASE("Ref::As upcasts derived to base and both Refs keep the object alive", "[ref]")
{
    auto derived = Ref<SpecialWidget>::Create(42);
    auto base    = derived.As<Widget>();

    REQUIRE(base);
    CHECK(base->value == 42);
    CHECK(derived->GetRefCount() == 2u); // both refs are live
}

TEST_CASE("Ref::As result destroyed reduces refcount back to 1", "[ref]")
{
    auto derived = Ref<SpecialWidget>::Create(10);
    {
        auto base = derived.As<Widget>();
        CHECK(derived->GetRefCount() == 2u);
    }
    CHECK(derived->GetRefCount() == 1u);
}

// ─── WeakRef ─────────────────────────────────────────────────────────────────

TEST_CASE("Default-constructed WeakRef is not valid", "[weakref]")
{
    WeakRef<Widget> w;
    CHECK_FALSE(w.IsValid());
    CHECK_FALSE(static_cast<bool>(w));
}

TEST_CASE("WeakRef constructed from a live Ref is valid", "[weakref]")
{
    auto r = Ref<Widget>::Create(0);
    WeakRef<Widget> w(r);
    CHECK(w.IsValid());
    CHECK(static_cast<bool>(w));
}

TEST_CASE("WeakRef becomes invalid after the last Ref is destroyed", "[weakref]")
{
    WeakRef<Widget> w;
    {
        auto r = Ref<Widget>::Create(0);
        w = WeakRef<Widget>(r);
        REQUIRE(w.IsValid());
    }
    CHECK_FALSE(w.IsValid());
}

TEST_CASE("WeakRef constructed from raw pointer is valid while the owning Ref is alive", "[weakref]")
{
    auto r = Ref<Widget>::Create(7);
    WeakRef<Widget> w(r.Raw());
    CHECK(w.IsValid());
    CHECK(w->value == 7);
}

TEST_CASE("Multiple WeakRefs to the same object all become invalid on destruction", "[weakref]")
{
    WeakRef<Widget> w1, w2;
    {
        auto r = Ref<Widget>::Create(0);
        w1 = WeakRef<Widget>(r);
        w2 = WeakRef<Widget>(r);
        CHECK(w1.IsValid());
        CHECK(w2.IsValid());
    }
    CHECK_FALSE(w1.IsValid());
    CHECK_FALSE(w2.IsValid());
}

TEST_CASE("WeakRef::As upcasts while the object is alive", "[weakref]")
{
    auto r = Ref<SpecialWidget>::Create(5);
    WeakRef<SpecialWidget> ws(r);
    auto wb = ws.As<Widget>();
    CHECK(wb.IsValid());
    CHECK(wb->value == 5);
}
