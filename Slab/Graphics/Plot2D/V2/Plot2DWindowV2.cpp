#include "Plot2DWindowV2.h"

#include "Core/Reflection/V2/ReflectionCodecsV2.h"
#include "Graphics/Utils.h"

#include <algorithm>
#include <cctype>

namespace Slab::Graphics::Plot2D::V2 {

    namespace ReflectionV2 = Slab::Core::Reflection::V2;

    std::mutex FPlot2DWindowV2::RegistryMutex;
    std::map<Str, FPlot2DWindowV2 *> FPlot2DWindowV2::Registry = {};
    UIntBig FPlot2DWindowV2::WindowCounter = 0;

    auto FPlot2DWindowV2::NormalizeToken(Str raw) -> Str {
        for (char &ch : raw) {
            if (std::isalnum(static_cast<unsigned char>(ch))) {
                ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            } else {
                ch = '_';
            }
        }

        Str out;
        out.reserve(raw.size());

        bool lastUnderscore = false;
        for (const char ch : raw) {
            const bool isUnderscore = ch == '_';
            if (isUnderscore && lastUnderscore) continue;

            out.push_back(ch);
            lastUnderscore = isUnderscore;
        }

        while (!out.empty() && out.front() == '_') out.erase(out.begin());
        while (!out.empty() && out.back() == '_') out.pop_back();

        if (out.empty()) out = "plot_window";
        return out;
    }

    auto FPlot2DWindowV2::MakeUniqueWindowId(const Str &titleToken) -> Str {
        return titleToken + "_" + ToStr(++WindowCounter);
    }

    auto FPlot2DWindowV2::MakeUniqueArtistId(Str desiredId) const -> Str {
        desiredId = NormalizeToken(std::move(desiredId));
        if (desiredId.empty()) desiredId = "artist";

        auto exists = [this](const Str &candidate) {
            return std::any_of(Artists.begin(), Artists.end(), [&](const FArtistSlotV2 &slot) {
                return slot.Artist != nullptr && slot.Artist->GetArtistId() == candidate;
            });
        };

        if (!exists(desiredId)) return desiredId;

        int suffix = 2;
        while (true) {
            const auto candidate = desiredId + "_" + ToStr(suffix);
            if (!exists(candidate)) return candidate;
            ++suffix;
        }
    }

    FPlot2DWindowV2::FPlot2DWindowV2(Str title, RectR region, RectI viewport)
    : WindowId(MakeUniqueWindowId(NormalizeToken(title)))
    , Title(std::move(title))
    , Region(region)
    , Viewport(viewport) {
        std::lock_guard lock(RegistryMutex);
        Registry[WindowId] = this;
    }

    FPlot2DWindowV2::~FPlot2DWindowV2() {
        std::lock_guard lock(RegistryMutex);
        const auto it = Registry.find(WindowId);
        if (it != Registry.end() && it->second == this) {
            Registry.erase(it);
        }
    }

    auto FPlot2DWindowV2::GetLiveWindows() -> Vector<FPlot2DWindowV2 *> {
        std::lock_guard lock(RegistryMutex);

        Vector<FPlot2DWindowV2 *> windows;
        windows.reserve(Registry.size());

        for (const auto &[id, window] : Registry) {
            (void) id;
            if (window == nullptr) continue;
            windows.push_back(window);
        }

        return windows;
    }

    auto FPlot2DWindowV2::AddArtist(FPlotArtistV2_ptr artist, const int zOrder) -> void {
        if (artist == nullptr) return;

        const auto existing = std::find_if(Artists.begin(), Artists.end(), [&](const FArtistSlotV2 &slot) {
            return slot.Artist == artist;
        });
        if (existing != Artists.end()) {
            existing->ZOrder = zOrder;
            return;
        }

        Str desiredId = artist->GetArtistId();
        if (desiredId.empty()) desiredId = artist->GetArtistTypeId();
        desiredId = MakeUniqueArtistId(std::move(desiredId));

        artist->SetArtistId(desiredId);
        artist->SetParentWindowId(WindowId);

        Artists.push_back({zOrder, std::move(artist)});
    }

