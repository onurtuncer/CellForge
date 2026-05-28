// ------------------------------------------------------------------------------------
// Project: CellForge
// Copyright (C) 2026, Melina Aero Teknoloji Gelistirme ve Dizayn Burosu A.S., Istanbul
// Author: Onur Tuncer, PhD
//
// SPDX-License-Identifier: LGPL-2.1-only
// License-Filename: LICENSE
// ------------------------------------------------------------------------------------

// This is the ONLY translation unit in CellForge that includes OCAF headers.
// All OCCT types are fully encapsulated here behind the Impl struct.

#include "OcafPersistenceBackend.h"

// ── OCAF framework ────────────────────────────────────────────────────────────
#include <BinDrivers.hxx>
#include <Message_ProgressRange.hxx>
#include <PCDM_ReaderStatus.hxx>
#include <PCDM_StoreStatus.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Name.hxx>
#include <TDataStd_NamedData.hxx>
#include <TDataStd_RealArray.hxx>
#include <TDF_Attribute.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_CopyLabel.hxx>
#include <TDF_Label.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDocStd_Application.hxx>
#include <TDocStd_Document.hxx>

// ── STL ───────────────────────────────────────────────────────────────────────
#include <map>
#include <string>
#include <unordered_map>

namespace CellForge {

// ─── CfShapeIdAttr ────────────────────────────────────────────────────────────
// Custom TDF_Attribute that stores a uint64_t shape handle.
// OCAF's TDataStd_Integer is only 32-bit; this custom attribute avoids splitting.

class CfShapeIdAttr : public TDF_Attribute {
    DEFINE_STANDARD_RTTI_INLINE(CfShapeIdAttr, TDF_Attribute)
public:
    static const Standard_GUID& GetID() {
        static const Standard_GUID kGuid("cf0a5e17-3b2c-4d8e-9f10-a1b2c3d4e5f6");
        return kGuid;
    }
    const Standard_GUID& ID() const override { return GetID(); }

    static Handle(CfShapeIdAttr) Set(const TDF_Label& label, uint64_t value) {
        Handle(CfShapeIdAttr) a;
        if (!label.FindAttribute(GetID(), a)) {
            a = new CfShapeIdAttr();
            label.AddAttribute(a);
        }
        a->Backup();
        a->m_value = value;
        return a;
    }

    static bool Find(const TDF_Label& label, uint64_t& out) {
        Handle(CfShapeIdAttr) a;
        if (!label.FindAttribute(GetID(), a)) return false;
        out = a->m_value;
        return true;
    }

    Handle(TDF_Attribute) NewEmpty() const override { return new CfShapeIdAttr(); }

    void Restore(const Handle(TDF_Attribute)& src) override {
        m_value = Handle(CfShapeIdAttr)::DownCast(src)->m_value;
    }

    void Paste(const Handle(TDF_Attribute)& dst,
               const Handle(TDF_RelocationTable)&) const override {
        Handle(CfShapeIdAttr)::DownCast(dst)->m_value = m_value;
    }

private:
    uint64_t m_value = 0;
};

} // namespace CellForge

// Must be at file scope, outside any namespace.
IMPLEMENT_STANDARD_RTTIEXT(CellForge::CfShapeIdAttr, TDF_Attribute)

namespace CellForge {

// ─── TDF_Label comparator ─────────────────────────────────────────────────────

struct LabelLess {
    bool operator()(const TDF_Label& a, const TDF_Label& b) const {
        TCollection_AsciiString ea, eb;
        a.EntryDumpToString(ea);
        b.EntryDumpToString(eb);
        return ea < eb;
    }
};

// ─── NamedData key helpers ────────────────────────────────────────────────────
// Each user attribute type gets a single-character prefix in the NamedData key
// so typed getters can distinguish which slot to look in.
// "r:" real  "i:" int64 (as decimal string)  "b:" bool (0/1 int)  "s:" string

static TCollection_ExtendedString rKey(std::string_view k) {
    return TCollection_ExtendedString(("r:" + std::string(k)).c_str());
}
static TCollection_ExtendedString iKey(std::string_view k) {
    return TCollection_ExtendedString(("i:" + std::string(k)).c_str());
}
static TCollection_ExtendedString bKey(std::string_view k) {
    return TCollection_ExtendedString(("b:" + std::string(k)).c_str());
}
static TCollection_ExtendedString sKey(std::string_view k) {
    return TCollection_ExtendedString(("s:" + std::string(k)).c_str());
}

static TCollection_ExtendedString toExt(std::string_view sv) {
    return TCollection_ExtendedString(sv.data(), static_cast<int>(sv.size()), Standard_True);
}

// ─── Impl ─────────────────────────────────────────────────────────────────────

struct OcafPersistenceBackend::Impl {
    Handle(TDocStd_Application) app;

