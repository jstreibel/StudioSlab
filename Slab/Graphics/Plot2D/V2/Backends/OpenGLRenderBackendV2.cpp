#include "OpenGLRenderBackendV2.h"

#include "Graphics/OpenGL/LegacyGL/LegacyMode.h"
#include "Graphics/OpenGL/LegacyGL/PointSetRenderer.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"
#include "Graphics/OpenGL/Shader.h"
#include "Graphics/OpenGL/Texture.h"
#include "Graphics/OpenGL/Utils.h"
#include "Graphics/Plot2D/PlotThemeManager.h"
#include "Graphics/Utils.h"

#include "Math/VectorSpace/Impl/PointSet.h"

#include <algorithm>
#include <cmath>
#include <type_traits>
#include <variant>

namespace Slab::Graphics::Plot2D::V2 {

    namespace {

        auto ResolveScreenViewport(const FPlotFrameContextV2 &frame) -> RectI {
            auto width = frame.Viewport.GetWidth();
            auto height = frame.Viewport.GetHeight();

            if (width <= 0 || height <= 0) {
                GLint viewport[4] = {0, 0, 1, 1};
                glGetIntegerv(GL_VIEWPORT, viewport);
                width = std::max(1, viewport[2]);
                height = std::max(1, viewport[3]);
            }

            return {0, width, 0, height};
        }

        auto SetupProjection(const EPlotCoordinateSpaceV2 coordinateSpace,
                             const FPlotFrameContextV2 &frame,
                             const RectI &screenViewport) -> void {
            OpenGL::Legacy::ResetModelView();

            if (coordinateSpace == EPlotCoordinateSpaceV2::Screen) {
                OpenGL::Legacy::SetupOrthoI(screenViewport);
            } else {
                OpenGL::Legacy::SetupOrtho(frame.PlotRegion);
            }
        }

        auto EmitPointSet(const Vector<FPoint2D> &points, PlotStyle style) -> void {
            if (points.empty()) return;

            Math::FPoint2DVec mathPoints;
            mathPoints.reserve(points.size());
            for (const auto &point : points) {
                mathPoints.push_back(point.ToReal2D());
            }

            Math::FPointSet pointSet{mathPoints};
            OpenGL::Legacy::RenderPointSet(pointSet, style);
        }

        auto Luminance(const FColor &color) -> DevFloat {
            return (0.2126 * color.r) + (0.7152 * color.g) + (0.0722 * color.b);
        }

        auto ContrastRatio(const FColor &lhs, const FColor &rhs) -> DevFloat {
            const auto l1 = Luminance(lhs);
            const auto l2 = Luminance(rhs);
            const auto bright = std::max(l1, l2);
            const auto dark = std::min(l1, l2);
            return (bright + 0.05) / (dark + 0.05);
        }

        auto MixColor(const FColor &lhs, const FColor &rhs, const DevFloat t) -> FColor {
            const auto a = std::clamp(t, static_cast<DevFloat>(0.0), static_cast<DevFloat>(1.0));
            return {
                static_cast<float>((1.0 - a) * lhs.r + a * rhs.r),
                static_cast<float>((1.0 - a) * lhs.g + a * rhs.g),
                static_cast<float>((1.0 - a) * lhs.b + a * rhs.b),
                lhs.a
            };
        }

        auto EnsureContrastColor(const FColor &color, const FColor &background) -> FColor {
            constexpr DevFloat MinContrast = 2.8;
            if (ContrastRatio(color, background) >= MinContrast) return color;

            const auto contrastToBlack = ContrastRatio(Black, background);
            const auto contrastToWhite = ContrastRatio(White, background);
            const auto target = contrastToWhite >= contrastToBlack ? White : Black;

            FColor candidate = color;
            for (int i = 1; i <= 10; ++i) {
                candidate = MixColor(color, target, static_cast<DevFloat>(i) / 10.0);
                if (ContrastRatio(candidate, background) >= MinContrast) {
                    return candidate;
                }
            }

            return MixColor(color, target, 0.85);
        }

        auto EnsureContrastStyle(PlotStyle style, const FColor &background) -> PlotStyle {
            style.lineColor = EnsureContrastColor(style.lineColor, background);
            return style;
        }