    auto FPlot2DWindowV2::RemoveArtist(const FPlotArtistV2_ptr &artist) -> bool {
        if (artist == nullptr) return false;

        const auto previousSize = Artists.size();
        std::erase_if(Artists, [&](const FArtistSlotV2 &slot) {
            return slot.Artist == artist;
        });

        return previousSize != Artists.size();
    }

    auto FPlot2DWindowV2::ClearArtists() -> void {
        Artists.clear();
    }

    auto FPlot2DWindowV2::GetArtists() const -> const Vector<FArtistSlotV2> & {
        return Artists;
    }

    auto FPlot2DWindowV2::GetArtistsInDrawOrder() const -> Vector<FArtistSlotV2> {
        auto ordered = Artists;
        std::stable_sort(ordered.begin(), ordered.end(), [](const FArtistSlotV2 &lhs, const FArtistSlotV2 &rhs) {
            return lhs.ZOrder < rhs.ZOrder;
        });

        return ordered;
    }

    auto FPlot2DWindowV2::FindArtistById(const Str &artistId) const -> FPlotArtistV2_ptr {
        const auto it = std::find_if(Artists.begin(), Artists.end(), [&](const FArtistSlotV2 &slot) {
            return slot.Artist != nullptr && slot.Artist->GetArtistId() == artistId;
        });

        if (it == Artists.end()) return nullptr;
        return it->Artist;
    }

    auto FPlot2DWindowV2::FitRegionToArtists(const DevFloat paddingFraction) -> bool {
        bool haveBounds = false;

        RectR merged{};

        for (const auto &slot : Artists) {
            if (slot.Artist == nullptr || !slot.Artist->ShouldAffectGraphRanges()) continue;

            const auto bounds = slot.Artist->ComputePlotBounds();
            if (!bounds.has_value()) continue;

            if (!haveBounds) {
                merged = bounds.value();
                haveBounds = true;
                continue;
            }

            merged.xMin = std::min(merged.xMin, bounds->xMin);
            merged.xMax = std::max(merged.xMax, bounds->xMax);
            merged.yMin = std::min(merged.yMin, bounds->yMin);
            merged.yMax = std::max(merged.yMax, bounds->yMax);
        }

        if (!haveBounds) return false;

        auto width = merged.GetWidth();
        auto height = merged.GetHeight();

        if (std::abs(width) < 1.0e-9) {
            width = 1.0;
            merged.xMin -= 0.5;
            merged.xMax += 0.5;
        }

        if (std::abs(height) < 1.0e-9) {
            height = 1.0;
            merged.yMin -= 0.5;
            merged.yMax += 0.5;
        }

        const auto padding = std::max<DevFloat>(0.0, paddingFraction);
        const auto padX = padding * width;
        const auto padY = padding * height;

        Region = {
            merged.xMin - padX,
            merged.xMax + padX,
            merged.yMin - padY,
            merged.yMax + padY
        };

        return true;
    }

    auto FPlot2DWindowV2::BuildFrameContext() const -> FPlotFrameContextV2 {
        return {
            .PlotRegion = Region,
            .Viewport = Viewport,
            .WindowId = WindowId,
            .Title = Title
        };
    }

    auto FPlot2DWindowV2::BuildDrawList() const -> FPlotDrawListV2 {
        FPlotDrawListV2 drawList;
        const auto frame = BuildFrameContext();

        for (const auto &slot : GetArtistsInDrawOrder()) {
            if (slot.Artist == nullptr || !slot.Artist->IsVisible()) continue;
            slot.Artist->EmitDrawCommands(frame, drawList);
        }

        return drawList;
    }

    auto FPlot2DWindowV2::Render(IPlotRenderBackendV2 &backend) const -> bool {
        if (bAutoFitRanges) {
            auto *self = const_cast<FPlot2DWindowV2 *>(this);
            self->FitRegionToArtists();
        }

        const auto drawList = BuildDrawList();
        const auto frame = BuildFrameContext();
        return backend.Render(frame, drawList);
    }

    auto FPlot2DWindowV2::ViewportToPlotCoord(const FPoint2D &viewportCoord) const -> FPoint2D {
        if (Viewport.GetWidth() <= 0 || Viewport.GetHeight() <= 0) return {};
        return Slab::Graphics::FromViewportToSpaceCoord(viewportCoord, Region, Viewport);
    }

