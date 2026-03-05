#include "LegacyPlotReflectionAdapterV2.h"

#include "Core/Reflection/V2/ReflectionCodecsV2.h"

#include <algorithm>

namespace Slab::Graphics::Plot2D::V2 {

    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    namespace {

        auto MakeParameterValue(Str typeId, Str encoded) -> ReflectionV2::FReflectionValueV2 {
            if (typeId.empty()) typeId = ReflectionV2::CTypeIdScalarString;
            return ReflectionV2::MakeEncodedValue(std::move(typeId), std::move(encoded));
        }

        auto MakeFloatBinding(const Str &parameterId,
                              const Str &displayName,
                              const Str &description,
                              const std::function<DevFloat()> &readCurrent,
                              const std::function<void(DevFloat)> &writeLive) -> FPlotReflectionParameterBindingV2 {
            FPlotReflectionParameterBindingV2 binding;
            binding.Schema.ParameterId = parameterId;
            binding.Schema.DisplayName = displayName;
            binding.Schema.Description = description;
            binding.Schema.TypeId = ReflectionV2::CTypeIdScalarFloat64;
            binding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
            binding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
            binding.ReadCurrent = [readCurrent] {
                return ReflectionV2::MakeFloatValue(readCurrent());
            };
            binding.WriteLiveValue = [parameterId, writeLive](const ReflectionV2::FReflectionValueV2 &value) {
                try {
                    writeLive(std::stod(value.Encoded));
                } catch (const std::exception &e) {
                    return ReflectionV2::FOperationResultV2::Error(
                        "plot2d.legacy.parameter.parse_float",
                        "Could not parse float for parameter '" + parameterId + "': " + e.what());
                }

                return ReflectionV2::FOperationResultV2::Ok();
            };
            return binding;
        }

    } // namespace

    auto FLegacyPlotReflectionAdapterV2::BuildWindowInterfaceId(const Slab::Graphics::FPlot2DWindow &window) -> Str {
        return "legacy.plot.window." + window.GetStableWindowIdV2();
    }

    auto FLegacyPlotReflectionAdapterV2::BuildArtistInterfaceId(const Slab::Graphics::FPlot2DWindow &window,
                                                                const Slab::Graphics::FArtist &artist) -> Str {
        Str artistId = artist.GetArtistId();
        if (artistId.empty()) {
            artistId = "artist";
            artist.TryGetV2ParameterValue("artist_id", artistId);
        }

        return "legacy.plot.artist." + window.GetStableWindowIdV2() + "." + artistId;
    }