        auto ResolveBackgroundColor(const FPlotFrameContextV2 &frame,
                                    const FPlotDrawListV2 &drawList,
                                    const GraphTheme_ptr &theme) -> FColor {
            auto color = theme != nullptr ? theme->graphBackground : Black;

            for (const auto &command : drawList.GetCommands()) {
                const auto *rectangle = std::get_if<FRectangleCommandV2>(&command);
                if (rectangle == nullptr) continue;
                if (!rectangle->bFilled || rectangle->CoordinateSpace != EPlotCoordinateSpaceV2::Plot) continue;
                if (rectangle->Rectangle == frame.PlotRegion) {
                    color = rectangle->Color;
                    break;
                }
            }

            return color;
        }

        auto DrawRectangle(const FRectangleCommandV2 &command) -> void {
            const auto &rect = command.Rectangle;

            if (command.bFilled) {
                OpenGL::Legacy::SetColor(command.Color);
                OpenGL::Legacy::DrawRectangle(FRectangleShape({rect.xMin, rect.yMax}, {rect.xMax, rect.yMin}));
                return;
            }

            OpenGL::Legacy::SetColor(command.Color);
            glLineWidth(1.0f);
            glBegin(GL_LINE_LOOP);
            glVertex2d(rect.xMin, rect.yMin);
            glVertex2d(rect.xMin, rect.yMax);
            glVertex2d(rect.xMax, rect.yMax);
            glVertex2d(rect.xMax, rect.yMin);
            glEnd();
        }

        auto DrawTextCommand(const FTextCommandV2 &command,
                             const FPlotFrameContextV2 &frame,
                             const RectI &screenViewport,
                             const TPointer<FWriter> &writer) -> void {
            if (writer == nullptr || command.Text.empty()) return;

            auto pen = command.Location;
            if (command.CoordinateSpace == EPlotCoordinateSpaceV2::Plot) {
                pen = FromSpaceToViewportCoord(pen, frame.PlotRegion, screenViewport);
            }

            writer->Reshape(screenViewport.GetWidth(), screenViewport.GetHeight());
            writer->ResetTransforms();
            const auto fontScale = std::clamp(command.FontScale, static_cast<DevFloat>(0.1), static_cast<DevFloat>(8.0));
            if (std::abs(fontScale - static_cast<DevFloat>(1.0)) > 1.0e-3) {
                // Scale around the anchor so plot-space labels can zoom with the view.
                writer->Translate(static_cast<float>(pen.x), static_cast<float>(pen.y));
                writer->Scale(static_cast<float>(fontScale), static_cast<float>(fontScale));
                writer->Translate(static_cast<float>(-pen.x), static_cast<float>(-pen.y));
            }
            writer->Write(command.Text, pen, command.Color, command.bVertical);

            // Ensure texture/shader state is returned to legacy defaults.
            OpenGL::FTexture::Deactivate();
            OpenGL::FShader::LegacyGL();
        }

        auto DrawLegendEntries(const Vector<FLegendEntryV2> &entries,
                               const RectI &screenViewport,
                               const TPointer<FWriter> &writer,
                               const FColor &plotBackgroundColor) -> void {
            if (entries.empty() || writer == nullptr) return;

            const auto fontHeight = std::max<DevFloat>(10.0, writer->GetFontHeightInPixels());
            const auto rowHeight = std::max<DevFloat>(14.0, fontHeight + 2.0);
            const DevFloat padding = 10.0;
            const DevFloat sampleWidth = 26.0;
            const DevFloat sampleGap = 8.0;

            DevFloat maxLabelWidth = 0.0;
            for (const auto &entry : entries) {
                const auto approx = static_cast<DevFloat>(entry.Label.size()) * (0.56 * fontHeight);
                maxLabelWidth = std::max(maxLabelWidth, approx);
            }

            const DevFloat boxWidth = (2.0 * padding) + sampleWidth + sampleGap + maxLabelWidth;
            const DevFloat boxHeight = (2.0 * padding) + (rowHeight * static_cast<DevFloat>(entries.size()));
            const DevFloat x0 = static_cast<DevFloat>(screenViewport.GetWidth()) - padding - boxWidth;
            const DevFloat y0 = static_cast<DevFloat>(screenViewport.GetHeight()) - padding - boxHeight;

            const auto legendBackgroundColor =
                Luminance(plotBackgroundColor) < 0.42
                    ? FColor(0.94f, 0.95f, 0.97f, 0.78f)
                    : FColor(0.04f, 0.05f, 0.07f, 0.70f);
            const auto legendBorderColor = EnsureContrastColor(
                legendBackgroundColor.inverse(false).WithAlpha(0.82f),
                legendBackgroundColor);

            // Background.
            OpenGL::Legacy::SetColor(legendBackgroundColor);
            OpenGL::Legacy::DrawRectangle(FRectangleShape({x0, y0 + boxHeight}, {x0 + boxWidth, y0}));
            OpenGL::Legacy::SetColor(legendBorderColor);
            glLineWidth(1.0f);
            glBegin(GL_LINE_LOOP);
            glVertex2d(x0, y0);
            glVertex2d(x0, y0 + boxHeight);
            glVertex2d(x0 + boxWidth, y0 + boxHeight);
            glVertex2d(x0 + boxWidth, y0);
            glEnd();

            for (std::size_t i = 0; i < entries.size(); ++i) {
                const auto &entry = entries[i];
                const auto y = y0 + boxHeight - padding - (rowHeight * (static_cast<DevFloat>(i) + 0.5));

                auto sampleStyle = EnsureContrastStyle(entry.Style, legendBackgroundColor);
                if (sampleStyle.getPrimitive() == LinePrimitive::Points) {
                    sampleStyle.setPrimitive(LinePrimitive::LineStrip);
                }

                EmitPointSet(
                    {
                        {x0 + padding, y},
                        {x0 + padding + sampleWidth, y}
                    },
                    sampleStyle);

                writer->Write(
                    entry.Label,
                    {x0 + padding + sampleWidth + sampleGap, y + 0.28 * fontHeight},
                    sampleStyle.lineColor);
            }

            OpenGL::FTexture::Deactivate();
            OpenGL::FShader::LegacyGL();
        }

    } // namespace