    uint64_t nextDocId  = 1;
    uint64_t nextNodeId = 1;

    std::unordered_map<DocumentId, Handle(TDocStd_Document)> docs;
    std::unordered_map<DocumentId, std::string>              paths;
    std::unordered_map<DocumentId, NodeId>                   roots;
    std::unordered_map<DocumentId, int>                      txDepth;

    std::unordered_map<NodeId, TDF_Label>       labelById;
    std::map<TDF_Label, NodeId, LabelLess>      idByLabel;

    Impl() {
        app = new TDocStd_Application();
        BinDrivers::DefineFormat(app);
    }

    // ── ID allocation ─────────────────────────────────────────────────────────

    DocumentId allocDoc()  { return nextDocId++;  }
    NodeId     allocNode() { return nextNodeId++; }

    // ── Label / NodeId registry ───────────────────────────────────────────────

    NodeId registerLabel(const TDF_Label& label) {
        auto it = idByLabel.find(label);
        if (it != idByLabel.end()) return it->second;
        NodeId id = allocNode();
        labelById[id]    = label;
        idByLabel[label] = id;
        return id;
    }

    void unregisterLabel(const TDF_Label& label) {
        auto it = idByLabel.find(label);
        if (it == idByLabel.end()) return;
        labelById.erase(it->second);
        idByLabel.erase(it);
    }

    void unregisterSubtree(const TDF_Label& label) {
        unregisterLabel(label);
        for (TDF_ChildIterator it(label, false); it.More(); it.Next())
            unregisterSubtree(it.Value());
    }

    void registerSubtree(const TDF_Label& label) {
        registerLabel(label);
        for (TDF_ChildIterator it(label, false); it.More(); it.Next())
            registerSubtree(it.Value());
    }

    // Remaps NodeIds from old subtree labels to their copies according to
    // the relocation table produced by TDF_CopyLabel::Translate.
    void reregisterSubtree(const TDF_Label& oldRoot,
                            const Handle(TDF_RelocationTable)& reloc) {
        TDF_Label newLabel;
        if (reloc->HasRelocation(oldRoot, newLabel)) {
            auto it = idByLabel.find(oldRoot);
            if (it != idByLabel.end()) {
                NodeId id = it->second;
                idByLabel.erase(it);
                labelById[id]     = newLabel;
                idByLabel[newLabel] = id;
            }
        }
        for (TDF_ChildIterator it(oldRoot, false); it.More(); it.Next())
            reregisterSubtree(it.Value(), reloc);
    }

    // ── Lookup helpers (non-const and const overloads) ────────────────────────

    TDF_Label* labelFor(NodeId id) {
        auto it = labelById.find(id);
        return it != labelById.end() ? &it->second : nullptr;
    }

    const TDF_Label* labelFor(NodeId id) const {
        auto it = labelById.find(id);
        return it != labelById.end() ? &it->second : nullptr;
    }

    Handle(TDocStd_Document) docFor(DocumentId id) const {
        auto it = docs.find(id);
        return it != docs.end() ? it->second : nullptr;
    }

    // ── NamedData factory helper ──────────────────────────────────────────────

