#ifndef STUDIOSLAB_ONTOLOGY_GRAPH_V2_H
#define STUDIOSLAB_ONTOLOGY_GRAPH_V2_H

#include "3rdParty/imgui-node-editor/crude_json.h"

#include "Core/Model/V2/ModelTypesV2.h"
#include "Graphics/Types2D.h"
#include "Utils/Types.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <filesystem>
#include <functional>
#include <initializer_list>
#include <map>
#include <optional>
#include <set>
#include <sstream>

namespace Slab::Core::Ontology::V2 {

    namespace json = crude_json;
    namespace ModelV2 = Slab::Core::Model::V2;

    enum class EOntologyDocumentKindV2 : unsigned char {
        Schema,
        GlobalOntology,
        Study
    };

    enum class EOntologyOwnershipScopeV2 : unsigned char {
        Global,
        StudyLocal
    };

    enum class EOntologyElementKindV2 : unsigned char {
        None,
        Node,
        Edge
    };

    enum class EOntologyFilterScopeModeV2 : unsigned char {
        ShowAll,
        GlobalOnly,
        SelectedStudyOnly
    };

    enum class EOntologyActivationStatusV2 : unsigned char {
        None,
        Active,
        Reachable,
        Satisfied,
        Blocked,
        Unresolved,
        Partial
    };

    struct FOntologyDiagnosticV2 {
        ModelV2::EValidationSeverityV2 Severity = ModelV2::EValidationSeverityV2::Warning;
        Str Code;
        Str EntityId;
        Str Context;
        Str Message;
        Str SourceDocument;
    };

    struct FOntologyValidationResultV2 {
        Vector<FOntologyDiagnosticV2> Messages;

        [[nodiscard]] auto ErrorCount() const -> std::size_t {
            return static_cast<std::size_t>(std::count_if(
                Messages.begin(),
                Messages.end(),
                [](const auto &message) {
                    return message.Severity == ModelV2::EValidationSeverityV2::Error;
                }));
        }

        [[nodiscard]] auto WarningCount() const -> std::size_t {
            return static_cast<std::size_t>(std::count_if(
                Messages.begin(),
                Messages.end(),
                [](const auto &message) {
                    return message.Severity == ModelV2::EValidationSeverityV2::Warning;
                }));
        }

        [[nodiscard]] auto HasErrors() const -> bool {
            return ErrorCount() > 0;
        }

        auto Add(const ModelV2::EValidationSeverityV2 severity,
                 Str code,
                 Str entityId,
                 Str context,
                 Str message,
                 Str sourceDocument = {}) -> void {
            Messages.push_back(FOntologyDiagnosticV2{
                .Severity = severity,
                .Code = std::move(code),
                .EntityId = std::move(entityId),
                .Context = std::move(context),
                .Message = std::move(message),
                .SourceDocument = std::move(sourceDocument)
            });
        }
    };

    struct FOntologyDocumentMetaV2 {
        Str Format;
        Str Version;
        Str DocKind;
        Str Title;
        Str Description;
        json::object ExtraFields;
    };

    struct FOntologySchemaInfoV2 {
        StrVector NodeKinds;
        StrVector EdgeKinds;
        StrVector Layers;
        StrVector ProvenanceKinds;
        json::object ExtraFields;
    };

    struct FOntologyStudyInfoV2 {
        Str Id;
        Str Title;
        Str Description;
        Str BaseVocabulary;
        json::value NotationContext;
        json::object ExtraFields;
    };

    struct FOntologyReferenceV2 {
        Str Path;
        Str Version;
        json::object ExtraFields;
    };

    struct FOntologyNodeRecordV2 {
        Str Id;
        Str Kind;
        Str Title;
        Str Layer;
        bool bAbstract = false;
        json::object Properties;
        json::object ExtraFields;
        Str SourceDocument;
        EOntologyDocumentKindV2 SourceKind = EOntologyDocumentKindV2::GlobalOntology;
        EOntologyOwnershipScopeV2 OwnershipScope = EOntologyOwnershipScopeV2::Global;
    };

    struct FOntologyEdgeRecordV2 {
        Str Id;
        Str Type;
        Str From;
        Str To;
        json::object Properties;
        json::object ExtraFields;
        Str SourceDocument;
        EOntologyDocumentKindV2 SourceKind = EOntologyDocumentKindV2::GlobalOntology;
        EOntologyOwnershipScopeV2 OwnershipScope = EOntologyOwnershipScopeV2::Global;
        bool bFromResolved = false;
        bool bToResolved = false;
        bool bCrossScopeReference = false;
    };

    struct FOntologySchemaDocument {
        std::filesystem::path SourcePath;
        Str SourceDocument;
        Str SchemaId;
        Str Title;
        Str Description;
        StrVector NodeKinds;
        StrVector EdgeKinds;
        StrVector Layers;
        StrVector ProvenanceKinds;
        StrVector ActivationStatuses;
        json::value RawRoot;
        bool bLoaded = false;
    };

    struct FGlobalDescentOntologyDocument {
        std::filesystem::path SourcePath;
        Str SourceDocument;
        FOntologyDocumentMetaV2 Meta;
        FOntologySchemaInfoV2 SchemaInfo;
        Vector<FOntologyNodeRecordV2> Nodes;
        Vector<FOntologyEdgeRecordV2> Edges;
        json::object ExtraFields;
        json::value RawRoot;
        bool bLoaded = false;
    };

    struct FStudyOntologyDocument {
        std::filesystem::path SourcePath;
        Str SourceDocument;
        FOntologyDocumentMetaV2 Meta;
        FOntologyReferenceV2 OntologyRef;
        FOntologyStudyInfoV2 Study;
        Vector<FOntologyNodeRecordV2> Nodes;
        Vector<FOntologyEdgeRecordV2> Edges;
        json::value ActivationSummary;
        json::value RealizationFrontier;
        json::object ExtraFields;
        json::value RawRoot;
        bool bLoaded = false;
    };

    struct FOntologyGraphSelectionV2 {
        EOntologyElementKindV2 Kind = EOntologyElementKindV2::None;
        Str ElementId;

        [[nodiscard]] auto IsValid() const -> bool {
            return Kind != EOntologyElementKindV2::None && !ElementId.empty();
        }
    };

    struct FOntologyGraphFilterStateV2 {
        EOntologyFilterScopeModeV2 ScopeMode = EOntologyFilterScopeModeV2::ShowAll;
        bool bHideAmbientLayer = false;
        bool bHideAbstractNodes = false;
        bool bFocusActiveReachableRegion = false;
        bool bShowBlockedRequirementsOnly = false;
        bool bShowRealizationRecipeArtifactPathOnly = false;
        bool bShowEdgeLabels = false;
    };

    struct FOntologyProjectedNodeV2 {
        Str NodeId;
        Str Title;
        Str Kind;
        Str Layer;
        bool bAbstract = false;
        EOntologyOwnershipScopeV2 OwnershipScope = EOntologyOwnershipScopeV2::Global;
        Str SourceDocument;
        EOntologyActivationStatusV2 ActivationStatus = EOntologyActivationStatusV2::None;
        bool bStudyRoot = false;
        bool bStudyOwned = false;
        bool bInActiveRegion = false;
        bool bInRealizationPath = false;
        Str CompactId;
        Str Summary;
        Str SecondarySummary;
        Slab::Graphics::FPoint2D Position{};
    };

    struct FOntologyProjectedEdgeV2 {
        Str EdgeId;
        Str Type;
        Str FromNodeId;
        Str ToNodeId;
        Str SourceDocument;
        EOntologyOwnershipScopeV2 OwnershipScope = EOntologyOwnershipScopeV2::Global;
        EOntologyActivationStatusV2 ActivationStatus = EOntologyActivationStatusV2::None;
        bool bCrossScopeReference = false;
        bool bBrokenReference = false;
        bool bInActiveRegion = false;
        bool bInRealizationPath = false;
        bool bStudyOwned = false;
        Str BadgeLabel;
        Str Summary;
    };

    struct FOntologyGraphProjection {
        Str SelectedStudyId;
        Str SelectedStudyDocument;
        FOntologyGraphFilterStateV2 Filters;
        Vector<FOntologyProjectedNodeV2> Nodes;
        Vector<FOntologyProjectedEdgeV2> Edges;
        std::map<Str, Vector<Str>> OutgoingEdgesByNodeId;
        std::map<Str, Vector<Str>> IncomingEdgesByNodeId;
        FOntologyValidationResultV2 Diagnostics;

        [[nodiscard]] auto FindNode(const Str &nodeId) const -> const FOntologyProjectedNodeV2 * {
            const auto it = std::find_if(Nodes.begin(), Nodes.end(), [&](const auto &node) {
                return node.NodeId == nodeId;
            });
            return it == Nodes.end() ? nullptr : &(*it);
        }

        [[nodiscard]] auto FindEdge(const Str &edgeId) const -> const FOntologyProjectedEdgeV2 * {
            const auto it = std::find_if(Edges.begin(), Edges.end(), [&](const auto &edge) {
                return edge.EdgeId == edgeId;
            });
            return it == Edges.end() ? nullptr : &(*it);
        }
    };

    struct FOntologyGraphBundle {
        FOntologySchemaDocument Schema;
        FGlobalDescentOntologyDocument GlobalDocument;
        Vector<FStudyOntologyDocument> StudyDocuments;
        std::map<Str, FOntologyNodeRecordV2> NodeTable;
        std::map<Str, FOntologyEdgeRecordV2> EdgeTable;
        std::map<Str, Vector<Str>> OutgoingEdgesByNodeId;
        std::map<Str, Vector<Str>> IncomingEdgesByNodeId;
        FOntologyValidationResultV2 Diagnostics;

        [[nodiscard]] auto FindNode(const Str &nodeId) const -> const FOntologyNodeRecordV2 * {
            const auto it = NodeTable.find(nodeId);
            return it == NodeTable.end() ? nullptr : &it->second;
        }

        [[nodiscard]] auto FindEdge(const Str &edgeId) const -> const FOntologyEdgeRecordV2 * {
            const auto it = EdgeTable.find(edgeId);
            return it == EdgeTable.end() ? nullptr : &it->second;
        }