    auto FPlot2DWindowV2::HitTestArtists(const FPoint2D &plotPosition,
                                         const FPoint2D &viewportPosition) const
        -> std::optional<FPlotArtistHitResultV2> {
        const auto frame = BuildFrameContext();
        auto ordered = GetArtistsInDrawOrder();

        for (auto it = ordered.rbegin(); it != ordered.rend(); ++it) {
            if (it->Artist == nullptr || !it->Artist->IsVisible()) continue;

            const auto hit = it->Artist->HitTest(frame, plotPosition, viewportPosition);
            if (!hit.has_value()) continue;

            return FPlotArtistHitResultV2{
                .Artist = it->Artist,
                .Target = *hit
            };
        }

        return std::nullopt;
    }

    auto FPlot2DWindowV2::DispatchPointerEvent(const FPlotPointerEventV2 &event) -> bool {
        const auto frame = BuildFrameContext();
        auto ordered = GetArtistsInDrawOrder();

        for (auto it = ordered.rbegin(); it != ordered.rend(); ++it) {
            if (it->Artist == nullptr || !it->Artist->IsVisible()) continue;
            if (it->Artist->HandlePointerEvent(frame, event)) return true;
        }

        return false;
    }

    auto FPlot2DWindowV2::DispatchKeyboardEvent(const FPlotKeyboardEventV2 &event) -> bool {
        const auto frame = BuildFrameContext();
        auto ordered = GetArtistsInDrawOrder();

        for (auto it = ordered.rbegin(); it != ordered.rend(); ++it) {
            if (it->Artist == nullptr || !it->Artist->IsVisible()) continue;
            if (it->Artist->HandleKeyboardEvent(frame, event)) return true;
        }

        return false;
    }

    auto FPlot2DWindowV2::SetWindowId(Str windowId) -> void {
        windowId = NormalizeToken(std::move(windowId));
        if (windowId.empty()) return;

        std::lock_guard lock(RegistryMutex);

        Registry.erase(WindowId);
        WindowId = std::move(windowId);
        Registry[WindowId] = this;

        for (const auto &slot : Artists) {
            if (slot.Artist == nullptr) continue;
            slot.Artist->SetParentWindowId(WindowId);
        }
    }

    auto FPlot2DWindowV2::GetWindowId() const -> const Str & {
        return WindowId;
    }

    auto FPlot2DWindowV2::SetTitle(Str title) -> void {
        Title = std::move(title);
    }

    auto FPlot2DWindowV2::GetTitle() const -> const Str & {
        return Title;
    }

    auto FPlot2DWindowV2::SetRegion(const RectR region) -> void {
        Region = region;
    }

    auto FPlot2DWindowV2::GetRegion() const -> const RectR & {
        return Region;
    }

    auto FPlot2DWindowV2::SetViewport(const RectI viewport) -> void {
        Viewport = viewport;
    }

    auto FPlot2DWindowV2::GetViewport() const -> const RectI & {
        return Viewport;
    }

    auto FPlot2DWindowV2::SetAutoFitRanges(const bool autoFitRanges) -> void {
        bAutoFitRanges = autoFitRanges;
    }

    auto FPlot2DWindowV2::GetAutoFitRanges() const -> bool {
        return bAutoFitRanges;
    }

