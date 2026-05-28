// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

// No OCAF headers here — they live exclusively in OcafPersistenceBackend.cpp.

#include "CellForge/IPersistanceBackend.h"

#include <memory>

namespace CellForge {

// Concrete OCAF (Open CASCADE Application Framework) implementation of
// IPersistenceBackend.  The Pimpl idiom keeps all OCAF types out of this
// header so consumers of CellForge::persistence never need to see OCCT.
//
// Documents are saved in the binary OCAF format ("BinOcaf", extension .cbf).
// Nodes map 1-to-1 to TDF_Labels in the document's label tree.

class OcafPersistenceBackend : public IPersistenceBackend {
public:
    OcafPersistenceBackend();
    ~OcafPersistenceBackend() override;

    // non-copyable — each instance owns a TDocStd_Application
    OcafPersistenceBackend(const OcafPersistenceBackend&)            = delete;
    OcafPersistenceBackend& operator=(const OcafPersistenceBackend&) = delete;

    // ── Document lifecycle ────────────────────────────────────────────────────

    DocumentId newDocument   (std::string_view name)                        override;
    DocumentId openDocument  (std::string_view path)                        override;
    bool       saveDocument  (DocumentId doc)                               override;
    bool       saveDocumentAs(DocumentId doc, std::string_view path)        override;
    void       closeDocument (DocumentId doc)                               override;
    bool       isModified    (DocumentId doc) const                         override;

    // ── Node tree ─────────────────────────────────────────────────────────────

    NodeId               rootNode (DocumentId doc)                          override;
    NodeId               newChild (NodeId parent)                           override;
    void                 deleteNode(NodeId node)                            override;
    bool                 reparent  (NodeId node, NodeId newParent)          override;
    std::vector<NodeId>  children  (NodeId node) const                      override;
    NodeId               parent    (NodeId node) const                      override;

    // ── Named attributes ──────────────────────────────────────────────────────

    void        setName(NodeId node, std::string_view value)                override;
    std::string getName(NodeId node) const                                  override;

    void                  setReal(NodeId node, std::string_view key, double value)      override;
    std::optional<double> getReal(NodeId node, std::string_view key) const              override;

    void                   setInt(NodeId node, std::string_view key, int64_t value)    override;
    std::optional<int64_t> getInt(NodeId node, std::string_view key) const             override;

    void               setBool(NodeId node, std::string_view key, bool value)          override;
    std::optional<bool>getBool(NodeId node, std::string_view key) const                override;

    void                      setString(NodeId node, std::string_view key, std::string_view value) override;
    std::optional<std::string>getString(NodeId node, std::string_view key) const                   override;

    bool removeAttribute(NodeId node, std::string_view key)                 override;

    // ── Geometry reference ────────────────────────────────────────────────────

    void    setShape(NodeId node, ShapeId shape)                            override;
    ShapeId getShape(NodeId node) const                                     override;

    // ── Assembly transform ────────────────────────────────────────────────────

    void      setTransform(NodeId node, const Transform& t)                 override;
    Transform getTransform(NodeId node) const                               override;

    // ── Undo / Redo ───────────────────────────────────────────────────────────

    void beginTransaction (DocumentId doc, std::string_view label = "")     override;
    bool commitTransaction(DocumentId doc)                                  override;
    void abortTransaction (DocumentId doc)                                  override;

    bool canUndo(DocumentId doc) const                                      override;
    bool canRedo(DocumentId doc) const                                      override;
    void undo   (DocumentId doc)                                            override;
    void redo   (DocumentId doc)                                            override;

    void setUndoLimit(DocumentId doc, uint32_t steps)                       override;

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace CellForge