        [[nodiscard]] auto FindStudyDocumentById(const Str &studyId) const -> const FStudyOntologyDocument * {
            const auto it = std::find_if(StudyDocuments.begin(), StudyDocuments.end(), [&](const auto &study) {
                return study.Study.Id == studyId;
            });
            return it == StudyDocuments.end() ? nullptr : &(*it);
        }
    };

    inline auto ToString(const EOntologyDocumentKindV2 value) -> const char * {
        switch (value) {
            case EOntologyDocumentKindV2::Schema: return "Schema";
            case EOntologyDocumentKindV2::GlobalOntology: return "GlobalOntology";
            case EOntologyDocumentKindV2::Study: return "Study";
        }

        return "Unknown";
    }

    inline auto ToString(const EOntologyOwnershipScopeV2 value) -> const char * {
        switch (value) {
            case EOntologyOwnershipScopeV2::Global: return "Global";
            case EOntologyOwnershipScopeV2::StudyLocal: return "StudyLocal";
        }

        return "Unknown";
    }

    inline auto ToString(const EOntologyFilterScopeModeV2 value) -> const char * {
        switch (value) {
            case EOntologyFilterScopeModeV2::ShowAll: return "ShowAll";
            case EOntologyFilterScopeModeV2::GlobalOnly: return "GlobalOnly";
            case EOntologyFilterScopeModeV2::SelectedStudyOnly: return "SelectedStudyOnly";
        }

        return "Unknown";
    }

    inline auto ToString(const EOntologyActivationStatusV2 value) -> const char * {
        switch (value) {
            case EOntologyActivationStatusV2::None: return "none";
            case EOntologyActivationStatusV2::Active: return "active";
            case EOntologyActivationStatusV2::Reachable: return "reachable";
            case EOntologyActivationStatusV2::Satisfied: return "satisfied";
            case EOntologyActivationStatusV2::Blocked: return "blocked";
            case EOntologyActivationStatusV2::Unresolved: return "unresolved";
            case EOntologyActivationStatusV2::Partial: return "partial";
        }

        return "none";
    }

    inline auto FriendlyNodeCategoryLabelV2(const Str &kind, const Str &layer) -> Str {
        if (kind == "Requirement") return "requirement";
        if (kind == "Study" || kind == "StudyObject" || layer == "study") return "study";
        if (kind == "ArtifactClass" || layer == "artifact") return "artifact";
        if (kind == "SolverClass") return "solver";
        if (kind == "RecipeClass" || layer == "recipe") return "recipe";
        if (kind == "RealizationClass" || layer == "realization") return "realization";
        if (kind == "DescentClass" || layer == "descent") return "descent";
        if (layer == "ambient") return "ambient";
        if (kind == "SemanticClass" || layer == "semantic") return "semantic";
        return layer.empty() ? kind : layer;
    }

    namespace Detail {

        inline constexpr auto COntologyFormatV2 = "studioslab-ontology-graph";

        inline auto MakeDefaultActivationStatusesV2() -> StrVector {
            return {"active", "reachable", "satisfied", "blocked", "unresolved", "partial"};
        }

        inline auto MakeGlobalIdPrefixesV2() -> std::array<const char *, 8> {
            return {
                "ambient:",
                "semantic:",
                "descent:",
                "realization:",
                "recipe:",
                "solver:",
                "artifact:",
                "requirement:"
            };
        }

        inline auto JsonTryGetValueV2(const json::value &value, const char *key) -> const json::value * {
            const auto *object = value.get_ptr<json::object>();
            if (object == nullptr) return nullptr;
            const auto it = object->find(key);
            return it == object->end() ? nullptr : &it->second;
        }

        inline auto JsonTryGetValueV2(const json::object &object, const char *key) -> const json::value * {
            const auto it = object.find(key);
            return it == object.end() ? nullptr : &it->second;
        }

        inline auto JsonTryGetPathV2(const json::value &value,
                                     const std::initializer_list<const char *> keys) -> const json::value * {
            const json::value *cursor = &value;
            for (const auto *key : keys) {
                cursor = JsonTryGetValueV2(*cursor, key);
                if (cursor == nullptr) return nullptr;
            }
            return cursor;
        }

        inline auto JsonReadStringV2(const json::value &value, const char *key, const Str &fallback = {}) -> Str {
            const auto *entry = JsonTryGetValueV2(value, key);
            if (entry == nullptr) return fallback;
            const auto *string = entry->get_ptr<json::string>();
            return string == nullptr ? fallback : *string;
        }

        inline auto JsonReadStringV2(const json::object &object, const char *key, const Str &fallback = {}) -> Str {
            const auto *entry = JsonTryGetValueV2(object, key);
            if (entry == nullptr) return fallback;
            const auto *string = entry->get_ptr<json::string>();
            return string == nullptr ? fallback : *string;
        }

        inline auto JsonReadBoolV2(const json::value &value, const char *key, const bool fallback = false) -> bool {
            const auto *entry = JsonTryGetValueV2(value, key);
            if (entry == nullptr) return fallback;
            const auto *boolean = entry->get_ptr<json::boolean>();
            return boolean == nullptr ? fallback : *boolean;
        }

        inline auto JsonReadStringArrayV2(const json::value &value) -> StrVector {
            StrVector out;
            const auto *array = value.get_ptr<json::array>();
            if (array == nullptr) return out;

            out.reserve(array->size());
            for (const auto &entry : *array) {
                const auto *string = entry.get_ptr<json::string>();
                if (string == nullptr) continue;
                out.push_back(*string);
            }
            return out;
        }

        inline auto JsonReadStringArrayV2(const json::value &value, const char *key) -> StrVector {
            const auto *entry = JsonTryGetValueV2(value, key);
            return entry == nullptr ? StrVector{} : JsonReadStringArrayV2(*entry);
        }

        inline auto JsonCopyObjectV2(const json::value &value) -> json::object {
            const auto *object = value.get_ptr<json::object>();
            return object == nullptr ? json::object{} : *object;
        }

        inline auto JsonCopyObjectV2(const json::value &value, const char *key) -> json::object {
            const auto *entry = JsonTryGetValueV2(value, key);
            return entry == nullptr ? json::object{} : JsonCopyObjectV2(*entry);
        }

        inline auto JsonValueToStringV2(const json::value &value) -> Str {
            if (const auto *string = value.get_ptr<json::string>(); string != nullptr) return *string;
            if (const auto *number = value.get_ptr<json::number>(); number != nullptr) {
                std::ostringstream ss;
                ss << *number;
                return ss.str();
            }
            if (const auto *boolean = value.get_ptr<json::boolean>(); boolean != nullptr) {
                return *boolean ? "true" : "false";
            }
            if (value.is_null()) return "null";
            return value.dump();
        }

        inline auto ExtractExtraFieldsV2(const json::object &object,
                                         const std::initializer_list<const char *> knownKeys) -> json::object {
            std::set<Str> known;
            for (const auto *key : knownKeys) known.insert(key);

            json::object extra;
            for (const auto &[key, value] : object) {
                if (known.contains(key)) continue;
                extra[key] = value;
            }
            return extra;
        }

        inline auto MakeSyntheticIdV2(const Str &prefix, const std::size_t index) -> Str {
            return "__invalid__" + prefix + "__" + ToStr(index);
        }

        inline auto CopyOrAssignIfEmptyV2(StrVector &target, const StrVector &source) -> void {
            if (!target.empty() || source.empty()) return;
            target = source;
        }

        inline auto ContainsValueV2(const StrVector &values, const Str &needle) -> bool {
            return std::find(values.begin(), values.end(), needle) != values.end();
        }

        inline auto LooksLikeGlobalIdV2(const Str &value) -> bool {
            const auto prefixes = MakeGlobalIdPrefixesV2();
            const auto hasKnownPrefix = std::any_of(prefixes.begin(), prefixes.end(), [&](const auto *prefix) {
                return value.starts_with(prefix);
            });
            if (!hasKnownPrefix) return false;

            return std::all_of(value.begin(), value.end(), [](const char c) {
                return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '.' || c == ':' || c == '-';
            });
        }

        inline auto LayerOrderV2(const Str &layer) -> int {
            if (layer == "ambient") return 0;
            if (layer == "semantic") return 1;
            if (layer == "descent") return 2;
            if (layer == "realization") return 3;
            if (layer == "recipe") return 4;
            if (layer == "artifact") return 5;
            if (layer == "study") return 6;
            return 7;
        }

        inline auto CompactIdSuffixV2(const Str &id) -> Str {
            if (id.empty()) return {};
            const auto colon = id.find_last_of(':');
            if (colon != Str::npos && colon + 1 < id.size()) return id.substr(colon + 1);
            const auto dot = id.find_last_of('.');
            if (dot != Str::npos && dot + 1 < id.size()) return id.substr(dot + 1);
            return id;
        }

        inline auto PreferredPropertyLineV2(const json::object &properties) -> Str {
            constexpr std::array<const char *, 10> preferredKeys{
                "expression_latex",
                "latex",
                "description",
                "reason",
                "condition",
                "via",
                "relation_kind",
                "semantic_role",
                "role_facet",
                "family"
            };

            for (const auto *key : preferredKeys) {
                const auto *entry = JsonTryGetValueV2(properties, key);
                if (entry == nullptr) continue;
                const auto asText = JsonValueToStringV2(*entry);
                if (!asText.empty()) return asText;
            }

            for (const auto &[key, value] : properties) {
                if (value.is_object() || value.is_array()) continue;
                return key + ": " + JsonValueToStringV2(value);
            }

            return {};
        }

        inline auto ActivationStatusFromStringV2(const Str &value) -> EOntologyActivationStatusV2 {
            if (value == "active") return EOntologyActivationStatusV2::Active;
            if (value == "reachable") return EOntologyActivationStatusV2::Reachable;
            if (value == "satisfied") return EOntologyActivationStatusV2::Satisfied;
            if (value == "blocked") return EOntologyActivationStatusV2::Blocked;
            if (value == "unresolved") return EOntologyActivationStatusV2::Unresolved;
            if (value == "partial") return EOntologyActivationStatusV2::Partial;
            return EOntologyActivationStatusV2::None;
        }