    auto FPlot2DWindowV2::BuildReflectionParameterBindings() -> Vector<FPlotReflectionParameterBindingV2> {
        Vector<FPlotReflectionParameterBindingV2> bindings;

        auto makeFloatBinding = [this](const Str &parameterId,
                                       const Str &displayName,
                                       const Str &description,
                                       DevFloat *target) {
            FPlotReflectionParameterBindingV2 binding;
            binding.Schema.ParameterId = parameterId;
            binding.Schema.DisplayName = displayName;
            binding.Schema.Description = description;
            binding.Schema.TypeId = ReflectionV2::CTypeIdScalarFloat64;
            binding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
            binding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
            binding.ReadCurrent = [target] { return ReflectionV2::MakeFloatValue(*target); };
            binding.WriteLiveValue = [target, parameterId](const ReflectionV2::FReflectionValueV2 &value) {
                try {
                    *target = std::stod(value.Encoded);
                } catch (const std::exception &e) {
                    return ReflectionV2::FOperationResultV2::Error(
                        "plot2d_v2.parameter.parse_float",
                        "Could not parse float for parameter '" + parameterId + "': " + e.what());
                }

                return ReflectionV2::FOperationResultV2::Ok();
            };
            return binding;
        };

        FPlotReflectionParameterBindingV2 windowIdBinding;
        windowIdBinding.Schema.ParameterId = "window_id";
        windowIdBinding.Schema.DisplayName = "Window Id";
        windowIdBinding.Schema.Description = "Stable reflection id for this window.";
        windowIdBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarString;
        windowIdBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        windowIdBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        windowIdBinding.ReadCurrent = [this] { return ReflectionV2::MakeStringValue(WindowId); };
        bindings.push_back(std::move(windowIdBinding));

        FPlotReflectionParameterBindingV2 titleBinding;
        titleBinding.Schema.ParameterId = "title";
        titleBinding.Schema.DisplayName = "Title";
        titleBinding.Schema.Description = "Window title.";
        titleBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarString;
        titleBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        titleBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        titleBinding.ReadCurrent = [this] { return ReflectionV2::MakeStringValue(Title); };
        titleBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            Title = value.Encoded;
            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(titleBinding));

        bindings.push_back(makeFloatBinding("x_min", "X Min", "Left boundary of plot region.", &Region.xMin));
        bindings.push_back(makeFloatBinding("x_max", "X Max", "Right boundary of plot region.", &Region.xMax));
        bindings.push_back(makeFloatBinding("y_min", "Y Min", "Bottom boundary of plot region.", &Region.yMin));
        bindings.push_back(makeFloatBinding("y_max", "Y Max", "Top boundary of plot region.", &Region.yMax));

        FPlotReflectionParameterBindingV2 autoFitBinding;
        autoFitBinding.Schema.ParameterId = "auto_fit_ranges";
        autoFitBinding.Schema.DisplayName = "Auto Fit Ranges";
        autoFitBinding.Schema.Description = "If true, region auto-fits artists before rendering.";
        autoFitBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarBool;
        autoFitBinding.Schema.Mutability = ReflectionV2::EParameterMutability::RuntimeMutable;
        autoFitBinding.Schema.Exposure = ReflectionV2::EParameterExposure::WritableExposed;
        autoFitBinding.ReadCurrent = [this] { return ReflectionV2::MakeBoolValue(bAutoFitRanges); };
        autoFitBinding.WriteLiveValue = [this](const ReflectionV2::FReflectionValueV2 &value) {
            bool parsed = false;
            if (!ReflectionV2::ParseBoolValue(value.Encoded, parsed)) {
                return ReflectionV2::FOperationResultV2::Error(
                    "plot2d_v2.parameter.parse_bool",
                    "Could not parse bool value for parameter 'auto_fit_ranges'.");
            }
            bAutoFitRanges = parsed;
            return ReflectionV2::FOperationResultV2::Ok();
        };
        bindings.push_back(std::move(autoFitBinding));

        FPlotReflectionParameterBindingV2 artistCountBinding;
        artistCountBinding.Schema.ParameterId = "artist_count";
        artistCountBinding.Schema.DisplayName = "Artist Count";
        artistCountBinding.Schema.Description = "Number of artists currently attached to this window.";
        artistCountBinding.Schema.TypeId = ReflectionV2::CTypeIdScalarInt32;
        artistCountBinding.Schema.Mutability = ReflectionV2::EParameterMutability::Const;
        artistCountBinding.Schema.Exposure = ReflectionV2::EParameterExposure::ReadOnlyExposed;
        artistCountBinding.ReadCurrent = [this] { return ReflectionV2::MakeIntValue(static_cast<int>(Artists.size())); };
        bindings.push_back(std::move(artistCountBinding));

