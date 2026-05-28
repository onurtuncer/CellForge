// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include "CellForge/Mathematics.h"
#include "CellForge/IGeometryBackend.h"

#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace CellForge {

// ─── Opaque handles ───────────────────────────────────────────────────────────
// The backend owns the lifetime of every document and node it creates.
// 0 is the null / invalid sentinel for both types.

using DocumentId = uint64_t;
inline constexpr DocumentId NullDocument = 0;

using NodeId = uint64_t;
inline constexpr NodeId NullNode = 0;

// ─── IPersistenceBackend ─────────────────────────────────────────────────────
// Pure interface that isolates all document-management and persistence
// operations (OCAF / XDE or any other backend) behind a single boundary.
// Concrete implementations live in their own library and are the only
// translation units that #include OCAF.
//
// Document model
//   A document contains a tree of Nodes.  Each node can carry:
//     • A display name
//     • A geometry reference (ShapeId managed by IGeometryBackend)
//     • A local-placement transform (assembly/component positioning)
//     • Arbitrary named attributes: real, integer, boolean, string
//   Mutations are wrapped in named transactions that feed the undo/redo stack.
//
// Ownership rules
//   • The backend owns all documents and nodes.
//   • DocumentIds remain valid until closeDocument() is called.
//   • NodeIds remain valid until the node is deleted or its document is closed.
//   • NullDocument / NullNode signal failure; there is nothing to release.
//   • setShape stores the ShapeId by value; the backend does NOT take
//     ownership of the geometry — that stays with IGeometryBackend.

class IPersistenceBackend {
public:
    virtual ~IPersistenceBackend() = default;

    // ── Document lifecycle ────────────────────────────────────────────────────

    // Creates a new, empty in-memory document with the given display name.
    virtual DocumentId newDocument(std::string_view name)      = 0;

    // Opens a document from disk.  Returns NullDocument on failure.
    virtual DocumentId openDocument(std::string_view path)     = 0;

    // Saves the document to its last-used path.  Returns false on failure.
    virtual bool saveDocument(DocumentId doc)                  = 0;

    // Saves the document to an explicit path (save-as).
    virtual bool saveDocumentAs(DocumentId doc, std::string_view path) = 0;

    // Closes the document and invalidates all NodeIds that belong to it.
    virtual void closeDocument(DocumentId doc)                 = 0;

    // Returns true when the document has unsaved modifications.
    virtual bool isModified(DocumentId doc) const              = 0;

    // ── Node (label) tree ─────────────────────────────────────────────────────

    // Returns the root node of the document.
    virtual NodeId rootNode(DocumentId doc)                    = 0;

    // Creates and returns a new child node under parent.
    virtual NodeId newChild(NodeId parent)                     = 0;

    // Removes node and all its descendants from the tree.
    virtual void deleteNode(NodeId node)                       = 0;

    // Moves node under newParent.  Returns false if the move would create
    // a cycle or if either handle is invalid.
    virtual bool reparent(NodeId node, NodeId newParent)       = 0;

    // Returns the immediate children of node, in insertion order.
    virtual std::vector<NodeId> children(NodeId node) const    = 0;

    // Returns the parent of node, or NullNode for the root.
    virtual NodeId parent(NodeId node) const                   = 0;

    // ── Named attributes ──────────────────────────────────────────────────────
    // Attributes are keyed by an arbitrary string.  Typed getters return
    // std::nullopt when the key is absent or has a different type.

    virtual void        setName  (NodeId node, std::string_view value)   = 0;
    virtual std::string getName  (NodeId node) const                     = 0;

    virtual void                  setReal  (NodeId node, std::string_view key, double value)      = 0;
    virtual std::optional<double> getReal  (NodeId node, std::string_view key) const              = 0;

    virtual void                   setInt  (NodeId node, std::string_view key, int64_t value)    = 0;
    virtual std::optional<int64_t> getInt  (NodeId node, std::string_view key) const             = 0;

    virtual void               setBool  (NodeId node, std::string_view key, bool value)          = 0;
    virtual std::optional<bool>getBool  (NodeId node, std::string_view key) const                = 0;

    virtual void                    setString(NodeId node, std::string_view key, std::string_view value) = 0;
    virtual std::optional<std::string> getString(NodeId node, std::string_view key) const               = 0;

    // Removes the named attribute.  Returns false if the key did not exist.
    virtual bool removeAttribute(NodeId node, std::string_view key)      = 0;

    // ── Geometry reference ────────────────────────────────────────────────────
    // Associates a B-rep shape (owned by IGeometryBackend) with this node.
    // The backend stores the handle only; it does not manage geometry lifetime.

    virtual void    setShape(NodeId node, ShapeId shape)       = 0;
    virtual ShapeId getShape(NodeId node) const                = 0;  // NullShape if none

    // ── Assembly transform ────────────────────────────────────────────────────
    // Local placement of this node relative to its parent (identity by default).

    virtual void      setTransform(NodeId node, const Transform& t) = 0;
    virtual Transform getTransform(NodeId node) const               = 0;

    // ── Undo / Redo ───────────────────────────────────────────────────────────
    // All mutations between beginTransaction / commitTransaction are grouped
    // into a single undo step.  abortTransaction rolls back the open group.
    // Nesting is supported: commit/abort must balance every begin.

    virtual void beginTransaction(DocumentId doc, std::string_view label = "") = 0;
    virtual bool commitTransaction(DocumentId doc) = 0;  // false if no open transaction
    virtual void abortTransaction (DocumentId doc) = 0;

    virtual bool canUndo(DocumentId doc) const = 0;
    virtual bool canRedo(DocumentId doc) const = 0;
    virtual void undo   (DocumentId doc)       = 0;
    virtual void redo   (DocumentId doc)       = 0;

    // Maximum undo steps to retain.  Pass 0 for unlimited.
    virtual void setUndoLimit(DocumentId doc, uint32_t steps) = 0;
};

}  // namespace CellForge