    auto FOpenGLRenderBackendV2::GetBackendId() const -> Str {
        return "plot2d.v2.opengl.legacy";
    }

    auto FOpenGLRenderBackendV2::Render(const FPlotFrameContextV2 &frame,
                                        const FPlotDrawListV2 &drawList) -> bool {
        const auto screenViewport = ResolveScreenViewport(frame);
        auto theme = FPlotThemeManager::GetCurrent();
        auto writer = theme != nullptr ? theme->LabelsWriter : nullptr;
        const auto plotBackgroundColor = ResolveBackgroundColor(frame, drawList, theme);

        OpenGL::Legacy::FShaderGuard guard{};
        OpenGL::Legacy::PushScene();

        for (const auto &commandVariant : drawList.GetCommands()) {
            std::visit([&](const auto &command) {
                using TCommand = std::decay_t<decltype(command)>;
                SetupProjection(command.CoordinateSpace, frame, screenViewport);

                if constexpr (std::is_same_v<TCommand, FPolylineCommandV2>) {
                    auto style = EnsureContrastStyle(command.Style, plotBackgroundColor);
                    if (command.bClosed && style.getPrimitive() == LinePrimitive::LineStrip) {
                        style.setPrimitive(LinePrimitive::LineLoop);
                    }
                    if (!command.bClosed && style.getPrimitive() == LinePrimitive::LineLoop) {
                        style.setPrimitive(LinePrimitive::LineStrip);
                    }
                    EmitPointSet(command.Points, style);
                } else if constexpr (std::is_same_v<TCommand, FPointSetCommandV2>) {
                    EmitPointSet(command.Points, EnsureContrastStyle(command.Style, plotBackgroundColor));
                } else if constexpr (std::is_same_v<TCommand, FRectangleCommandV2>) {
                    DrawRectangle(command);
                } else if constexpr (std::is_same_v<TCommand, FTextCommandV2>) {
                    DrawTextCommand(command, frame, screenViewport, writer);
                }
            }, commandVariant);
        }

        if (bRenderLegend && !drawList.GetLegendEntries().empty()) {
            SetupProjection(EPlotCoordinateSpaceV2::Screen, frame, screenViewport);
            if (writer != nullptr) {
                writer->Reshape(screenViewport.GetWidth(), screenViewport.GetHeight());
                writer->ResetTransforms();
            }
            DrawLegendEntries(drawList.GetLegendEntries(), screenViewport, writer, plotBackgroundColor);
        }

        OpenGL::Legacy::PopScene();

        return !OpenGL::CheckGLErrors("FOpenGLRenderBackendV2::Render");
    }

    auto FOpenGLRenderBackendV2::SetRenderLegend(const bool renderLegend) -> void {
        bRenderLegend = renderLegend;
    }

    auto FOpenGLRenderBackendV2::GetRenderLegend() const -> bool {
        return bRenderLegend;
    }

} // namespace Slab::Graphics::Plot2D::V2