        inline auto MergeActivationStatusV2(const EOntologyActivationStatusV2 lhs,
                                            const EOntologyActivationStatusV2 rhs) -> EOntologyActivationStatusV2 {
            const auto rank = [](const EOntologyActivationStatusV2 status) {
                switch (status) {
                    case EOntologyActivationStatusV2::Blocked: return 60;
                    case EOntologyActivationStatusV2::Satisfied: return 50;
                    case EOntologyActivationStatusV2::Active: return 40;
                    case EOntologyActivationStatusV2::Reachable: return 30;
                    case EOntologyActivationStatusV2::Partial: return 20;
                    case EOntologyActivationStatusV2::Unresolved: return 10;
                    case EOntologyActivationStatusV2::None: return 0;
                }

                return 0;
            };

            return rank(rhs) > rank(lhs) ? rhs : lhs;
        }

        inline auto VisitStringLeavesV2(const json::value &value, const std::function<void(const Str &)> &visitor) -> void {
            if (const auto *string = value.get_ptr<json::string>(); string != nullptr) {
                visitor(*string);
                return;
            }

            if (const auto *array = value.get_ptr<json::array>(); array != nullptr) {
                for (const auto &entry : *array) {
                    VisitStringLeavesV2(entry, visitor);
                }
                return;
            }

            if (const auto *object = value.get_ptr<json::object>(); object != nullptr) {
                for (const auto &[key, entry] : *object) {
                    (void) key;
                    VisitStringLeavesV2(entry, visitor);
                }
            }
        }

