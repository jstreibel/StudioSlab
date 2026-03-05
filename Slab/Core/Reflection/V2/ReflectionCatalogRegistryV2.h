#ifndef STUDIOSLAB_REFLECTION_CATALOG_REGISTRY_V2_H
#define STUDIOSLAB_REFLECTION_CATALOG_REGISTRY_V2_H

#include "ReflectionInvokeV2.h"
#include "ReflectionTypesV2.h"

#include <algorithm>
#include <functional>
#include <map>
#include <optional>
#include <ranges>
#include <set>

namespace Slab::Core::Reflection::V2 {

    struct FReflectionCatalogSourceBindingV2 {
        Str SourceId;
        Str DisplayName;
        Str Description;

        std::function<void()> Refresh;
        std::function<const FReflectionCatalogV2 &()> GetCatalog;
        std::function<FOperationResultV2(const Str &interfaceId,
                                         const Str &operationId,
                                         const FValueMapV2 &inputs,
                                         const FInvocationContextV2 &context)> Invoke;
        std::function<std::optional<FReflectionValueV2>(const Str &interfaceId,
                                                        const Str &parameterId)> EncodeCurrentParameterValue;
    };

    struct FReflectionCatalogSourceSummaryV2 {
        Str SourceId;
        Str DisplayName;
        Str Description;
        std::size_t InterfaceCount = 0;
    };

    class FReflectionCatalogRegistryV2 {
        std::map<Str, FReflectionCatalogSourceBindingV2> SourcesById;
        std::map<Str, Str> InterfaceSourceById;

        auto RebuildInterfaceSourceIndex() -> void {
            InterfaceSourceById.clear();
            for (const auto &[sourceId, source] : SourcesById) {
                if (!source.GetCatalog) continue;

                const auto &catalog = source.GetCatalog();
                for (const auto &interfaceSchema : catalog.Interfaces) {
                    if (interfaceSchema.InterfaceId.empty()) continue;
                    InterfaceSourceById.try_emplace(interfaceSchema.InterfaceId, sourceId);
                }
            }
        }

        [[nodiscard]] auto FindSourceBinding(const Str &sourceId) const -> const FReflectionCatalogSourceBindingV2 * {
            const auto it = SourcesById.find(sourceId);
            if (it == SourcesById.end()) return nullptr;
            return &it->second;
        }

        [[nodiscard]] auto FindSourceBindingForInterface(const Str &interfaceId) const
            -> const FReflectionCatalogSourceBindingV2 * {
            const auto sourceIt = InterfaceSourceById.find(interfaceId);
            if (sourceIt == InterfaceSourceById.end()) return nullptr;
            return FindSourceBinding(sourceIt->second);
        }

    public:
        static auto Get() -> FReflectionCatalogRegistryV2 & {
            static FReflectionCatalogRegistryV2 registry;
            return registry;
        }

        auto RegisterSource(FReflectionCatalogSourceBindingV2 source) -> void {
            if (source.SourceId.empty()) return;
            SourcesById[source.SourceId] = std::move(source);
            RebuildInterfaceSourceIndex();
        }

        auto UnregisterSource(const Str &sourceId) -> void {
            if (sourceId.empty()) return;
            SourcesById.erase(sourceId);
            RebuildInterfaceSourceIndex();
        }

        auto RefreshAll() -> void {
            for (auto &[sourceId, source] : SourcesById) {
                (void) sourceId;
                if (source.Refresh) source.Refresh();
            }
            RebuildInterfaceSourceIndex();
        }

        auto RefreshSource(const Str &sourceId) -> bool {
            auto *source = const_cast<FReflectionCatalogSourceBindingV2 *>(FindSourceBinding(sourceId));
            if (source == nullptr) return false;
            if (source->Refresh) source->Refresh();
            RebuildInterfaceSourceIndex();
            return true;
        }

        [[nodiscard]] auto GetCatalog(const Str &sourceId) const -> const FReflectionCatalogV2 * {
            const auto *source = FindSourceBinding(sourceId);
            if (source == nullptr || !source->GetCatalog) return nullptr;
            return &source->GetCatalog();
        }

