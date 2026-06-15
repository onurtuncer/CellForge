// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#pragma once

#include "CellForge/IPersistanceBackend.h"
#include "CellForge/Mathematics.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace CellForge {

// ─── Project ──────────────────────────────────────────────────────────────────
// Manages the offline robot programming project tree on top of any
// IPersistenceBackend.  The tree is structured as:
//
//   root  (Project)
//   ├── Robots
//   │   └── <robot>   cf.urdf_path, cf.srdf_path, transform
//   ├── Workpieces
//   │   └── <part>    cf.geometry_path, transform
//   ├── Fixtures
//   │   └── <fixture> cf.geometry_path, transform
//   └── Programs
//       └── <program> cf.robot_node_id
//           └── <operation>  cf.operation_type
//
// Every mutation is wrapped in a named transaction so undo/redo works out of
// the box.  The project does NOT depend on OcafPersistenceBackend directly —
// any IPersistenceBackend implementation works.

class Project {
public:
    explicit Project(IPersistenceBackend& persistence);
    ~Project();

    Project(const Project&) = delete;
    Project& operator=(const Project&) = delete;

    // ── Lifecycle ──────────────────────────────────────────────────────────────
    void newProject(std::string_view name);
    bool open(std::string_view path);
    bool save();
    bool saveAs(std::string_view path);
    void close();

    bool        isOpen()     const;
    bool        isModified() const;
    std::string name()       const;
    const std::string& path() const;

    // ── Tree root ──────────────────────────────────────────────────────────────
    NodeId rootNode()      const;
    NodeId robotsNode()    const;
    NodeId workpiecesNode() const;
    NodeId fixturesNode()  const;
    NodeId programsNode()  const;

    // ── Robots ─────────────────────────────────────────────────────────────────
    NodeId              addRobot(std::string_view name,
                                 std::string_view urdfPath,
                                 std::string_view srdfPath = {});
    void                removeRobot(NodeId id);
    std::vector<NodeId> robots() const;
    std::string         robotUrdfPath(NodeId id) const;
    std::string         robotSrdfPath(NodeId id) const;

    // ── Workpieces ─────────────────────────────────────────────────────────────
    NodeId              addWorkpiece(std::string_view name, std::string_view geometryPath);
    void                removeWorkpiece(NodeId id);
    std::vector<NodeId> workpieces() const;

    // ── Fixtures ───────────────────────────────────────────────────────────────
    NodeId              addFixture(std::string_view name, std::string_view geometryPath);
    void                removeFixture(NodeId id);
    std::vector<NodeId> fixtures() const;

    // ── Programs ───────────────────────────────────────────────────────────────
    NodeId              addProgram(std::string_view name, NodeId robotId);
    void                removeProgram(NodeId id);
    std::vector<NodeId> programs() const;
    NodeId              robotForProgram(NodeId programId) const;

    // ── Operations ─────────────────────────────────────────────────────────────
    // Operations are children of a Program node and carry a typed operation
    // string (e.g. "MoveJ", "MoveL", "Wait", "SetIO").  Additional parameters
    // can be stored via setProperty / getProperty.
    NodeId              addOperation(NodeId programId,
                                     std::string_view name,
                                     std::string_view operationType);
    void                removeOperation(NodeId id);
    std::vector<NodeId> operations(NodeId programId) const;
    std::string         operationType(NodeId id) const;

    // ── Generic item access ────────────────────────────────────────────────────
    std::string itemName(NodeId id)      const;
    std::string itemType(NodeId id)      const; // "Robot", "Workpiece", …
    Transform   itemTransform(NodeId id) const;
    void setItemTransform(NodeId id, const Transform& t);
    void renameItem(NodeId id, std::string_view name);

    // Open-ended string attributes for item-specific data (operation waypoints,
    // joint configs, I/O identifiers, …).  Key namespace is caller's choice.
    void                       setProperty(NodeId id, std::string_view key, std::string_view value);
    std::optional<std::string> getProperty(NodeId id, std::string_view key) const;

    // ── Undo / Redo ────────────────────────────────────────────────────────────
    void undo();
    void redo();
    bool canUndo() const;
    bool canRedo() const;

private:
    void   buildTree(std::string_view projectName);
    void   discoverContainerNodes();
    NodeId findChildByName(NodeId parent, std::string_view name) const;

    IPersistenceBackend& m_persistence;
    DocumentId  m_document        = NullDocument;
    std::string m_path;

    NodeId m_root           = NullNode;
    NodeId m_robotsNode     = NullNode;
    NodeId m_workpiecesNode = NullNode;
    NodeId m_fixturesNode   = NullNode;
    NodeId m_programsNode   = NullNode;
};

} // namespace CellForge
