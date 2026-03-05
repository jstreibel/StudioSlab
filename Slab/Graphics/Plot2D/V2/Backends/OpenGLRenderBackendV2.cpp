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
            writer->Write(command.Text, pen, command.Color, command.bVertical);

            // Ensure texture/shader state is returned to legacy defaults.
            OpenGL::FTexture::Deactivate();
            OpenGL::FShader::LegacyGL();
        }

        auto DrawLegendEntries(const Vector<FLegendEntryV2> &entries,
                               const RectI &screenViewport,
                               const TPointer<FWriter> &writer) -> void {
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

            // Background.
            OpenGL::Legacy::SetColor(FColor(0.04f, 0.05f, 0.07f, 0.70f));
            OpenGL::Legacy::DrawRectangle(FRectangleShape({x0, y0 + boxHeight}, {x0 + boxWidth, y0}));

            for (std::size_t i = 0; i < entries.size(); ++i) {
                const auto &entry = entries[i];
                const auto y = y0 + boxHeight - padding - (rowHeight * (static_cast<DevFloat>(i) + 0.5));

                auto sampleStyle = entry.Style;
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
                    entry.Style.lineColor);
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

        OpenGL::Legacy::FShaderGuard guard{};
        OpenGL::Legacy::PushScene();

        for (const auto &commandVariant : drawList.GetCommands()) {
            std::visit([&](const auto &command) {
                using TCommand = std::decay_t<decltype(command)>;
                SetupProjection(command.CoordinateSpace, frame, screenViewport);

                if constexpr (std::is_same_v<TCommand, FPolylineCommandV2>) {
                    auto style = command.Style;
                    if (command.bClosed && style.getPrimitive() == LinePrimitive::LineStrip) {
                        style.setPrimitive(LinePrimitive::LineLoop);
                    }
                    if (!command.bClosed && style.getPrimitive() == LinePrimitive::LineLoop) {
                        style.setPrimitive(LinePrimitive::LineStrip);
                    }
                    EmitPointSet(command.Points, style);
                } else if constexpr (std::is_same_v<TCommand, FPointSetCommandV2>) {
                    EmitPointSet(command.Points, command.Style);
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
            DrawLegendEntries(drawList.GetLegendEntries(), screenViewport, writer);
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