        [[nodiscard]] auto ListSources() const -> Vector<FReflectionCatalogSourceSummaryV2> {
            Vector<FReflectionCatalogSourceSummaryV2> summaries;
            summaries.reserve(SourcesById.size());

            for (const auto &[sourceId, source] : SourcesById) {
                std::size_t interfaceCount = 0;
                if (source.GetCatalog) interfaceCount = source.GetCatalog().Interfaces.size();

                FReflectionCatalogSourceSummaryV2 summary;
                summary.SourceId = sourceId;
                summary.DisplayName = source.DisplayName.empty() ? sourceId : source.DisplayName;
                summary.Description = source.Description;
                summary.InterfaceCount = interfaceCount;
                summaries.push_back(std::move(summary));
            }

            std::sort(summaries.begin(), summaries.end(), [](const auto &lhs, const auto &rhs) {
                if (lhs.DisplayName == rhs.DisplayName) return lhs.SourceId < rhs.SourceId;
                return lhs.DisplayName < rhs.DisplayName;
            });

            return summaries;
        }

        [[nodiscard]] auto BuildMergedCatalog(const bool annotateSources = true) const -> FReflectionCatalogV2 {
            FReflectionCatalogV2 merged;
            std::set<Str> seenInterfaceIds;

            for (const auto &[sourceId, source] : SourcesById) {
                if (!source.GetCatalog) continue;
                const auto &catalog = source.GetCatalog();

                for (const auto &interfaceSchema : catalog.Interfaces) {
                    if (interfaceSchema.InterfaceId.empty()) continue;
                    if (seenInterfaceIds.contains(interfaceSchema.InterfaceId)) continue;
                    seenInterfaceIds.insert(interfaceSchema.InterfaceId);

                    auto mergedSchema = interfaceSchema;
                    if (annotateSources) {
                        const auto sourceTag = "catalog.source_id:" + sourceId;
                        if (!std::ranges::contains(mergedSchema.Tags, sourceTag)) {
                            mergedSchema.Tags.push_back(sourceTag);
                        }

                        if (!source.DisplayName.empty()) {
                            const auto sourceNameTag = "catalog.source_name:" + source.DisplayName;
                            if (!std::ranges::contains(mergedSchema.Tags, sourceNameTag)) {
                                mergedSchema.Tags.push_back(sourceNameTag);
                            }
                        }
                    }

                    merged.Interfaces.push_back(std::move(mergedSchema));
                }
            }

            std::sort(merged.Interfaces.begin(), merged.Interfaces.end(), [](const auto &lhs, const auto &rhs) {
                if (lhs.DisplayName == rhs.DisplayName) return lhs.InterfaceId < rhs.InterfaceId;
                return lhs.DisplayName < rhs.DisplayName;
            });

            return merged;
        }

        [[nodiscard]] auto FindSourceForInterface(const Str &interfaceId) const
            -> std::optional<FReflectionCatalogSourceSummaryV2> {
            if (interfaceId.empty()) return std::nullopt;

            const auto sourceIt = InterfaceSourceById.find(interfaceId);
            if (sourceIt == InterfaceSourceById.end()) return std::nullopt;

            const auto *source = FindSourceBinding(sourceIt->second);
            if (source == nullptr) return std::nullopt;

            FReflectionCatalogSourceSummaryV2 summary;
            summary.SourceId = source->SourceId;
            summary.DisplayName = source->DisplayName.empty() ? source->SourceId : source->DisplayName;
            summary.Description = source->Description;
            if (source->GetCatalog) summary.InterfaceCount = source->GetCatalog().Interfaces.size();

            return summary;
        }

        auto Invoke(const Str &interfaceId,
                    const Str &operationId,
                    const FValueMapV2 &inputs,
                    const FInvocationContextV2 &context) const -> FOperationResultV2 {
            const auto *source = FindSourceBindingForInterface(interfaceId);
            if (source == nullptr || !source->Invoke) {
                return FOperationResultV2::Error(
                    "reflection.interface.not_found",
                    "Interface '" + interfaceId + "' is not registered in reflection catalog registry.");
            }

            return source->Invoke(interfaceId, operationId, inputs, context);
        }

        [[nodiscard]] auto EncodeCurrentParameterValue(const Str &interfaceId, const Str &parameterId) const
            -> std::optional<FReflectionValueV2> {
            const auto *source = FindSourceBindingForInterface(interfaceId);
            if (source == nullptr || !source->EncodeCurrentParameterValue) return std::nullopt;
            return source->EncodeCurrentParameterValue(interfaceId, parameterId);
        }
    };

} // namespace Slab::Core::Reflection::V2

#endif // STUDIOSLAB_REFLECTION_CATALOG_REGISTRY_V2_H