    auto FLegacyPlotReflectionAdapterV2::DescribeWindow(Slab::Graphics::FPlot2DWindow &window)
        -> FPlotEntityReflectionDescriptorV2 {
        FPlotEntityReflectionDescriptorV2 descriptor;
        descriptor.InterfaceId = BuildWindowInterfaceId(window);
        descriptor.DisplayName = window.GetWindowTitle().empty() ? window.GetStableWindowIdV2() : window.GetWindowTitle();
        descriptor.Description = "Legacy Plot2D window reflection bridge surface.";
        descriptor.Tags = {"plot2d", "legacy", "window"};

        descriptor.Parameters.push_back(FPlotReflectionParameterBindingV2{
            .Schema = ReflectionV2::FParameterSchemaV2{
                .ParameterId = "window_id",
                .DisplayName = "Window Id",
                .Description = "Stable legacy plot window id.",
                .TypeId = ReflectionV2::CTypeIdScalarString,
                .Mutability = ReflectionV2::EParameterMutability::Const,
                .Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed
            },
            .ReadCurrent = [&window] {
                return ReflectionV2::MakeStringValue(window.GetStableWindowIdV2());
            }
        });

        descriptor.Parameters.push_back(FPlotReflectionParameterBindingV2{
            .Schema = ReflectionV2::FParameterSchemaV2{
                .ParameterId = "title",
                .DisplayName = "Title",
                .Description = "Legacy plot window title.",
                .TypeId = ReflectionV2::CTypeIdScalarString,
                .Mutability = ReflectionV2::EParameterMutability::Const,
                .Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed
            },
            .ReadCurrent = [&window] {
                return ReflectionV2::MakeStringValue(window.GetWindowTitle());
            }
        });

        descriptor.Parameters.push_back(FPlotReflectionParameterBindingV2{
            .Schema = ReflectionV2::FParameterSchemaV2{
                .ParameterId = "artist_count",
                .DisplayName = "Artist Count",
                .Description = "Number of artists currently attached to this window.",
                .TypeId = ReflectionV2::CTypeIdScalarInt32,
                .Mutability = ReflectionV2::EParameterMutability::Const,
                .Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed
            },
            .ReadCurrent = [&window] {
                return ReflectionV2::MakeIntValue(static_cast<int>(window.GetArtists().size()));
            }
        });

        descriptor.Parameters.push_back(MakeFloatBinding(
            "x_min",
            "X Min",
            "Left boundary of plot region.",
            [&window] { return window.GetRegion().getRect().xMin; },
            [&window](const DevFloat value) {
                auto region = window.GetRegion().getRect();
                region.xMin = value;
                auto &plotRegion = window.GetRegion();
                plotRegion = region;
            }));

        descriptor.Parameters.push_back(MakeFloatBinding(
            "x_max",
            "X Max",
            "Right boundary of plot region.",
            [&window] { return window.GetRegion().getRect().xMax; },
            [&window](const DevFloat value) {
                auto region = window.GetRegion().getRect();
                region.xMax = value;
                auto &plotRegion = window.GetRegion();
                plotRegion = region;
            }));

        descriptor.Parameters.push_back(MakeFloatBinding(
            "y_min",
            "Y Min",
            "Bottom boundary of plot region.",
            [&window] { return window.GetRegion().getRect().yMin; },
            [&window](const DevFloat value) {
                auto region = window.GetRegion().getRect();
                region.yMin = value;
                auto &plotRegion = window.GetRegion();
                plotRegion = region;
            }));

        descriptor.Parameters.push_back(MakeFloatBinding(
            "y_max",
            "Y Max",
            "Top boundary of plot region.",
            [&window] { return window.GetRegion().getRect().yMax; },
            [&window](const DevFloat value) {
                auto region = window.GetRegion().getRect();
                region.yMax = value;
                auto &plotRegion = window.GetRegion();
                plotRegion = region;
            }));

        descriptor.Parameters.push_back(FPlotReflectionParameterBindingV2{
            .Schema = ReflectionV2::FParameterSchemaV2{
                .ParameterId = "auto_fit_ranges",
                .DisplayName = "Auto Fit Ranges",
                .Description = "Automatically review graph ranges on draw.",
                .TypeId = ReflectionV2::CTypeIdScalarBool,
                .Mutability = ReflectionV2::EParameterMutability::RuntimeMutable,
                .Exposure = ReflectionV2::EParameterExposure::WritableExposed
            },
            .ReadCurrent = [&window] {
                return ReflectionV2::MakeBoolValue(window.GetAutoReviewGraphRanges());
            },
            .WriteLiveValue = [&window](const ReflectionV2::FReflectionValueV2 &value) {
                bool parsed = false;
                if (!ReflectionV2::ParseBoolValue(value.Encoded, parsed)) {
                    return ReflectionV2::FOperationResultV2::Error(
                        "plot2d.legacy.parameter.parse_bool",
                        "Could not parse bool value for parameter 'auto_fit_ranges'.");
                }

                window.SetAutoReviewGraphRanges(parsed);
                return ReflectionV2::FOperationResultV2::Ok();
            }
        });

        ReflectionV2::FOperationSchemaV2 fitToArtists;
        fitToArtists.OperationId = "command.window.fit_to_artists";
        fitToArtists.DisplayName = "Fit To Artists";
        fitToArtists.Description = "Review legacy graph ranges using artists that affect ranges.";
        fitToArtists.Kind = ReflectionV2::EOperationKind::Command;
        fitToArtists.ThreadAffinity = ReflectionV2::EThreadAffinity::UI;
        fitToArtists.RunStatePolicy = ReflectionV2::ERunStatePolicy::Any;
        fitToArtists.SideEffectClass = ReflectionV2::ESideEffectClass::LocalState;
        fitToArtists.InvokeHandler = [&window](const ReflectionV2::FValueMapV2 &, const FInvocationContextV2 &) {
            const bool hasRangeContributor = std::any_of(
                window.GetArtists().begin(),
                window.GetArtists().end(),
                [](const auto &slot) {
                    return slot.second != nullptr && slot.second->AffectsGraphRanges();
                });

            window.ReviewGraphRanges();

            const auto region = window.GetRegion().getRect();
            ReflectionV2::FValueMapV2 output;
            output["interface_id"] = ReflectionV2::MakeStringValue(BuildWindowInterfaceId(window));
            output["fitted"] = ReflectionV2::MakeBoolValue(hasRangeContributor);
            output["x_min"] = ReflectionV2::MakeFloatValue(region.xMin);
            output["x_max"] = ReflectionV2::MakeFloatValue(region.xMax);
            output["y_min"] = ReflectionV2::MakeFloatValue(region.yMin);
            output["y_max"] = ReflectionV2::MakeFloatValue(region.yMax);
            return ReflectionV2::FOperationResultV2::Ok(std::move(output));
        };
        descriptor.Operations.push_back(std::move(fitToArtists));

        ReflectionV2::FOperationSchemaV2 listArtists;
        listArtists.OperationId = "query.window.list_artists";
        listArtists.DisplayName = "List Artists";
        listArtists.Description = "List artists currently registered in this legacy plot window.";
        listArtists.Kind = ReflectionV2::EOperationKind::Query;
        listArtists.ThreadAffinity = ReflectionV2::EThreadAffinity::Any;
        listArtists.RunStatePolicy = ReflectionV2::ERunStatePolicy::Any;
        listArtists.SideEffectClass = ReflectionV2::ESideEffectClass::None;
        listArtists.InvokeHandler = [&window](const ReflectionV2::FValueMapV2 &, const FInvocationContextV2 &) {
            ReflectionV2::FValueMapV2 output;
            output["interface_id"] = ReflectionV2::MakeStringValue(BuildWindowInterfaceId(window));
            output["artist_count"] = ReflectionV2::MakeIntValue(static_cast<int>(window.GetArtists().size()));

            int index = 0;
            for (const auto &[zOrder, artist] : window.GetArtists()) {
                if (artist == nullptr) continue;

                const auto prefix = "artist." + ToStr(index) + ".";
                output[prefix + "id"] = ReflectionV2::MakeStringValue(artist->GetArtistId());
                output[prefix + "label"] = ReflectionV2::MakeStringValue(artist->GetLabel());
                output[prefix + "z_order"] = ReflectionV2::MakeIntValue(static_cast<int>(zOrder));
                output[prefix + "visible"] = ReflectionV2::MakeBoolValue(artist->IsVisible());
                ++index;
            }

            return ReflectionV2::FOperationResultV2::Ok(std::move(output));
        };
        descriptor.Operations.push_back(std::move(listArtists));

        return descriptor;
    }