    static Handle(TDataStd_NamedData) getOrCreateND(const TDF_Label& label) {
        Handle(TDataStd_NamedData) nd;
        if (!label.FindAttribute(TDataStd_NamedData::GetID(), nd)) {
            nd = new TDataStd_NamedData();
            label.AddAttribute(nd);
        }
        return nd;
    }
};

// ─── OcafPersistenceBackend ───────────────────────────────────────────────────

OcafPersistenceBackend::OcafPersistenceBackend()
    : m_impl(std::make_unique<Impl>()) {}

OcafPersistenceBackend::~OcafPersistenceBackend() = default;

// ── Document lifecycle ────────────────────────────────────────────────────────

DocumentId OcafPersistenceBackend::newDocument(std::string_view name)
{
    Handle(TDocStd_Document) doc;
    m_impl->app->NewDocument("BinOcaf", doc);
    if (doc.IsNull()) return NullDocument;

    doc->SetUndoLimit(50);

    DocumentId id = m_impl->allocDoc();
    m_impl->docs[id] = doc;

    TDF_Label main = doc->Main();
    NodeId rootId  = m_impl->registerLabel(main);
    m_impl->roots[id] = rootId;

    TDataStd_Name::Set(main, toExt(name));
    return id;
}

DocumentId OcafPersistenceBackend::openDocument(std::string_view path)
{
    Handle(TDocStd_Document) doc;
    Message_ProgressRange progress;
    PCDM_ReaderStatus status =
        m_impl->app->Open(toExt(path), doc, progress);
    if (status != PCDM_RS_OK || doc.IsNull()) return NullDocument;

    DocumentId id = m_impl->allocDoc();
    m_impl->docs[id]  = doc;
    m_impl->paths[id] = std::string(path);

    m_impl->registerSubtree(doc->Main());
    m_impl->roots[id] = m_impl->idByLabel.at(doc->Main());
    return id;
}

bool OcafPersistenceBackend::saveDocument(DocumentId docId)
{
    auto it = m_impl->paths.find(docId);
    if (it == m_impl->paths.end()) return false;
    return saveDocumentAs(docId, it->second);
}

bool OcafPersistenceBackend::saveDocumentAs(DocumentId docId, std::string_view path)
{
    Handle(TDocStd_Document) doc = m_impl->docFor(docId);
    if (doc.IsNull()) return false;

    Message_ProgressRange progress;
    PCDM_StoreStatus status =
        m_impl->app->SaveAs(doc, toExt(path), progress);
    if (status == PCDM_SS_OK)
        m_impl->paths[docId] = std::string(path);
    return status == PCDM_SS_OK;
}

void OcafPersistenceBackend::closeDocument(DocumentId docId)
{
    Handle(TDocStd_Document) doc = m_impl->docFor(docId);
    if (doc.IsNull()) return;

    m_impl->unregisterSubtree(doc->Main());
    m_impl->app->Close(doc);
    m_impl->docs.erase(docId);
    m_impl->paths.erase(docId);
    m_impl->roots.erase(docId);
    m_impl->txDepth.erase(docId);
}

bool OcafPersistenceBackend::isModified(DocumentId docId) const
{
    Handle(TDocStd_Document) doc = m_impl->docFor(docId);
    return !doc.IsNull() && doc->IsModified();
}

// ── Node tree ─────────────────────────────────────────────────────────────────

NodeId OcafPersistenceBackend::rootNode(DocumentId docId)
{
    auto it = m_impl->roots.find(docId);
    return it != m_impl->roots.end() ? it->second : NullNode;
}

NodeId OcafPersistenceBackend::newChild(NodeId parentId)
{
    TDF_Label* pl = m_impl->labelFor(parentId);
    if (!pl) return NullNode;
    return m_impl->registerLabel(pl->NewChild());
}

void OcafPersistenceBackend::deleteNode(NodeId nodeId)
{
    TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return;
    // Unregister first so children() skips this subtree.
    m_impl->unregisterSubtree(*lptr);
    // OCAF cannot physically delete labels; clear all attributes recursively.
    lptr->ForgetAllAttributes(Standard_True);
}

bool OcafPersistenceBackend::reparent(NodeId nodeId, NodeId newParentId)
{
    TDF_Label* src = m_impl->labelFor(nodeId);
    TDF_Label* dst = m_impl->labelFor(newParentId);
    if (!src || !dst) return false;

    // Cycle guard: newParent must not lie within node's subtree.
    for (TDF_Label check = *dst; !check.IsNull(); check = check.Father()) {
        if (check.IsEqual(*src)) return false;
    }

    // Copy the entire subtree to a new child under newParent.
    TDF_Label newLabel = dst->NewChild();
    Handle(TDF_RelocationTable) reloc = new TDF_RelocationTable(Standard_False);
    TDF_CopyLabel::Translate(*src, newLabel, reloc);

    // Transfer NodeId mappings from old labels to their copies.
    m_impl->reregisterSubtree(*src, reloc);

    // Clear the original (can't be physically removed from OCAF's label tree).
    src->ForgetAllAttributes(Standard_True);
    return true;
}

std::vector<NodeId> OcafPersistenceBackend::children(NodeId nodeId) const
{
    const TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return {};

    std::vector<NodeId> result;
    for (TDF_ChildIterator it(*lptr, false); it.More(); it.Next()) {
        // Skip labels that were cleared by deleteNode (not in registry).
        auto jt = m_impl->idByLabel.find(it.Value());
        if (jt != m_impl->idByLabel.end())
            result.push_back(jt->second);
    }
    return result;
}

NodeId OcafPersistenceBackend::parent(NodeId nodeId) const
{
    const TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return NullNode;
    TDF_Label p = lptr->Father();
    if (p.IsNull()) return NullNode;
    auto it = m_impl->idByLabel.find(p);
    return it != m_impl->idByLabel.end() ? it->second : NullNode;
}

// ── Named attributes ──────────────────────────────────────────────────────────

void OcafPersistenceBackend::setName(NodeId nodeId, std::string_view value)
{
    TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return;
    TDataStd_Name::Set(*lptr, toExt(value));
}

std::string OcafPersistenceBackend::getName(NodeId nodeId) const
{
    const TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return {};
    Handle(TDataStd_Name) attr;
    if (!lptr->FindAttribute(TDataStd_Name::GetID(), attr)) return {};
    return TCollection_AsciiString(attr->Get()).ToCString();
}

void OcafPersistenceBackend::setReal(NodeId nodeId, std::string_view key, double value)
{
    TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return;
    Impl::getOrCreateND(*lptr)->SetReal(rKey(key), value);
}

std::optional<double> OcafPersistenceBackend::getReal(NodeId nodeId, std::string_view key) const
{
    const TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return std::nullopt;
    Handle(TDataStd_NamedData) nd;
    if (!lptr->FindAttribute(TDataStd_NamedData::GetID(), nd)) return std::nullopt;
    auto k = rKey(key);
    if (!nd->HasReal(k)) return std::nullopt;
    return nd->GetReal(k);
}

void OcafPersistenceBackend::setInt(NodeId nodeId, std::string_view key, int64_t value)
{
    TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return;
    // OCAF integers are 32-bit; store int64 as a decimal ExtendedString.
    Impl::getOrCreateND(*lptr)->SetString(iKey(key),
        TCollection_ExtendedString(std::to_string(value).c_str()));
}

std::optional<int64_t> OcafPersistenceBackend::getInt(NodeId nodeId, std::string_view key) const
{
    const TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return std::nullopt;
    Handle(TDataStd_NamedData) nd;
    if (!lptr->FindAttribute(TDataStd_NamedData::GetID(), nd)) return std::nullopt;
    auto k = iKey(key);
    if (!nd->HasString(k)) return std::nullopt;
    return static_cast<int64_t>(
        std::stoll(TCollection_AsciiString(nd->GetString(k)).ToCString()));
}

void OcafPersistenceBackend::setBool(NodeId nodeId, std::string_view key, bool value)
{
    TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return;
    Impl::getOrCreateND(*lptr)->SetInteger(bKey(key), value ? 1 : 0);
}

std::optional<bool> OcafPersistenceBackend::getBool(NodeId nodeId, std::string_view key) const
{
    const TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return std::nullopt;
    Handle(TDataStd_NamedData) nd;
    if (!lptr->FindAttribute(TDataStd_NamedData::GetID(), nd)) return std::nullopt;
    auto k = bKey(key);
    if (!nd->HasInteger(k)) return std::nullopt;
    return nd->GetInteger(k) != 0;
}

void OcafPersistenceBackend::setString(NodeId nodeId, std::string_view key, std::string_view value)
{
    TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return;
    Impl::getOrCreateND(*lptr)->SetString(sKey(key), toExt(value));
}

std::optional<std::string> OcafPersistenceBackend::getString(NodeId nodeId, std::string_view key) const
{
    const TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return std::nullopt;
    Handle(TDataStd_NamedData) nd;
    if (!lptr->FindAttribute(TDataStd_NamedData::GetID(), nd)) return std::nullopt;
    auto k = sKey(key);
    if (!nd->HasString(k)) return std::nullopt;
    return std::string(TCollection_AsciiString(nd->GetString(k)).ToCString());
}

bool OcafPersistenceBackend::removeAttribute(NodeId nodeId, std::string_view key)
{
    TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return false;
    Handle(TDataStd_NamedData) nd;
    if (!lptr->FindAttribute(TDataStd_NamedData::GetID(), nd)) return false;

    nd->Backup();
    bool removed = false;
    // Each type uses a different slot in NamedData — check all four.
    removed |= static_cast<bool>(nd->ChangeReals().UnBind(rKey(key)));
    removed |= static_cast<bool>(nd->ChangeIntegers().UnBind(bKey(key)));
    removed |= static_cast<bool>(nd->ChangeStrings().UnBind(iKey(key)));
    removed |= static_cast<bool>(nd->ChangeStrings().UnBind(sKey(key)));
    return removed;
}

// ── Geometry reference ────────────────────────────────────────────────────────

void OcafPersistenceBackend::setShape(NodeId nodeId, ShapeId shape)
{
    TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return;
    CfShapeIdAttr::Set(*lptr, shape);
}

ShapeId OcafPersistenceBackend::getShape(NodeId nodeId) const
{
    const TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return NullShape;
    uint64_t id = 0;
    CfShapeIdAttr::Find(*lptr, id);
    return id;
}

// ── Assembly transform ────────────────────────────────────────────────────────

void OcafPersistenceBackend::setTransform(NodeId nodeId, const Transform& t)
{
    TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return;

    Handle(TDataStd_RealArray) ra;
    if (!lptr->FindAttribute(TDataStd_RealArray::GetID(), ra))
        ra = TDataStd_RealArray::Set(*lptr, 0, 15);

    for (int i = 0; i < 16; ++i)
        ra->SetValue(i, t.data[i]);
}

Transform OcafPersistenceBackend::getTransform(NodeId nodeId) const
{
    const TDF_Label* lptr = m_impl->labelFor(nodeId);
    if (!lptr) return Transform{};

    Handle(TDataStd_RealArray) ra;
    if (!lptr->FindAttribute(TDataStd_RealArray::GetID(), ra)) return Transform{};

    Transform t;
    for (int i = 0; i < 16; ++i)
        t.data[i] = ra->Value(i);
    return t;
}

// ── Undo / Redo ───────────────────────────────────────────────────────────────

void OcafPersistenceBackend::beginTransaction(DocumentId docId, std::string_view /*label*/)
{
    Handle(TDocStd_Document) doc = m_impl->docFor(docId);
    if (doc.IsNull()) return;
    int& depth = m_impl->txDepth[docId];
    if (depth == 0)
        doc->NewCommand();
    ++depth;
}

bool OcafPersistenceBackend::commitTransaction(DocumentId docId)
{
    Handle(TDocStd_Document) doc = m_impl->docFor(docId);
    if (doc.IsNull()) return false;
    int& depth = m_impl->txDepth[docId];
    if (depth <= 0) return false;
    if (--depth == 0)
        doc->CommitCommand();
    return true;
}

void OcafPersistenceBackend::abortTransaction(DocumentId docId)
{
    Handle(TDocStd_Document) doc = m_impl->docFor(docId);
    if (doc.IsNull()) return;
    int& depth = m_impl->txDepth[docId];
    if (depth > 0) {
        doc->AbortCommand();
        depth = 0;
    }
}

bool OcafPersistenceBackend::canUndo(DocumentId docId) const
{
    Handle(TDocStd_Document) doc = m_impl->docFor(docId);
    return !doc.IsNull() && doc->GetAvailableUndos() > 0;
}

bool OcafPersistenceBackend::canRedo(DocumentId docId) const
{
    Handle(TDocStd_Document) doc = m_impl->docFor(docId);
    return !doc.IsNull() && doc->GetAvailableRedos() > 0;
}

void OcafPersistenceBackend::undo(DocumentId docId)
{
    Handle(TDocStd_Document) doc = m_impl->docFor(docId);
    if (!doc.IsNull()) doc->Undo();
}

void OcafPersistenceBackend::redo(DocumentId docId)
{
    Handle(TDocStd_Document) doc = m_impl->docFor(docId);
    if (!doc.IsNull()) doc->Redo();
}

void OcafPersistenceBackend::setUndoLimit(DocumentId docId, uint32_t steps)
{
    Handle(TDocStd_Document) doc = m_impl->docFor(docId);
    if (!doc.IsNull())
        doc->SetUndoLimit(static_cast<int>(steps));
}

} // namespace CellForge