        return bindings;
    }

    auto FPlot2DWindowV2::BuildReflectionOperations() -> Vector<FOperationSchemaV2> {
        Vector<FOperationSchemaV2> operations;

        FOperationSchemaV2 fitToArtists;
        fitToArtists.OperationId = "command.window.fit_to_artists";
        fitToArtists.DisplayName = "Fit To Artists";
        fitToArtists.Description = "Compute region bounds from artists and optionally apply padding.";
        fitToArtists.Kind = ReflectionV2::EOperationKind::Command;
        fitToArtists.ThreadAffinity = ReflectionV2::EThreadAffinity::Any;
        fitToArtists.RunStatePolicy = ReflectionV2::ERunStatePolicy::Any;
        fitToArtists.SideEffectClass = ReflectionV2::ESideEffectClass::LocalState;
        fitToArtists.Inputs = {
            ReflectionV2::FOperationFieldSchemaV2{
                "padding_fraction",
                "Padding Fraction",
                "Optional non-negative padding fraction.",
                ReflectionV2::CTypeIdScalarFloat64,
                false
            }
        };
        fitToArtists.InvokeHandler = [this](const ReflectionV2::FValueMapV2 &inputs,
                                            const ReflectionV2::FInvocationContextV2 &) {
            DevFloat padding = 0.05;
            if (const auto it = inputs.find("padding_fraction"); it != inputs.end()) {
                try {
                    padding = std::stod(it->second.Encoded);
                } catch (const std::exception &e) {
                    return ReflectionV2::FOperationResultV2::Error(
                        "plot2d_v2.operation.parse_padding",
                        "Could not parse 'padding_fraction': " + Str(e.what()));
                }
            }

            const bool fitted = FitRegionToArtists(padding);

            ReflectionV2::FValueMapV2 output;
            output["window_id"] = ReflectionV2::MakeStringValue(WindowId);
            output["fitted"] = ReflectionV2::MakeBoolValue(fitted);
            output["x_min"] = ReflectionV2::MakeFloatValue(Region.xMin);
            output["x_max"] = ReflectionV2::MakeFloatValue(Region.xMax);
            output["y_min"] = ReflectionV2::MakeFloatValue(Region.yMin);
            output["y_max"] = ReflectionV2::MakeFloatValue(Region.yMax);
            return ReflectionV2::FOperationResultV2::Ok(std::move(output));
        };
        operations.push_back(std::move(fitToArtists));

        FOperationSchemaV2 queryArtists;
        queryArtists.OperationId = "query.window.list_artists";
        queryArtists.DisplayName = "List Artists";
        queryArtists.Description = "Query artist ids currently bound to this window.";
        queryArtists.Kind = ReflectionV2::EOperationKind::Query;
        queryArtists.ThreadAffinity = ReflectionV2::EThreadAffinity::Any;
        queryArtists.RunStatePolicy = ReflectionV2::ERunStatePolicy::Any;
        queryArtists.SideEffectClass = ReflectionV2::ESideEffectClass::None;
        queryArtists.InvokeHandler = [this](const ReflectionV2::FValueMapV2 &inputs,
                                            const ReflectionV2::FInvocationContextV2 &) {
            (void) inputs;

            StrVector artistIds;
            artistIds.reserve(Artists.size());
            for (const auto &slot : Artists) {
                if (slot.Artist == nullptr) continue;
                artistIds.push_back(slot.Artist->GetArtistId());
            }

            ReflectionV2::FValueMapV2 output;
            output["window_id"] = ReflectionV2::MakeStringValue(WindowId);
            output["artist_count"] = ReflectionV2::MakeIntValue(static_cast<int>(artistIds.size()));
            output["artist_ids"] = ReflectionV2::MakeStringListValue(artistIds);
            return ReflectionV2::FOperationResultV2::Ok(std::move(output));
        };
        operations.push_back(std::move(queryArtists));

        return operations;
    }

    auto FPlot2DWindowV2::DescribeReflection() -> FPlotEntityReflectionDescriptorV2 {
        FPlotEntityReflectionDescriptorV2 descriptor;
        descriptor.InterfaceId = "v2.plot.window." + WindowId;
        descriptor.DisplayName = Title;
        descriptor.Description = "Plot2D window schema with backend-decoupled V2 artists.";
        descriptor.Version = 2;
        descriptor.Tags = {"plot2d", "v2", "window"};

        descriptor.Parameters = BuildReflectionParameterBindings();
        descriptor.Operations = BuildReflectionOperations();

        descriptor.Children.reserve(Artists.size());
        for (const auto &slot : Artists) {
            if (slot.Artist == nullptr) continue;
            descriptor.Children.push_back("v2.plot.artist." + WindowId + "." + slot.Artist->GetArtistId());
        }

        return descriptor;
    }

} // namespace Slab::Graphics::Plot2D::V2