    auto FLegacyPlotReflectionAdapterV2::DescribeArtist(Slab::Graphics::FPlot2DWindow &window,
                                                        Slab::Graphics::FPlot2DWindow::zOrder_t zOrder,
                                                        const Slab::Graphics::FArtist_ptr &artist)
        -> FPlotEntityReflectionDescriptorV2 {
        FPlotEntityReflectionDescriptorV2 descriptor;
        if (artist == nullptr) return descriptor;

        descriptor.InterfaceId = BuildArtistInterfaceId(window, *artist);
        descriptor.DisplayName = artist->GetLabel().empty() ? artist->GetArtistId() : artist->GetLabel();
        descriptor.Description = "Legacy Plot2D artist reflection bridge surface.";
        descriptor.Tags = {"plot2d", "legacy", "artist"};

        const auto legacyParams = artist->DescribeV2Parameters();
        descriptor.Parameters.reserve(legacyParams.size() + 2);

        for (const auto &legacyParam : legacyParams) {
            FPlotReflectionParameterBindingV2 binding;
            binding.Schema.ParameterId = legacyParam.ParameterId;
            binding.Schema.DisplayName = legacyParam.DisplayName;
            binding.Schema.Description = legacyParam.Description;
            binding.Schema.TypeId = legacyParam.TypeId.empty() ? ReflectionV2::CTypeIdScalarString : legacyParam.TypeId;
            binding.Schema.Mutability = legacyParam.bWritable
                ? (legacyParam.bRestartRequired
                    ? ReflectionV2::EParameterMutability::RestartRequired
                    : ReflectionV2::EParameterMutability::RuntimeMutable)
                : ReflectionV2::EParameterMutability::Const;
            binding.Schema.Exposure = legacyParam.bWritable
                ? ReflectionV2::EParameterExposure::WritableExposed
                : ReflectionV2::EParameterExposure::ReadOnlyExposed;

            auto artistHandle = artist;
            binding.ReadCurrent = [artistHandle, legacyParam] {
                Str encoded;
                if (!artistHandle->TryGetV2ParameterValue(legacyParam.ParameterId, encoded)) {
                    encoded.clear();
                }

                const auto typeId = legacyParam.TypeId.empty()
                    ? ReflectionV2::CTypeIdScalarString
                    : legacyParam.TypeId;
                return MakeParameterValue(typeId, encoded);
            };

            if (legacyParam.bWritable) {
                binding.WriteLiveValue = [artistHandle, legacyParam](const ReflectionV2::FReflectionValueV2 &value) {
                    Str errorMessage;
                    if (!artistHandle->TrySetV2ParameterValue(legacyParam.ParameterId, value.Encoded, &errorMessage)) {
                        if (errorMessage.empty()) {
                            errorMessage = "Failed to write legacy artist parameter '" + legacyParam.ParameterId + "'.";
                        }

                        return ReflectionV2::FOperationResultV2::Error(
                            "plot2d.legacy.artist.parameter_write_failed",
                            std::move(errorMessage));
                    }

                    return ReflectionV2::FOperationResultV2::Ok();
                };
            }

            descriptor.Parameters.push_back(std::move(binding));
        }

        descriptor.Parameters.push_back(FPlotReflectionParameterBindingV2{
            .Schema = ReflectionV2::FParameterSchemaV2{
                .ParameterId = "parent_window_id",
                .DisplayName = "Parent Window Id",
                .Description = "Stable legacy window id that owns this artist.",
                .TypeId = ReflectionV2::CTypeIdScalarString,
                .Mutability = ReflectionV2::EParameterMutability::Const,
                .Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed
            },
            .ReadCurrent = [&window] {
                return ReflectionV2::MakeStringValue(window.GetStableWindowIdV2());
            }
        });

        auto artistHandle = artist;
        descriptor.Parameters.push_back(FPlotReflectionParameterBindingV2{
            .Schema = ReflectionV2::FParameterSchemaV2{
                .ParameterId = "z_order",
                .DisplayName = "Z Order",
                .Description = "Artist layering order inside this window.",
                .TypeId = ReflectionV2::CTypeIdScalarInt32,
                .Mutability = ReflectionV2::EParameterMutability::RuntimeMutable,
                .Exposure = ReflectionV2::EParameterExposure::WritableExposed
            },
            .ReadCurrent = [&window, artistHandle, zOrder] {
                Slab::Graphics::FPlot2DWindow::zOrder_t currentZ = zOrder;
                (void) window.TryGetArtistZOrder(artistHandle, currentZ);
                return ReflectionV2::MakeIntValue(static_cast<int>(currentZ));
            },
            .WriteLiveValue = [&window, artistHandle](const ReflectionV2::FReflectionValueV2 &value) {
                int parsed = 0;
                try {
                    parsed = std::stoi(value.Encoded);
                } catch (const std::exception &e) {
                    return ReflectionV2::FOperationResultV2::Error(
                        "plot2d.legacy.artist.parse_z_order",
                        "Could not parse integer for parameter 'z_order': " + Str(e.what()));
                }

                if (!window.SetArtistZOrder(artistHandle, parsed)) {
                    return ReflectionV2::FOperationResultV2::Error(
                        "plot2d.legacy.artist.z_order_set_failed",
                        "Could not set z-order for legacy artist '" + artistHandle->GetArtistId() + "'.");
                }

                return ReflectionV2::FOperationResultV2::Ok();
            }
        });

        return descriptor;
    }

} // namespace Slab::Graphics::Plot2D::V2