        inline auto ValidateKnownGlobalReferenceV2(const FOntologyGraphBundle &bundle,
                                                   const Str &candidate,
                                                   const Str &entityId,
                                                   const Str &context,
                                                   const Str &sourceDocument,
                                                   FOntologyValidationResultV2 &validation) -> void {
            if (!LooksLikeGlobalIdV2(candidate)) return;
            const auto *node = bundle.FindNode(candidate);
            if (node == nullptr || node->OwnershipScope != EOntologyOwnershipScopeV2::Global) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.study.unknown_global_reference",
                    entityId,
                    context,
                    "Unknown study reference to global id '" + candidate + "'.",
                    sourceDocument);
            }
        }

        inline auto ParseDocumentMetaV2(const json::value &root,
                                        const Str &sourceDocument,
                                        const Str &expectedDocKind,
                                        FOntologyValidationResultV2 &validation) -> FOntologyDocumentMetaV2 {
            FOntologyDocumentMetaV2 meta;
            const auto *metaValue = JsonTryGetValueV2(root, "meta");
            if (metaValue == nullptr || !metaValue->is_object()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.document.meta_missing",
                    {},
                    "Document",
                    "Missing required object field 'meta'.",
                    sourceDocument);
                return meta;
            }

            const auto &metaObject = metaValue->get<json::object>();
            meta.Format = JsonReadStringV2(metaObject, "format");
            meta.Version = JsonReadStringV2(metaObject, "version");
            meta.DocKind = JsonReadStringV2(metaObject, "doc_kind");
            meta.Title = JsonReadStringV2(metaObject, "title");
            meta.Description = JsonReadStringV2(metaObject, "description");
            meta.ExtraFields = ExtractExtraFieldsV2(metaObject, {"format", "version", "doc_kind", "title", "description"});

            if (meta.Format.empty()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.document.meta_format_missing",
                    {},
                    "Document.Meta",
                    "Missing required field 'meta.format'.",
                    sourceDocument);
            } else if (meta.Format != COntologyFormatV2) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.document.meta_format_invalid",
                    {},
                    "Document.Meta",
                    "Unexpected document format '" + meta.Format + "'.",
                    sourceDocument);
            }

            if (meta.Version.empty()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.document.meta_version_missing",
                    {},
                    "Document.Meta",
                    "Missing required field 'meta.version'.",
                    sourceDocument);
            }

            if (meta.DocKind.empty()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.document.meta_doc_kind_missing",
                    {},
                    "Document.Meta",
                    "Missing required field 'meta.doc_kind'.",
                    sourceDocument);
            } else if (!expectedDocKind.empty() && meta.DocKind != expectedDocKind) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.document.meta_doc_kind_invalid",
                    {},
                    "Document.Meta",
                    "Expected meta.doc_kind='" + expectedDocKind + "' but found '" + meta.DocKind + "'.",
                    sourceDocument);
            }

            return meta;
        }

        inline auto ParseSchemaInfoV2(const json::value &root,
                                      const Str &sourceDocument,
                                      FOntologyValidationResultV2 &validation) -> FOntologySchemaInfoV2 {
            FOntologySchemaInfoV2 info;
            const auto *schemaInfoValue = JsonTryGetValueV2(root, "schema_info");
            if (schemaInfoValue == nullptr || !schemaInfoValue->is_object()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.document.schema_info_missing",
                    {},
                    "Document",
                    "Missing required object field 'schema_info'.",
                    sourceDocument);
                return info;
            }

            const auto &schemaInfoObject = schemaInfoValue->get<json::object>();
            info.NodeKinds = JsonReadStringArrayV2(*schemaInfoValue, "node_kinds");
            info.EdgeKinds = JsonReadStringArrayV2(*schemaInfoValue, "edge_kinds");
            info.Layers = JsonReadStringArrayV2(*schemaInfoValue, "layers");
            info.ProvenanceKinds = JsonReadStringArrayV2(*schemaInfoValue, "provenance_kinds");
            info.ExtraFields = ExtractExtraFieldsV2(
                schemaInfoObject,
                {"node_kinds", "edge_kinds", "layers", "provenance_kinds"});

            if (info.NodeKinds.empty()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Warning,
                    "ontology.document.schema_info_node_kinds_empty",
                    {},
                    "Document.SchemaInfo",
                    "schema_info.node_kinds is empty.",
                    sourceDocument);
            }

            return info;
        }

        inline auto ParseNodeRecordV2(const json::value &nodeValue,
                                      const std::size_t index,
                                      const FOntologySchemaDocument &schema,
                                      const Str &sourceDocument,
                                      const EOntologyDocumentKindV2 sourceKind,
                                      const EOntologyOwnershipScopeV2 ownershipScope,
                                      FOntologyValidationResultV2 &validation) -> std::optional<FOntologyNodeRecordV2> {
            if (!nodeValue.is_object()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.node.not_object",
                    {},
                    "Node",
                    "Node entry is not an object.",
                    sourceDocument);
                return std::nullopt;
            }

            FOntologyNodeRecordV2 node;
            const auto &object = nodeValue.get<json::object>();

            node.Id = JsonReadStringV2(object, "id");
            if (node.Id.empty()) {
                node.Id = MakeSyntheticIdV2("node", index);
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.node.id_missing",
                    node.Id,
                    "Node",
                    "Missing required field 'id'.",
                    sourceDocument);
            }

            node.Kind = JsonReadStringV2(object, "kind");
            if (node.Kind.empty()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.node.kind_missing",
                    node.Id,
                    "Node",
                    "Missing required field 'kind'.",
                    sourceDocument);
            } else if (!schema.NodeKinds.empty() && !ContainsValueV2(schema.NodeKinds, node.Kind)) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Warning,
                    "ontology.node.kind_unknown",
                    node.Id,
                    "Node",
                    "Unknown node kind '" + node.Kind + "'.",
                    sourceDocument);
            }

            node.Title = JsonReadStringV2(object, "title");
            if (node.Title.empty()) {
                node.Title = node.Id;
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.node.title_missing",
                    node.Id,
                    "Node",
                    "Missing required field 'title'.",
                    sourceDocument);
            }

            node.Layer = JsonReadStringV2(object, "layer");
            if (node.Layer.empty()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.node.layer_missing",
                    node.Id,
                    "Node",
                    "Missing required field 'layer'.",
                    sourceDocument);
            } else if (!schema.Layers.empty() && !ContainsValueV2(schema.Layers, node.Layer)) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Warning,
                    "ontology.node.layer_unknown",
                    node.Id,
                    "Node",
                    "Unknown node layer '" + node.Layer + "'.",
                    sourceDocument);
            }

            const auto *abstractValue = JsonTryGetValueV2(object, "abstract");
            if (abstractValue == nullptr || abstractValue->get_ptr<json::boolean>() == nullptr) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.node.abstract_missing",
                    node.Id,
                    "Node",
                    "Missing required bool field 'abstract'.",
                    sourceDocument);
            }
            node.bAbstract = abstractValue != nullptr && abstractValue->get_ptr<json::boolean>() != nullptr
                ? abstractValue->get<json::boolean>()
                : false;

            const auto *propertiesValue = JsonTryGetValueV2(object, "properties");
            if (propertiesValue == nullptr || !propertiesValue->is_object()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.node.properties_missing",
                    node.Id,
                    "Node",
                    "Missing required object field 'properties'.",
                    sourceDocument);
            }
            node.Properties = propertiesValue != nullptr ? JsonCopyObjectV2(*propertiesValue) : json::object{};
            node.ExtraFields = ExtractExtraFieldsV2(object, {"id", "kind", "title", "layer", "abstract", "properties"});
            node.SourceDocument = sourceDocument;
            node.SourceKind = sourceKind;
            node.OwnershipScope = ownershipScope;
            return node;
        }

        inline auto ParseEdgeRecordV2(const json::value &edgeValue,
                                      const std::size_t index,
                                      const FOntologySchemaDocument &schema,
                                      const Str &sourceDocument,
                                      const EOntologyDocumentKindV2 sourceKind,
                                      const EOntologyOwnershipScopeV2 ownershipScope,
                                      FOntologyValidationResultV2 &validation) -> std::optional<FOntologyEdgeRecordV2> {
            if (!edgeValue.is_object()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.edge.not_object",
                    {},
                    "Edge",
                    "Edge entry is not an object.",
                    sourceDocument);
                return std::nullopt;
            }

            FOntologyEdgeRecordV2 edge;
            const auto &object = edgeValue.get<json::object>();

            edge.Id = JsonReadStringV2(object, "id");
            if (edge.Id.empty()) {
                edge.Id = MakeSyntheticIdV2("edge", index);
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.edge.id_missing",
                    edge.Id,
                    "Edge",
                    "Missing required field 'id'.",
                    sourceDocument);
            }

            edge.Type = JsonReadStringV2(object, "type");
            if (edge.Type.empty()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.edge.type_missing",
                    edge.Id,
                    "Edge",
                    "Missing required field 'type'.",
                    sourceDocument);
            } else if (!schema.EdgeKinds.empty() && !ContainsValueV2(schema.EdgeKinds, edge.Type)) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Warning,
                    "ontology.edge.type_unknown",
                    edge.Id,
                    "Edge",
                    "Unknown edge type '" + edge.Type + "'.",
                    sourceDocument);
            }

            edge.From = JsonReadStringV2(object, "from");
            if (edge.From.empty()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.edge.from_missing",
                    edge.Id,
                    "Edge",
                    "Missing required field 'from'.",
                    sourceDocument);
            }

            edge.To = JsonReadStringV2(object, "to");
            if (edge.To.empty()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.edge.to_missing",
                    edge.Id,
                    "Edge",
                    "Missing required field 'to'.",
                    sourceDocument);
            }

            const auto *propertiesValue = JsonTryGetValueV2(object, "properties");
            if (propertiesValue == nullptr || !propertiesValue->is_object()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.edge.properties_missing",
                    edge.Id,
                    "Edge",
                    "Missing required object field 'properties'.",
                    sourceDocument);
            }
            edge.Properties = propertiesValue != nullptr ? JsonCopyObjectV2(*propertiesValue) : json::object{};
            edge.ExtraFields = ExtractExtraFieldsV2(object, {"id", "type", "from", "to", "properties"});
            edge.SourceDocument = sourceDocument;
            edge.SourceKind = sourceKind;
            edge.OwnershipScope = ownershipScope;
            return edge;
        }

        inline auto ValidateActivationStatusValueV2(const FOntologySchemaDocument &schema,
                                                    const Str &status,
                                                    const Str &entityId,
                                                    const Str &context,
                                                    const Str &sourceDocument,
                                                    FOntologyValidationResultV2 &validation) -> void {
            if (status.empty()) return;
            const auto &allowed = schema.ActivationStatuses;
            if (!allowed.empty() && !ContainsValueV2(allowed, status)) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Warning,
                    "ontology.activation.status_unknown",
                    entityId,
                    context,
                    "Unknown activation status value '" + status + "'.",
                    sourceDocument);
            }
        }

        inline auto BuildStudyStatusMapV2(const FOntologyGraphBundle &bundle,
                                          const FStudyOntologyDocument &study) -> std::map<Str, EOntologyActivationStatusV2> {
            std::map<Str, EOntologyActivationStatusV2> statusByNodeId;

            const auto mergeStatus = [&](const Str &nodeId, const EOntologyActivationStatusV2 status) {
                if (nodeId.empty() || status == EOntologyActivationStatusV2::None) return;
                statusByNodeId[nodeId] = MergeActivationStatusV2(statusByNodeId[nodeId], status);
            };

            const auto *summaryObject = study.ActivationSummary.get_ptr<json::object>();
            if (summaryObject != nullptr) {
                for (const auto &[statusKey, value] : *summaryObject) {
                    const auto status = ActivationStatusFromStringV2(statusKey);
                    if (status == EOntologyActivationStatusV2::None) continue;
                    const auto ids = JsonReadStringArrayV2(value);
                    for (const auto &id : ids) {
                        mergeStatus(id, status);
                    }
                }
            }

            for (const auto &edge : study.Edges) {
                auto status = ActivationStatusFromStringV2(JsonReadStringV2(edge.Properties, "status"));
                if (status == EOntologyActivationStatusV2::None) {
                    if (edge.Type == "blocks") status = EOntologyActivationStatusV2::Blocked;
                    else if (edge.Type == "satisfies") status = EOntologyActivationStatusV2::Satisfied;
                }

                if (edge.Type == "activates" || edge.Type == "satisfies" || edge.Type == "blocks") {
                    mergeStatus(edge.To, status);
                }
            }

            if (const auto *frontierObject = study.RealizationFrontier.get_ptr<json::object>(); frontierObject != nullptr) {
                if (const auto *nextTargets = JsonTryGetValueV2(*frontierObject, "next_targets"); nextTargets != nullptr) {
                    for (const auto &id : JsonReadStringArrayV2(*nextTargets)) {
                        mergeStatus(id, EOntologyActivationStatusV2::Reachable);
                    }
                }

                if (const auto *blocking = JsonTryGetValueV2(*frontierObject, "blocking_requirements"); blocking != nullptr) {
                    for (const auto &id : JsonReadStringArrayV2(*blocking)) {
                        mergeStatus(id, EOntologyActivationStatusV2::Blocked);
                    }
                }
            }

            if (!study.Study.Id.empty()) {
                mergeStatus(study.Study.Id, EOntologyActivationStatusV2::Active);
            }

            (void) bundle;
            return statusByNodeId;
        }

        inline auto ComputeStudyLocalNodeIdsV2(const FStudyOntologyDocument &study) -> std::set<Str> {
            std::set<Str> ids;
            for (const auto &node : study.Nodes) {
                ids.insert(node.Id);
            }
            return ids;
        }

        inline auto LayoutPreviewTextV2(const FOntologyNodeRecordV2 &node) -> Str {
            auto preview = PreferredPropertyLineV2(node.Properties);
            if (preview.empty()) {
                preview = CompactIdSuffixV2(node.Id);
            }
            return preview;
        }

        inline auto EstimateNodeLayoutWidthUnitsV2(const FOntologyNodeRecordV2 &node,
                                                  const bool bStudyRoot = false) -> DevFloat {
            const auto titleChars = std::clamp<std::size_t>(node.Title.size(), 8, 60);
            const auto titleLineCount = std::clamp<std::size_t>((titleChars + 19) / 20, 1, 3);
            const auto wrappedTitleChars = static_cast<DevFloat>(std::clamp<std::size_t>(
                (titleChars + titleLineCount - 1) / titleLineCount,
                10,
                24));
            const auto categoryChars = static_cast<DevFloat>(std::clamp<std::size_t>(
                FriendlyNodeCategoryLabelV2(node.Kind, node.Layer).size(),
                6,
                14));
            const auto badgeReserve = bStudyRoot
                ? static_cast<DevFloat>(4.6)
                : (node.OwnershipScope == EOntologyOwnershipScopeV2::StudyLocal ? 4.0 : 3.7);
            const auto statusReserve = static_cast<DevFloat>(4.9);
            const auto dominantChars = std::max(wrappedTitleChars, categoryChars + static_cast<DevFloat>(2.0));
            return std::clamp(
                7.2 + (0.30 * dominantChars) + badgeReserve + statusReserve,
                12.0,
                22.0);
        }

        inline auto EstimateNodeLayoutHeightUnitsV2(const FOntologyNodeRecordV2 &node,
                                                    const bool bStudyRoot = false) -> DevFloat {
            const auto titleChars = std::clamp<std::size_t>(node.Title.size(), 8, 60);
            const auto titleLineCount = static_cast<DevFloat>(std::clamp<std::size_t>((titleChars + 19) / 20, 1, 3));
            auto height = static_cast<DevFloat>(6.6) + ((titleLineCount - 1.0) * 1.6);
            if (bStudyRoot) {
                height += 0.5;
            }
            return height;
        }

        inline auto ExpandIncidentNodesV2(const FOntologyGraphBundle &bundle,
                                          const std::set<Str> &seedNodeIds,
                                          const std::function<bool(const FOntologyEdgeRecordV2 &)> &edgePredicate)
            -> std::set<Str> {
            std::set<Str> expanded = seedNodeIds;
            for (const auto &[edgeId, edge] : bundle.EdgeTable) {
                (void) edgeId;
                if (!edgePredicate(edge)) continue;
                const bool bTouchesFrom = seedNodeIds.contains(edge.From);
                const bool bTouchesTo = seedNodeIds.contains(edge.To);
                if (!bTouchesFrom && !bTouchesTo) continue;
                if (!edge.From.empty()) expanded.insert(edge.From);
                if (!edge.To.empty()) expanded.insert(edge.To);
            }
            return expanded;
        }

        inline auto ComputeGlobalNodePositionsV2(const FOntologyGraphBundle &bundle,
                                                 const FStudyOntologyDocument *selectedStudy)
            -> std::map<Str, Slab::Graphics::FPoint2D> {
            constexpr DevFloat HorizontalCompression = 0.76;
            constexpr DevFloat LayerGap = 4.8;
            constexpr DevFloat RowGap = 1.9;
            constexpr DevFloat GroupGap = 2.2;
            constexpr DevFloat StudySidecarGap = 2.8;
            constexpr DevFloat ColumnOuterPadding = 0.7;

            std::map<Str, Slab::Graphics::FPoint2D> positions;
            std::map<Str, int> nodeKindOrder;
            for (std::size_t i = 0; i < bundle.Schema.NodeKinds.size(); ++i) {
                nodeKindOrder[bundle.Schema.NodeKinds[i]] = static_cast<int>(i);
            }

            std::map<Str, Vector<const FOntologyNodeRecordV2 *>> globalByLayer;
            std::map<Str, Vector<const FOntologyNodeRecordV2 *>> studyByLayer;
            std::map<Str, DevFloat> reservedStudyWidthByLayer;
            for (const auto &study : bundle.StudyDocuments) {
                for (const auto &studyNode : study.Nodes) {
                    const bool bStudyRoot = studyNode.Id == study.Study.Id;
                    reservedStudyWidthByLayer[studyNode.Layer] = std::max(
                        reservedStudyWidthByLayer[studyNode.Layer],
                        EstimateNodeLayoutWidthUnitsV2(studyNode, bStudyRoot) + (2.0 * ColumnOuterPadding));
                }
            }

            const auto selectedStudyNodeIds = selectedStudy != nullptr
                ? ComputeStudyLocalNodeIdsV2(*selectedStudy)
                : std::set<Str>{};
            for (const auto &[nodeId, node] : bundle.NodeTable) {
                (void) nodeId;
                if (selectedStudy == nullptr) {
                    if (node.OwnershipScope == EOntologyOwnershipScopeV2::Global) {
                        globalByLayer[node.Layer].push_back(&node);
                    }
                    continue;
                }

                if (node.OwnershipScope == EOntologyOwnershipScopeV2::Global) {
                    globalByLayer[node.Layer].push_back(&node);
                    continue;
                }

                if (selectedStudyNodeIds.contains(node.Id)) {
                    studyByLayer[node.Layer].push_back(&node);
                }
            }

            auto sortNodes = [&](Vector<const FOntologyNodeRecordV2 *> &nodes, const bool bStudyNodes) {
                std::sort(nodes.begin(), nodes.end(), [&](const auto *lhs, const auto *rhs) {
                    const bool lhsRoot = bStudyNodes && selectedStudy != nullptr && lhs->Id == selectedStudy->Study.Id;
                    const bool rhsRoot = bStudyNodes && selectedStudy != nullptr && rhs->Id == selectedStudy->Study.Id;
                    if (lhsRoot != rhsRoot) return lhsRoot;

                    const auto lhsOrder = nodeKindOrder.contains(lhs->Kind) ? nodeKindOrder[lhs->Kind] : 999;
                    const auto rhsOrder = nodeKindOrder.contains(rhs->Kind) ? nodeKindOrder[rhs->Kind] : 999;
                    if (lhsOrder != rhsOrder) return lhsOrder < rhsOrder;
                    if (lhs->Title != rhs->Title) return lhs->Title < rhs->Title;
                    return lhs->Id < rhs->Id;
                });
            };

            std::set<Str> layers;
            for (const auto &[layer, nodes] : globalByLayer) {
                if (!nodes.empty()) layers.insert(layer);
            }
            for (const auto &[layer, nodes] : studyByLayer) {
                if (!nodes.empty()) layers.insert(layer);
            }

            Vector<Str> orderedLayers(layers.begin(), layers.end());
            std::sort(orderedLayers.begin(), orderedLayers.end(), [&](const auto &lhs, const auto &rhs) {
                const auto lhsOrder = LayerOrderV2(lhs);
                const auto rhsOrder = LayerOrderV2(rhs);
                if (lhsOrder != rhsOrder) return lhsOrder < rhsOrder;
                return lhs < rhs;
            });

            std::map<Str, DevFloat> globalCenterByLayer;
            std::map<Str, DevFloat> studyCenterByLayer;
            DevFloat cursorX = 0.0;
            for (const auto &layer : orderedLayers) {
                const auto &globalNodes = globalByLayer[layer];
                const auto &studyNodes = studyByLayer[layer];
                DevFloat globalWidth = 0.0;
                for (const auto *node : globalNodes) {
                    globalWidth = std::max(
                        globalWidth,
                        EstimateNodeLayoutWidthUnitsV2(*node, false) + (2.0 * ColumnOuterPadding));
                }

                const auto reservedStudyWidth = !studyNodes.empty()
                    ? reservedStudyWidthByLayer[layer]
                    : static_cast<DevFloat>(0.0);
                const bool bHasGlobalNodes = !globalNodes.empty();
                const bool bHasStudyNodes = !studyNodes.empty();

                if (bHasGlobalNodes) {
                    globalCenterByLayer[layer] = cursorX + (0.5 * globalWidth);
                    cursorX += globalWidth;
                    if (bHasStudyNodes && reservedStudyWidth > 0.0) {
                        studyCenterByLayer[layer] = cursorX + StudySidecarGap + (0.5 * reservedStudyWidth);
                        cursorX += StudySidecarGap + reservedStudyWidth;
                    }
                } else if (bHasStudyNodes && reservedStudyWidth > 0.0) {
                    studyCenterByLayer[layer] = cursorX + (0.5 * reservedStudyWidth);
                    cursorX += reservedStudyWidth;
                }

                cursorX += LayerGap;
            }

            const auto assignColumn = [&](const Vector<const FOntologyNodeRecordV2 *> &nodes, const DevFloat x) {
                if (nodes.empty()) return;

                Vector<DevFloat> heights;
                heights.reserve(nodes.size());
                for (const auto *node : nodes) {
                    const bool bStudyRoot = selectedStudy != nullptr && node->Id == selectedStudy->Study.Id;
                    heights.push_back(EstimateNodeLayoutHeightUnitsV2(*node, bStudyRoot));
                }

                DevFloat totalHeight = heights.front();
                for (std::size_t index = 1; index < nodes.size(); ++index) {
                    const bool bKindBreak = nodes[index - 1]->Kind != nodes[index]->Kind;
                    totalHeight +=
                        (0.5 * heights[index - 1]) +
                        (0.5 * heights[index]) +
                        RowGap +
                        (bKindBreak ? GroupGap : 0.0);
                }

                auto y = (0.5 * totalHeight) - (0.5 * heights.front());
                positions[nodes.front()->Id] = {x, y};
                for (std::size_t index = 1; index < nodes.size(); ++index) {
                    const bool bKindBreak = nodes[index - 1]->Kind != nodes[index]->Kind;
                    y -=
                        (0.5 * heights[index - 1]) +
                        (0.5 * heights[index]) +
                        RowGap +
                        (bKindBreak ? GroupGap : 0.0);
                    positions[nodes[index]->Id] = {x, y};
                }
            };

            for (const auto &layer : orderedLayers) {
                auto globalNodes = globalByLayer[layer];
                auto studyNodes = studyByLayer[layer];
                sortNodes(globalNodes, false);
                sortNodes(studyNodes, true);

                const auto globalCenterIt = globalCenterByLayer.find(layer);
                if (globalCenterIt != globalCenterByLayer.end()) {
                    assignColumn(globalNodes, globalCenterIt->second);
                }

                const auto studyCenterIt = studyCenterByLayer.find(layer);
                if (studyCenterIt != studyCenterByLayer.end()) {
                    assignColumn(studyNodes, studyCenterIt->second);
                }
            }

            for (auto &[nodeId, position] : positions) {
                (void) nodeId;
                position.x *= HorizontalCompression;
            }

            return positions;
        }

        inline auto BuildNodeSummaryV2(const FOntologyNodeRecordV2 &node,
                                       const EOntologyActivationStatusV2 status) -> std::pair<Str, Str> {
            auto summary = FriendlyNodeCategoryLabelV2(node.Kind, node.Layer);
            if (status != EOntologyActivationStatusV2::None) {
                summary += " / ";
                summary += ToString(status);
            }

            auto secondary = PreferredPropertyLineV2(node.Properties);
            if (secondary.empty()) {
                secondary = CompactIdSuffixV2(node.Id);
            }

            return {summary, secondary};
        }

        inline auto BuildEdgeSummaryV2(const FOntologyEdgeRecordV2 &edge,
                                       const EOntologyActivationStatusV2 status) -> Str {
            auto summary = edge.Type;
            if (status != EOntologyActivationStatusV2::None) {
                summary += " / ";
                summary += ToString(status);
            }

            const auto detail = PreferredPropertyLineV2(edge.Properties);
            if (!detail.empty()) {
                summary += " / ";
                summary += detail;
            }

            return summary;
        }

    } // namespace Detail

    inline auto DiscoverOntologyStudyDocumentsV2(const std::filesystem::path &directory)
        -> Vector<std::filesystem::path> {
        Vector<std::filesystem::path> paths;
        if (directory.empty() || !std::filesystem::exists(directory)) return paths;

        for (const auto &entry : std::filesystem::directory_iterator(directory)) {
            if (!entry.is_regular_file()) continue;
            const auto filename = entry.path().filename().string();
            if (!filename.ends_with(".study.json")) continue;
            paths.push_back(entry.path());
        }

        std::sort(paths.begin(), paths.end());
        return paths;
    }

    inline auto LoadOntologySchemaDocumentV2(const std::filesystem::path &schemaPath,
                                             FOntologyValidationResultV2 &validation)
        -> FOntologySchemaDocument {
        FOntologySchemaDocument schema;
        schema.SourcePath = schemaPath;
        schema.SourceDocument = schemaPath.string();
        schema.ActivationStatuses = Detail::MakeDefaultActivationStatusesV2();

        const auto [rootValue, loaded] = json::value::load(schemaPath.string());
        if (!loaded || rootValue.is_discarded()) {
            validation.Add(
                ModelV2::EValidationSeverityV2::Error,
                "ontology.schema.load_failed",
                {},
                "Schema",
                "Failed to load schema JSON from '" + schemaPath.string() + "'.",
                schema.SourceDocument);
            return schema;
        }

        schema.RawRoot = rootValue;
        schema.Title = Detail::JsonReadStringV2(rootValue, "title");
        schema.Description = Detail::JsonReadStringV2(rootValue, "description");
        schema.SchemaId = Detail::JsonReadStringV2(rootValue, "$id");
        if (const auto *nodeKinds = Detail::JsonTryGetPathV2(rootValue, {"$defs", "node", "properties", "kind", "enum"}); nodeKinds != nullptr) {
            schema.NodeKinds = Detail::JsonReadStringArrayV2(*nodeKinds);
        }
        if (const auto *edgeKinds = Detail::JsonTryGetPathV2(rootValue, {"$defs", "edge", "properties", "type", "enum"}); edgeKinds != nullptr) {
            schema.EdgeKinds = Detail::JsonReadStringArrayV2(*edgeKinds);
        }
        if (const auto *layers = Detail::JsonTryGetPathV2(rootValue, {"$defs", "node", "properties", "layer", "enum"}); layers != nullptr) {
            schema.Layers = Detail::JsonReadStringArrayV2(*layers);
        }

        if (schema.NodeKinds.empty() || schema.EdgeKinds.empty() || schema.Layers.empty()) {
            validation.Add(
                ModelV2::EValidationSeverityV2::Warning,
                "ontology.schema.enums_incomplete",
                {},
                "Schema",
                "Schema did not provide a complete set of node/edge/layer enums; runtime validation will fall back where possible.",
                schema.SourceDocument);
        }

        schema.bLoaded = true;
        return schema;
    }

    inline auto LoadGlobalDescentOntologyDocumentV2(const std::filesystem::path &globalPath,
                                                    const FOntologySchemaDocument &schema,
                                                    FOntologyValidationResultV2 &validation)
        -> FGlobalDescentOntologyDocument {
        FGlobalDescentOntologyDocument document;
        document.SourcePath = globalPath;
        document.SourceDocument = globalPath.string();

        const auto [rootValue, loaded] = json::value::load(globalPath.string());
        if (!loaded || rootValue.is_discarded()) {
            validation.Add(
                ModelV2::EValidationSeverityV2::Error,
                "ontology.global.load_failed",
                {},
                "GlobalOntology",
                "Failed to load global ontology JSON from '" + globalPath.string() + "'.",
                document.SourceDocument);
            return document;
        }

        document.RawRoot = rootValue;
        document.Meta = Detail::ParseDocumentMetaV2(rootValue, document.SourceDocument, "ontology", validation);
        document.SchemaInfo = Detail::ParseSchemaInfoV2(rootValue, document.SourceDocument, validation);
        document.ExtraFields = Detail::JsonCopyObjectV2(rootValue);
        document.ExtraFields.erase("meta");
        document.ExtraFields.erase("schema_info");
        document.ExtraFields.erase("nodes");
        document.ExtraFields.erase("edges");

        std::set<Str> seenNodeIds;
        if (const auto *nodesValue = Detail::JsonTryGetValueV2(rootValue, "nodes"); nodesValue != nullptr && nodesValue->is_array()) {
            const auto &nodesArray = nodesValue->get<json::array>();
            for (std::size_t index = 0; index < nodesArray.size(); ++index) {
                auto node = Detail::ParseNodeRecordV2(
                    nodesArray[index],
                    index,
                    schema,
                    document.SourceDocument,
                    EOntologyDocumentKindV2::GlobalOntology,
                    EOntologyOwnershipScopeV2::Global,
                    validation);
                if (!node.has_value()) continue;
                if (!seenNodeIds.insert(node->Id).second) {
                    validation.Add(
                        ModelV2::EValidationSeverityV2::Error,
                        "ontology.global.duplicate_node_id",
                        node->Id,
                        "GlobalOntology.Nodes",
                        "Duplicate node id within document; later entry skipped.",
                        document.SourceDocument);
                    continue;
                }
                document.Nodes.push_back(std::move(*node));
            }
        } else {
            validation.Add(
                ModelV2::EValidationSeverityV2::Error,
                "ontology.global.nodes_missing",
                {},
                "GlobalOntology",
                "Missing required array field 'nodes'.",
                document.SourceDocument);
        }

        std::set<Str> seenEdgeIds;
        if (const auto *edgesValue = Detail::JsonTryGetValueV2(rootValue, "edges"); edgesValue != nullptr && edgesValue->is_array()) {
            const auto &edgesArray = edgesValue->get<json::array>();
            for (std::size_t index = 0; index < edgesArray.size(); ++index) {
                auto edge = Detail::ParseEdgeRecordV2(
                    edgesArray[index],
                    index,
                    schema,
                    document.SourceDocument,
                    EOntologyDocumentKindV2::GlobalOntology,
                    EOntologyOwnershipScopeV2::Global,
                    validation);
                if (!edge.has_value()) continue;
                if (!seenEdgeIds.insert(edge->Id).second) {
                    validation.Add(
                        ModelV2::EValidationSeverityV2::Error,
                        "ontology.global.duplicate_edge_id",
                        edge->Id,
                        "GlobalOntology.Edges",
                        "Duplicate edge id within document; later entry skipped.",
                        document.SourceDocument);
                    continue;
                }
                document.Edges.push_back(std::move(*edge));
            }
        } else {
            validation.Add(
                ModelV2::EValidationSeverityV2::Error,
                "ontology.global.edges_missing",
                {},
                "GlobalOntology",
                "Missing required array field 'edges'.",
                document.SourceDocument);
        }

        document.bLoaded = true;
        return document;
    }

    inline auto LoadStudyOntologyDocumentV2(const std::filesystem::path &studyPath,
                                            const FOntologySchemaDocument &schema,
                                            FOntologyValidationResultV2 &validation)
        -> FStudyOntologyDocument {
        FStudyOntologyDocument document;
        document.SourcePath = studyPath;
        document.SourceDocument = studyPath.string();

        const auto [rootValue, loaded] = json::value::load(studyPath.string());
        if (!loaded || rootValue.is_discarded()) {
            validation.Add(
                ModelV2::EValidationSeverityV2::Error,
                "ontology.study.load_failed",
                {},
                "Study",
                "Failed to load study JSON from '" + studyPath.string() + "'.",
                document.SourceDocument);
            return document;
        }

        document.RawRoot = rootValue;
        document.Meta = Detail::ParseDocumentMetaV2(rootValue, document.SourceDocument, "study", validation);

        if (const auto *refValue = Detail::JsonTryGetValueV2(rootValue, "ontology_ref"); refValue != nullptr && refValue->is_object()) {
            const auto &refObject = refValue->get<json::object>();
            document.OntologyRef.Path = Detail::JsonReadStringV2(refObject, "path");
            document.OntologyRef.Version = Detail::JsonReadStringV2(refObject, "version");
            document.OntologyRef.ExtraFields = Detail::ExtractExtraFieldsV2(refObject, {"path", "version"});
            if (document.OntologyRef.Path.empty()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.study.ontology_ref_path_missing",
                    {},
                    "Study.OntologyRef",
                    "Missing required field 'ontology_ref.path'.",
                    document.SourceDocument);
            }
            if (document.OntologyRef.Version.empty()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.study.ontology_ref_version_missing",
                    {},
                    "Study.OntologyRef",
                    "Missing required field 'ontology_ref.version'.",
                    document.SourceDocument);
            }
        } else {
            validation.Add(
                ModelV2::EValidationSeverityV2::Error,
                "ontology.study.ontology_ref_missing",
                {},
                "Study",
                "Missing required object field 'ontology_ref'.",
                document.SourceDocument);
        }

        if (const auto *studyValue = Detail::JsonTryGetValueV2(rootValue, "study"); studyValue != nullptr && studyValue->is_object()) {
            const auto &studyObject = studyValue->get<json::object>();
            document.Study.Id = Detail::JsonReadStringV2(studyObject, "id");
            document.Study.Title = Detail::JsonReadStringV2(studyObject, "title");
            document.Study.Description = Detail::JsonReadStringV2(studyObject, "description");
            document.Study.BaseVocabulary = Detail::JsonReadStringV2(studyObject, "base_vocabulary");
            if (const auto *notationContext = Detail::JsonTryGetValueV2(studyObject, "notation_context"); notationContext != nullptr) {
                document.Study.NotationContext = *notationContext;
            }
            document.Study.ExtraFields = Detail::ExtractExtraFieldsV2(
                studyObject,
                {"id", "title", "description", "base_vocabulary", "notation_context"});

            if (document.Study.Id.empty()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.study.id_missing",
                    {},
                    "Study.Info",
                    "Missing required field 'study.id'.",
                    document.SourceDocument);
            }
            if (document.Study.Title.empty()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.study.title_missing",
                    document.Study.Id,
                    "Study.Info",
                    "Missing required field 'study.title'.",
                    document.SourceDocument);
            }
            if (document.Study.Description.empty()) {
                validation.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.study.description_missing",
                    document.Study.Id,
                    "Study.Info",
                    "Missing required field 'study.description'.",
                    document.SourceDocument);
            }
        } else {
            validation.Add(
                ModelV2::EValidationSeverityV2::Error,
                "ontology.study.info_missing",
                {},
                "Study",
                "Missing required object field 'study'.",
                document.SourceDocument);
        }

        std::set<Str> seenNodeIds;
        if (const auto *nodesValue = Detail::JsonTryGetValueV2(rootValue, "nodes"); nodesValue != nullptr && nodesValue->is_array()) {
            const auto &nodesArray = nodesValue->get<json::array>();
            for (std::size_t index = 0; index < nodesArray.size(); ++index) {
                auto node = Detail::ParseNodeRecordV2(
                    nodesArray[index],
                    index,
                    schema,
                    document.SourceDocument,
                    EOntologyDocumentKindV2::Study,
                    EOntologyOwnershipScopeV2::StudyLocal,
                    validation);
                if (!node.has_value()) continue;
                if (!seenNodeIds.insert(node->Id).second) {
                    validation.Add(
                        ModelV2::EValidationSeverityV2::Error,
                        "ontology.study.duplicate_node_id",
                        node->Id,
                        "Study.Nodes",
                        "Duplicate node id within document; later entry skipped.",
                        document.SourceDocument);
                    continue;
                }
                document.Nodes.push_back(std::move(*node));
            }
        } else {
            validation.Add(
                ModelV2::EValidationSeverityV2::Error,
                "ontology.study.nodes_missing",
                document.Study.Id,
                "Study",
                "Missing required array field 'nodes'.",
                document.SourceDocument);
        }

        std::set<Str> seenEdgeIds;
        if (const auto *edgesValue = Detail::JsonTryGetValueV2(rootValue, "edges"); edgesValue != nullptr && edgesValue->is_array()) {
            const auto &edgesArray = edgesValue->get<json::array>();
            for (std::size_t index = 0; index < edgesArray.size(); ++index) {
                auto edge = Detail::ParseEdgeRecordV2(
                    edgesArray[index],
                    index,
                    schema,
                    document.SourceDocument,
                    EOntologyDocumentKindV2::Study,
                    EOntologyOwnershipScopeV2::StudyLocal,
                    validation);
                if (!edge.has_value()) continue;
                if (!seenEdgeIds.insert(edge->Id).second) {
                    validation.Add(
                        ModelV2::EValidationSeverityV2::Error,
                        "ontology.study.duplicate_edge_id",
                        edge->Id,
                        "Study.Edges",
                        "Duplicate edge id within document; later entry skipped.",
                        document.SourceDocument);
                    continue;
                }
                const auto status = Detail::JsonReadStringV2(edge->Properties, "status");
                if (edge->Type == "activates" || edge->Type == "satisfies" || edge->Type == "blocks") {
                    Detail::ValidateActivationStatusValueV2(
                        schema,
                        status,
                        edge->Id,
                        "Study.Edge.Properties.status",
                        document.SourceDocument,
                        validation);
                }
                document.Edges.push_back(std::move(*edge));
            }
        } else {
            validation.Add(
                ModelV2::EValidationSeverityV2::Error,
                "ontology.study.edges_missing",
                document.Study.Id,
                "Study",
                "Missing required array field 'edges'.",
                document.SourceDocument);
        }

        if (const auto *activationSummary = Detail::JsonTryGetValueV2(rootValue, "activation_summary"); activationSummary != nullptr) {
            document.ActivationSummary = *activationSummary;
            if (const auto *summaryObject = activationSummary->get_ptr<json::object>(); summaryObject != nullptr) {
                for (const auto &[statusKey, value] : *summaryObject) {
                    Detail::ValidateActivationStatusValueV2(
                        schema,
                        statusKey,
                        document.Study.Id,
                        "Study.ActivationSummary",
                        document.SourceDocument,
                        validation);
                    (void) value;
                }
            }
        }

        if (const auto *realizationFrontier = Detail::JsonTryGetValueV2(rootValue, "realization_frontier"); realizationFrontier != nullptr) {
            document.RealizationFrontier = *realizationFrontier;
            const auto frontierStatus = Detail::JsonReadStringV2(document.RealizationFrontier, "status");
            Detail::ValidateActivationStatusValueV2(
                schema,
                frontierStatus,
                document.Study.Id,
                "Study.RealizationFrontier.status",
                document.SourceDocument,
                validation);
        }

        document.ExtraFields = Detail::JsonCopyObjectV2(rootValue);
        document.ExtraFields.erase("meta");
        document.ExtraFields.erase("ontology_ref");
        document.ExtraFields.erase("study");
        document.ExtraFields.erase("nodes");
        document.ExtraFields.erase("edges");
        document.ExtraFields.erase("activation_summary");
        document.ExtraFields.erase("realization_frontier");

        document.bLoaded = true;
        return document;
    }

    inline auto LoadOntologyGraphBundleV2(const std::filesystem::path &schemaPath,
                                          const std::filesystem::path &globalPath,
                                          const Vector<std::filesystem::path> &studyPaths)
        -> FOntologyGraphBundle {
        FOntologyGraphBundle bundle;
        bundle.Schema = LoadOntologySchemaDocumentV2(schemaPath, bundle.Diagnostics);
        bundle.GlobalDocument = LoadGlobalDescentOntologyDocumentV2(globalPath, bundle.Schema, bundle.Diagnostics);

        Detail::CopyOrAssignIfEmptyV2(bundle.Schema.NodeKinds, bundle.GlobalDocument.SchemaInfo.NodeKinds);
        Detail::CopyOrAssignIfEmptyV2(bundle.Schema.EdgeKinds, bundle.GlobalDocument.SchemaInfo.EdgeKinds);
        Detail::CopyOrAssignIfEmptyV2(bundle.Schema.Layers, bundle.GlobalDocument.SchemaInfo.Layers);
        Detail::CopyOrAssignIfEmptyV2(bundle.Schema.ProvenanceKinds, bundle.GlobalDocument.SchemaInfo.ProvenanceKinds);

        for (const auto &studyPath : studyPaths) {
            bundle.StudyDocuments.push_back(LoadStudyOntologyDocumentV2(studyPath, bundle.Schema, bundle.Diagnostics));
        }

        const auto addNode = [&](const FOntologyNodeRecordV2 &node) {
            if (bundle.NodeTable.contains(node.Id)) {
                bundle.Diagnostics.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.bundle.duplicate_node_id",
                    node.Id,
                    "Bundle.Nodes",
                    "Duplicate stable node id across loaded documents; later entry skipped.",
                    node.SourceDocument);
                return;
            }
            bundle.NodeTable[node.Id] = node;
        };

        const auto addEdge = [&](const FOntologyEdgeRecordV2 &edge) {
            if (bundle.EdgeTable.contains(edge.Id)) {
                bundle.Diagnostics.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.bundle.duplicate_edge_id",
                    edge.Id,
                    "Bundle.Edges",
                    "Duplicate stable edge id across loaded documents; later entry skipped.",
                    edge.SourceDocument);
                return;
            }
            bundle.EdgeTable[edge.Id] = edge;
        };

        for (const auto &node : bundle.GlobalDocument.Nodes) addNode(node);
        for (const auto &edge : bundle.GlobalDocument.Edges) addEdge(edge);
        for (const auto &study : bundle.StudyDocuments) {
            for (const auto &node : study.Nodes) addNode(node);
            for (const auto &edge : study.Edges) addEdge(edge);
        }

        for (auto &[edgeId, edge] : bundle.EdgeTable) {
            edge.bFromResolved = bundle.NodeTable.contains(edge.From);
            edge.bToResolved = bundle.NodeTable.contains(edge.To);
            if (!edge.bFromResolved) {
                bundle.Diagnostics.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.edge.from_unresolved",
                    edge.Id,
                    "Bundle.Edges",
                    "Broken edge reference: missing source node '" + edge.From + "'.",
                    edge.SourceDocument);
            }
            if (!edge.bToResolved) {
                bundle.Diagnostics.Add(
                    ModelV2::EValidationSeverityV2::Error,
                    "ontology.edge.to_unresolved",
                    edge.Id,
                    "Bundle.Edges",
                    "Broken edge reference: missing target node '" + edge.To + "'.",
                    edge.SourceDocument);
            }

            const auto *fromNode = bundle.FindNode(edge.From);
            const auto *toNode = bundle.FindNode(edge.To);
            edge.bCrossScopeReference = fromNode != nullptr && toNode != nullptr &&
                fromNode->OwnershipScope != toNode->OwnershipScope;

            if (edge.bFromResolved) {
                bundle.OutgoingEdgesByNodeId[edge.From].push_back(edgeId);
            }
            if (edge.bToResolved) {
                bundle.IncomingEdgesByNodeId[edge.To].push_back(edgeId);
            }
        }

        for (auto &[nodeId, edgeIds] : bundle.OutgoingEdgesByNodeId) {
            (void) nodeId;
            std::sort(edgeIds.begin(), edgeIds.end());
        }
        for (auto &[nodeId, edgeIds] : bundle.IncomingEdgesByNodeId) {
            (void) nodeId;
            std::sort(edgeIds.begin(), edgeIds.end());
        }

        for (const auto &study : bundle.StudyDocuments) {
            if (!study.bLoaded) continue;

            if (!study.OntologyRef.Path.empty()) {
                const auto expectedGlobalName = globalPath.filename().string();
                const auto referencedName = std::filesystem::path(study.OntologyRef.Path).filename().string();
                if (!expectedGlobalName.empty() && !referencedName.empty() && referencedName != expectedGlobalName) {
                    bundle.Diagnostics.Add(
                        ModelV2::EValidationSeverityV2::Warning,
                        "ontology.study.ontology_ref_path_mismatch",
                        study.Study.Id,
                        "Study.OntologyRef",
                        "Study ontology_ref.path points to '" + study.OntologyRef.Path +
                            "' but the loaded global ontology is '" + expectedGlobalName + "'.",
                        study.SourceDocument);
                }
            }

            if (!study.OntologyRef.Version.empty() && !bundle.GlobalDocument.Meta.Version.empty() &&
                study.OntologyRef.Version != bundle.GlobalDocument.Meta.Version) {
                bundle.Diagnostics.Add(
                    ModelV2::EValidationSeverityV2::Warning,
                    "ontology.study.ontology_ref_version_mismatch",
                    study.Study.Id,
                    "Study.OntologyRef",
                    "Study ontology_ref.version does not match the loaded global ontology version.",
                    study.SourceDocument);
            }

            for (const auto &node : study.Nodes) {
                Detail::VisitStringLeavesV2(
                    json::value(node.Properties),
                    [&](const auto &candidate) {
                        Detail::ValidateKnownGlobalReferenceV2(
                            bundle,
                            candidate,
                            node.Id,
                            "Study.Node.Properties",
                            study.SourceDocument,
                            bundle.Diagnostics);
                    });
            }

            for (const auto &edge : study.Edges) {
                Detail::VisitStringLeavesV2(
                    json::value(edge.Properties),
                    [&](const auto &candidate) {
                        Detail::ValidateKnownGlobalReferenceV2(
                            bundle,
                            candidate,
                            edge.Id,
                            "Study.Edge.Properties",
                            study.SourceDocument,
                            bundle.Diagnostics);
                    });
            }

            Detail::VisitStringLeavesV2(
                study.ActivationSummary,
                [&](const auto &candidate) {
                    Detail::ValidateKnownGlobalReferenceV2(
                        bundle,
                        candidate,
                        study.Study.Id,
                        "Study.ActivationSummary",
                        study.SourceDocument,
                        bundle.Diagnostics);
                });

            Detail::VisitStringLeavesV2(
                study.RealizationFrontier,
                [&](const auto &candidate) {
                    Detail::ValidateKnownGlobalReferenceV2(
                        bundle,
                        candidate,
                        study.Study.Id,
                        "Study.RealizationFrontier",
                        study.SourceDocument,
                        bundle.Diagnostics);
                });
        }

        return bundle;
    }

    inline auto BuildOntologyGraphProjectionV2(const FOntologyGraphBundle &bundle,
                                               const Str &selectedStudyId,
                                               const FOntologyGraphFilterStateV2 &filters)
        -> FOntologyGraphProjection {
        FOntologyGraphProjection projection;
        projection.SelectedStudyId = selectedStudyId;
        projection.Filters = filters;
        projection.Diagnostics = bundle.Diagnostics;

        const auto *selectedStudy = bundle.FindStudyDocumentById(selectedStudyId);
        if (selectedStudy == nullptr && !bundle.StudyDocuments.empty()) {
            selectedStudy = &bundle.StudyDocuments.front();
            projection.SelectedStudyId = selectedStudy->Study.Id;
        }

        if (selectedStudy == nullptr) {
            return projection;
        }

        projection.SelectedStudyDocument = selectedStudy->SourceDocument;

        const auto studyStatusMap = Detail::BuildStudyStatusMapV2(bundle, *selectedStudy);
        const auto studyLocalNodeIds = Detail::ComputeStudyLocalNodeIdsV2(*selectedStudy);
        auto focusActiveNodeIds = studyLocalNodeIds;
        for (const auto &[nodeId, status] : studyStatusMap) {
            if (status != EOntologyActivationStatusV2::None) {
                focusActiveNodeIds.insert(nodeId);
            }
        }
        focusActiveNodeIds = Detail::ExpandIncidentNodesV2(
            bundle,
            focusActiveNodeIds,
            [&](const auto &edge) {
                return edge.SourceDocument == selectedStudy->SourceDocument ||
                    edge.SourceKind == EOntologyDocumentKindV2::GlobalOntology;
            });

        std::set<Str> blockedNodeIds;
        for (const auto &[nodeId, status] : studyStatusMap) {
            if (status == EOntologyActivationStatusV2::Blocked) blockedNodeIds.insert(nodeId);
        }
        if (!selectedStudy->Study.Id.empty()) blockedNodeIds.insert(selectedStudy->Study.Id);
        blockedNodeIds = Detail::ExpandIncidentNodesV2(
            bundle,
            blockedNodeIds,
            [](const auto &edge) {
                return edge.Type == "blocks" || edge.Type == "requires_for_descent";
            });

        std::set<Str> realizationPathNodeIds = studyLocalNodeIds;
        for (const auto &[nodeId, status] : studyStatusMap) {
            if (status == EOntologyActivationStatusV2::None) continue;
            const auto *node = bundle.FindNode(nodeId);
            if (node == nullptr) continue;
            if (node->Layer == "descent" || node->Layer == "realization" || node->Layer == "recipe" ||
                node->Layer == "artifact") {
                realizationPathNodeIds.insert(nodeId);
            }
        }
        realizationPathNodeIds = Detail::ExpandIncidentNodesV2(
            bundle,
            realizationPathNodeIds,
            [](const auto &edge) {
                return edge.Type == "can_descend_to" ||
                    edge.Type == "admits_realization" ||
                    edge.Type == "admits_solver" ||
                    edge.Type == "stepped_by" ||
                    edge.Type == "produces" ||
                    edge.Type == "discretized_by" ||
                    edge.Type == "requires_for_descent" ||
                    edge.Type == "activates";
            });

        const auto positions = Detail::ComputeGlobalNodePositionsV2(bundle, selectedStudy);
        std::set<Str> visibleNodeIds;

        auto passesBaseScope = [&](const FOntologyNodeRecordV2 &node) {
            if (filters.ScopeMode == EOntologyFilterScopeModeV2::GlobalOnly &&
                node.OwnershipScope != EOntologyOwnershipScopeV2::Global) {
                return false;
            }
            if (filters.ScopeMode == EOntologyFilterScopeModeV2::SelectedStudyOnly &&
                !studyLocalNodeIds.contains(node.Id)) {
                return false;
            }
            if (filters.bHideAmbientLayer && node.Layer == "ambient") return false;
            if (filters.bHideAbstractNodes && node.bAbstract) return false;
            return true;
        };

        auto passesFocusFilters = [&](const FOntologyNodeRecordV2 &node) {
            bool bVisible = true;
            if (filters.bFocusActiveReachableRegion) {
                bVisible = bVisible && focusActiveNodeIds.contains(node.Id);
            }
            if (filters.bShowBlockedRequirementsOnly) {
                bVisible = bVisible && blockedNodeIds.contains(node.Id);
            }
            if (filters.bShowRealizationRecipeArtifactPathOnly) {
                bVisible = bVisible && realizationPathNodeIds.contains(node.Id);
            }
            return bVisible;
        };

        for (const auto &[nodeId, node] : bundle.NodeTable) {
            if (!passesBaseScope(node)) continue;
            if (!passesFocusFilters(node)) continue;
            visibleNodeIds.insert(nodeId);

            const auto statusIt = studyStatusMap.find(nodeId);
            const auto status = statusIt == studyStatusMap.end()
                ? EOntologyActivationStatusV2::None
                : statusIt->second;
            const auto [summary, secondary] = Detail::BuildNodeSummaryV2(node, status);

            FOntologyProjectedNodeV2 projectedNode;
            projectedNode.NodeId = node.Id;
            projectedNode.Title = node.Title;
            projectedNode.Kind = node.Kind;
            projectedNode.Layer = node.Layer;
            projectedNode.bAbstract = node.bAbstract;
            projectedNode.OwnershipScope = node.OwnershipScope;
            projectedNode.SourceDocument = node.SourceDocument;
            projectedNode.ActivationStatus = status;
            projectedNode.bStudyRoot = node.Id == selectedStudy->Study.Id;
            projectedNode.bStudyOwned = node.OwnershipScope == EOntologyOwnershipScopeV2::StudyLocal;
            projectedNode.bInActiveRegion = focusActiveNodeIds.contains(node.Id);
            projectedNode.bInRealizationPath = realizationPathNodeIds.contains(node.Id);
            projectedNode.CompactId = Detail::CompactIdSuffixV2(node.Id);
            projectedNode.Summary = summary;
            projectedNode.SecondarySummary = secondary;
            if (const auto it = positions.find(node.Id); it != positions.end()) {
                projectedNode.Position = it->second;
            }
            projection.Nodes.push_back(std::move(projectedNode));
        }

        std::sort(projection.Nodes.begin(), projection.Nodes.end(), [&](const auto &lhs, const auto &rhs) {
            if (lhs.Layer != rhs.Layer) {
                const auto lhsOrder = Detail::LayerOrderV2(lhs.Layer);
                const auto rhsOrder = Detail::LayerOrderV2(rhs.Layer);
                if (lhsOrder != rhsOrder) return lhsOrder < rhsOrder;
            }
            if (lhs.Position.x != rhs.Position.x) return lhs.Position.x < rhs.Position.x;
            if (lhs.Position.y != rhs.Position.y) return lhs.Position.y > rhs.Position.y;
            return lhs.NodeId < rhs.NodeId;
        });

        for (const auto &[edgeId, edge] : bundle.EdgeTable) {
            const bool bStudyOwned = edge.SourceDocument == selectedStudy->SourceDocument;
            if (filters.ScopeMode == EOntologyFilterScopeModeV2::GlobalOnly && bStudyOwned) continue;
            if (filters.ScopeMode == EOntologyFilterScopeModeV2::SelectedStudyOnly && !bStudyOwned) continue;
            if (!visibleNodeIds.contains(edge.From) || !visibleNodeIds.contains(edge.To)) continue;

            auto status = Detail::ActivationStatusFromStringV2(Detail::JsonReadStringV2(edge.Properties, "status"));
            if (status == EOntologyActivationStatusV2::None) {
                const auto fromStatusIt = studyStatusMap.find(edge.From);
                const auto toStatusIt = studyStatusMap.find(edge.To);
                if (fromStatusIt != studyStatusMap.end()) {
                    status = Detail::MergeActivationStatusV2(status, fromStatusIt->second);
                }
                if (toStatusIt != studyStatusMap.end()) {
                    status = Detail::MergeActivationStatusV2(status, toStatusIt->second);
                }
            }

            FOntologyProjectedEdgeV2 projectedEdge;
            projectedEdge.EdgeId = edge.Id;
            projectedEdge.Type = edge.Type;
            projectedEdge.FromNodeId = edge.From;
            projectedEdge.ToNodeId = edge.To;
            projectedEdge.SourceDocument = edge.SourceDocument;
            projectedEdge.OwnershipScope = edge.OwnershipScope;
            projectedEdge.ActivationStatus = status;
            projectedEdge.bCrossScopeReference = edge.bCrossScopeReference;
            projectedEdge.bBrokenReference = !edge.bFromResolved || !edge.bToResolved;
            projectedEdge.bInActiveRegion = focusActiveNodeIds.contains(edge.From) || focusActiveNodeIds.contains(edge.To);
            projectedEdge.bInRealizationPath =
                realizationPathNodeIds.contains(edge.From) || realizationPathNodeIds.contains(edge.To);
            projectedEdge.bStudyOwned = bStudyOwned;
            projectedEdge.BadgeLabel = edge.Type;
            projectedEdge.Summary = Detail::BuildEdgeSummaryV2(edge, status);
            projection.Edges.push_back(std::move(projectedEdge));
            projection.OutgoingEdgesByNodeId[edge.From].push_back(edge.Id);
            projection.IncomingEdgesByNodeId[edge.To].push_back(edge.Id);
        }

        std::sort(projection.Edges.begin(), projection.Edges.end(), [&](const auto &lhs, const auto &rhs) {
            if (lhs.bStudyOwned != rhs.bStudyOwned) return lhs.bStudyOwned > rhs.bStudyOwned;
            if (lhs.Type != rhs.Type) return lhs.Type < rhs.Type;
            return lhs.EdgeId < rhs.EdgeId;
        });

        for (auto &[nodeId, edgeIds] : projection.OutgoingEdgesByNodeId) {
            (void) nodeId;
            std::sort(edgeIds.begin(), edgeIds.end());
        }
        for (auto &[nodeId, edgeIds] : projection.IncomingEdgesByNodeId) {
            (void) nodeId;
            std::sort(edgeIds.begin(), edgeIds.end());
        }

        return projection;
    }

} // namespace Slab::Core::Ontology::V2

#endif // STUDIOSLAB_ONTOLOGY_GRAPH_V2_H
