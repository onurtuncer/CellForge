// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

#include "CellForge/Project.h"

namespace CellForge {

// ─── Attribute key / item-type constants ─────────────────────────────────────
namespace {
    constexpr std::string_view kType        = "cf.type";
    constexpr std::string_view kUrdfPath    = "cf.urdf_path";
    constexpr std::string_view kSrdfPath    = "cf.srdf_path";
    constexpr std::string_view kGeomPath    = "cf.geometry_path";
    constexpr std::string_view kRobotNodeId = "cf.robot_node_id";
    constexpr std::string_view kOpType      = "cf.operation_type";

    constexpr std::string_view kTProject    = "Project";
    constexpr std::string_view kTContainer  = "Container";
    constexpr std::string_view kTRobot      = "Robot";
    constexpr std::string_view kTWorkpiece  = "Workpiece";
    constexpr std::string_view kTFixture    = "Fixture";
    constexpr std::string_view kTProgram    = "Program";
    constexpr std::string_view kTOperation  = "Operation";
} // namespace

// ─── Lifecycle ────────────────────────────────────────────────────────────────

Project::Project(IPersistenceBackend& persistence)
    : m_persistence(persistence)
{}

Project::~Project() { close(); }

void Project::newProject(std::string_view name) {
    close();
    m_document = m_persistence.newDocument(name);
    buildTree(name);
}

bool Project::open(std::string_view path) {
    close();
    m_document = m_persistence.openDocument(path);
    if (m_document == NullDocument)
        return false;
    m_path = std::string(path);
    m_root = m_persistence.rootNode(m_document);
    discoverContainerNodes();
    return true;
}

bool Project::save() {
    if (m_document == NullDocument) return false;
    return m_persistence.saveDocument(m_document);
}

bool Project::saveAs(std::string_view path) {
    if (m_document == NullDocument) return false;
    if (!m_persistence.saveDocumentAs(m_document, path)) return false;
    m_path = std::string(path);
    return true;
}

void Project::close() {
    if (m_document == NullDocument) return;
    m_persistence.closeDocument(m_document);
    m_document        = NullDocument;
    m_path            = {};
    m_root            = NullNode;
    m_robotsNode      = NullNode;
    m_workpiecesNode  = NullNode;
    m_fixturesNode    = NullNode;
    m_programsNode    = NullNode;
}

bool        Project::isOpen()     const { return m_document != NullDocument; }
bool        Project::isModified() const { return m_document != NullDocument && m_persistence.isModified(m_document); }
std::string Project::name()       const { return m_root != NullNode ? m_persistence.getName(m_root) : std::string{}; }
const std::string& Project::path() const { return m_path; }

// ─── Tree root accessors ──────────────────────────────────────────────────────

NodeId Project::rootNode()       const { return m_root; }
NodeId Project::robotsNode()     const { return m_robotsNode; }
NodeId Project::workpiecesNode() const { return m_workpiecesNode; }
NodeId Project::fixturesNode()   const { return m_fixturesNode; }
NodeId Project::programsNode()   const { return m_programsNode; }

// ─── Robots ───────────────────────────────────────────────────────────────────

NodeId Project::addRobot(std::string_view name, std::string_view urdfPath, std::string_view srdfPath) {
    m_persistence.beginTransaction(m_document, "Add Robot");
    NodeId node = m_persistence.newChild(m_robotsNode);
    m_persistence.setName(node, name);
    m_persistence.setString(node, kType, kTRobot);
    m_persistence.setString(node, kUrdfPath, urdfPath);
    if (!srdfPath.empty())
        m_persistence.setString(node, kSrdfPath, srdfPath);
    m_persistence.commitTransaction(m_document);
    return node;
}

void Project::removeRobot(NodeId id) {
    m_persistence.beginTransaction(m_document, "Remove Robot");
    m_persistence.deleteNode(id);
    m_persistence.commitTransaction(m_document);
}

std::vector<NodeId> Project::robots() const {
    return m_persistence.children(m_robotsNode);
}

std::string Project::robotUrdfPath(NodeId id) const {
    auto v = m_persistence.getString(id, kUrdfPath);
    return v ? *v : std::string{};
}

std::string Project::robotSrdfPath(NodeId id) const {
    auto v = m_persistence.getString(id, kSrdfPath);
    return v ? *v : std::string{};
}

// ─── Workpieces ───────────────────────────────────────────────────────────────

NodeId Project::addWorkpiece(std::string_view name, std::string_view geometryPath) {
    m_persistence.beginTransaction(m_document, "Add Workpiece");
    NodeId node = m_persistence.newChild(m_workpiecesNode);
    m_persistence.setName(node, name);
    m_persistence.setString(node, kType, kTWorkpiece);
    m_persistence.setString(node, kGeomPath, geometryPath);
    m_persistence.commitTransaction(m_document);
    return node;
}

void Project::removeWorkpiece(NodeId id) {
    m_persistence.beginTransaction(m_document, "Remove Workpiece");
    m_persistence.deleteNode(id);
    m_persistence.commitTransaction(m_document);
}

std::vector<NodeId> Project::workpieces() const {
    return m_persistence.children(m_workpiecesNode);
}

// ─── Fixtures ─────────────────────────────────────────────────────────────────

NodeId Project::addFixture(std::string_view name, std::string_view geometryPath) {
    m_persistence.beginTransaction(m_document, "Add Fixture");
    NodeId node = m_persistence.newChild(m_fixturesNode);
    m_persistence.setName(node, name);
    m_persistence.setString(node, kType, kTFixture);
    m_persistence.setString(node, kGeomPath, geometryPath);
    m_persistence.commitTransaction(m_document);
    return node;
}

void Project::removeFixture(NodeId id) {
    m_persistence.beginTransaction(m_document, "Remove Fixture");
    m_persistence.deleteNode(id);
    m_persistence.commitTransaction(m_document);
}

std::vector<NodeId> Project::fixtures() const {
    return m_persistence.children(m_fixturesNode);
}

// ─── Programs ─────────────────────────────────────────────────────────────────

NodeId Project::addProgram(std::string_view name, NodeId robotId) {
    m_persistence.beginTransaction(m_document, "Add Program");
    NodeId node = m_persistence.newChild(m_programsNode);
    m_persistence.setName(node, name);
    m_persistence.setString(node, kType, kTProgram);
    m_persistence.setInt(node, kRobotNodeId, static_cast<int64_t>(robotId));
    m_persistence.commitTransaction(m_document);
    return node;
}

void Project::removeProgram(NodeId id) {
    m_persistence.beginTransaction(m_document, "Remove Program");
    m_persistence.deleteNode(id);
    m_persistence.commitTransaction(m_document);
}

std::vector<NodeId> Project::programs() const {
    return m_persistence.children(m_programsNode);
}

NodeId Project::robotForProgram(NodeId programId) const {
    auto id = m_persistence.getInt(programId, kRobotNodeId);
    return id ? static_cast<NodeId>(*id) : NullNode;
}

// ─── Operations ───────────────────────────────────────────────────────────────

NodeId Project::addOperation(NodeId programId, std::string_view name, std::string_view operationType) {
    m_persistence.beginTransaction(m_document, "Add Operation");
    NodeId node = m_persistence.newChild(programId);
    m_persistence.setName(node, name);
    m_persistence.setString(node, kType, kTOperation);
    m_persistence.setString(node, kOpType, operationType);
    m_persistence.commitTransaction(m_document);
    return node;
}

void Project::removeOperation(NodeId id) {
    m_persistence.beginTransaction(m_document, "Remove Operation");
    m_persistence.deleteNode(id);
    m_persistence.commitTransaction(m_document);
}

std::vector<NodeId> Project::operations(NodeId programId) const {
    return m_persistence.children(programId);
}

std::string Project::operationType(NodeId id) const {
    auto v = m_persistence.getString(id, kOpType);
    return v ? *v : std::string{};
}

// ─── Generic item access ──────────────────────────────────────────────────────

std::string Project::itemName(NodeId id) const {
    return m_persistence.getName(id);
}

std::string Project::itemType(NodeId id) const {
    auto t = m_persistence.getString(id, kType);
    return t ? *t : std::string{};
}

Transform Project::itemTransform(NodeId id) const {
    return m_persistence.getTransform(id);
}

void Project::setItemTransform(NodeId id, const Transform& t) {
    m_persistence.beginTransaction(m_document, "Set Transform");
    m_persistence.setTransform(id, t);
    m_persistence.commitTransaction(m_document);
}

void Project::renameItem(NodeId id, std::string_view name) {
    m_persistence.beginTransaction(m_document, "Rename");
    m_persistence.setName(id, name);
    m_persistence.commitTransaction(m_document);
}

void Project::setProperty(NodeId id, std::string_view key, std::string_view value) {
    m_persistence.beginTransaction(m_document, "Set Property");
    m_persistence.setString(id, key, value);
    m_persistence.commitTransaction(m_document);
}

std::optional<std::string> Project::getProperty(NodeId id, std::string_view key) const {
    return m_persistence.getString(id, key);
}

// ─── Undo / Redo ──────────────────────────────────────────────────────────────

void Project::undo() { if (m_document != NullDocument) m_persistence.undo(m_document); }
void Project::redo() { if (m_document != NullDocument) m_persistence.redo(m_document); }
bool Project::canUndo() const { return m_document != NullDocument && m_persistence.canUndo(m_document); }
bool Project::canRedo() const { return m_document != NullDocument && m_persistence.canRedo(m_document); }

// ─── Private helpers ──────────────────────────────────────────────────────────

void Project::buildTree(std::string_view projectName) {
    m_root = m_persistence.rootNode(m_document);
    m_persistence.setName(m_root, projectName);
    m_persistence.setString(m_root, kType, kTProject);

    auto makeContainer = [&](std::string_view containerName) -> NodeId {
        NodeId n = m_persistence.newChild(m_root);
        m_persistence.setName(n, containerName);
        m_persistence.setString(n, kType, kTContainer);
        return n;
    };

    m_robotsNode     = makeContainer("Robots");
    m_workpiecesNode = makeContainer("Workpieces");
    m_fixturesNode   = makeContainer("Fixtures");
    m_programsNode   = makeContainer("Programs");
}

void Project::discoverContainerNodes() {
    m_robotsNode     = findChildByName(m_root, "Robots");
    m_workpiecesNode = findChildByName(m_root, "Workpieces");
    m_fixturesNode   = findChildByName(m_root, "Fixtures");
    m_programsNode   = findChildByName(m_root, "Programs");
}

NodeId Project::findChildByName(NodeId parent, std::string_view name) const {
    for (NodeId child : m_persistence.children(parent)) {
        if (m_persistence.getName(child) == name)
            return child;
    }
    return NullNode;
}

} // namespace CellForge
