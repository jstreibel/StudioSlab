//
// Created by joao on 11/14/25.
//

#include "Drawables.h"

#include "Core/Tools/Log.h"
#include "Core/Tools/Resources.h"
#include "Graphics/OpenGL/Texture.h"
#include "Graphics/OpenGL/Texture2D_Color.h"
#include "Graphics/OpenGL/LegacyGL/LegacyMode.h"
#include "Graphics/OpenGL/LegacyGL/SceneSetup.h"
#include "Graphics/OpenGL/LegacyGL/ShapeRenderer.h"
#include "Graphics/IDrawBackend2D.h"
#include "Graphics/Styles/Colors.h"

#include <array>
#include <vector>

static Slab::TPointer<Slab::Graphics::OpenGL::FTexture> GuyTexture = nullptr;
static Slab::TPointer<Slab::Graphics::OpenGL::FTexture> CatTexture = nullptr;

inline auto GetTexture(const Slab::Str& FileName) {
    fix Loc = Slab::Core::Resources::GetResourcesPath() + "/LittlePlaneDesigner/" + FileName;

    fix TextureResult = Slab::Graphics::OpenGL::FTexture2D_Color::FromImageFile(Loc);

    if (TextureResult.IsFailure()) {
        Slab::Core::Log::Error("Failed to load texture: " + FileName) << TextureResult.ToString() << Slab::Core::Log::Flush;
        throw std::runtime_error("Failed to load texture: " + FileName);
    }

    return TextureResult.Value();
}

FDecal::FDecal(
    const Slab::Str& LittlePlaneDesigner_FileName,
    const Slab::Graphics::FPoint2D Position,
    const Slab::Graphics::FPoint2D Size, Slab::DevFloat ZIndex)
: Position(Position)
, ZIndex(ZIndex)
, Size(Size)
, Texture(GetTexture(LittlePlaneDesigner_FileName)){ }

void FDecal::Draw(const Slab::Graphics::FDraw2DParams& Params) {

    fix Rect = Slab::Graphics::FRectangleShapeBuilder{}
    .WithWidth(Size.x)
    .WithHeight(Size.y)
    .At(Position)
    .Build();

    const auto Backend = Params.Backend;
    if (Backend != nullptr) {
        std::array<Slab::Graphics::FTexturedVertex2D, 4> Vertices{{
            {{Rect.top_left.x, Rect.top_left.y}, {0.0f, 1.0f}, ZIndex},
            {{Rect.bottom_right.x, Rect.top_left.y}, {1.0f, 1.0f}, ZIndex},
            {{Rect.bottom_right.x, Rect.bottom_right.y}, {1.0f, 0.0f}, ZIndex},
            {{Rect.top_left.x, Rect.bottom_right.y}, {0.0f, 0.0f}, ZIndex},
        }};
        Backend->DrawTexturedQuad(Vertices, Slab::Graphics::White, *Texture);
        return;
    }

    Draw::DrawRectangleWithTexture(Rect, *Texture, ZIndex);
}

FRuler::FRuler(
    const Slab::Graphics::FPoint2D &Loc,
    const float &Unit,
    const Slab::DevFloat ZIndex): Loc(Loc), Unit(Unit), ZIndex(ZIndex) {}

void FRuler::Draw(const Slab::Graphics::FDraw2DParams& Params) {
    const Slab::Graphics::FColor SafeYellow = Slab::Graphics::FColor::FromHex("#FFC000");

    const float HalfWidth = RulerWidth * .5f;
    const auto Backend = Params.Backend;

    if (Backend != nullptr) {
        Slab::Graphics::FRectangleShape Body{{Loc.x - HalfWidth, Loc.y + Unit}, {Loc.x + HalfWidth, Loc.y}};
        Backend->DrawSolidQuad(Body, SafeYellow, ZIndex);

        std::vector<Slab::Graphics::FLineSegment2D> Segments;
        Segments.reserve(static_cast<size_t>(Unit / 0.01f) + 4);

        for (float y = Loc.y; y < Loc.y + Unit; y += 0.1f) {
            Segments.push_back({{Loc.x + HalfWidth, y}, {Loc.x, y}, ZIndex});
        }
        Backend->DrawLineSegments(Segments, Slab::Graphics::Black, 2.0f);

        Segments.clear();
        for (float y = Loc.y; y < Loc.y + Unit; y += 0.05f) {
            Segments.push_back({{Loc.x + HalfWidth, y}, {Loc.x + .3f * HalfWidth, y}, ZIndex});
        }
        Backend->DrawLineSegments(Segments, Slab::Graphics::Black, 2.0f);

        Segments.clear();
        for (float y = Loc.y; y < Loc.y + Unit; y += 0.01f) {
            Segments.push_back({{Loc.x + HalfWidth, y}, {Loc.x + .5f * HalfWidth, y}, ZIndex});
        }
        Backend->DrawLineSegments(Segments, Slab::Graphics::Black, 1.0f);
        return;
    }

    glColor4f(SafeYellow.r, SafeYellow.g, SafeYellow.b, SafeYellow.a);

    glBegin(GL_QUADS);
    {
        glVertex2f(Loc.x+HalfWidth, Loc.y);
        glVertex2f(Loc.x+HalfWidth, Loc.y+Unit);
        glVertex2f(Loc.x-HalfWidth, Loc.y+Unit);
        glVertex2f(Loc.x-HalfWidth, Loc.y);
    }
    glEnd();

    glColor3i(0, 0, 0);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (float y = Loc.y; y < Loc.y+Unit; y+=0.1f) {
        glVertex2f(Loc.x+HalfWidth, y);
        glVertex2f(Loc.x, y);
    }
    glEnd();
    glLineWidth(2.0f);
    glBegin(GL_LINES);
    for (float y = Loc.y; y < Loc.y+Unit; y+=0.05f) {
        glVertex2f(Loc.x+HalfWidth, y);
        glVertex2f(Loc.x+.3*HalfWidth, y);
    }
    glEnd();
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    for (float y = Loc.y; y < Loc.y+Unit; y+=0.01f) {
        glVertex2f(Loc.x+HalfWidth, y);
        glVertex2f(Loc.x+.5*HalfWidth, y);
    }
    glEnd();
}

void FSky::Draw(const Slab::Graphics::FDraw2DParams& Params) {

    constexpr float horizonY = .0f;
    constexpr float Infinity = -0.9f;

    if (const auto Backend = Params.Backend; Backend != nullptr) {
        std::array<Slab::Graphics::FColoredVertex2D, 4> Vertices{{
            {{0.0f, horizonY}, Infinity, HorizonColor},
            {{0.0f, 1.0f},     Infinity, SkyColor},
            {{1.0f, horizonY}, Infinity, HorizonColor},
            {{1.0f, 1.0f},     Infinity, SkyColor},
        }};
        Backend->DrawTriangleStrip(Vertices);
        return;
    }

    Draw::SetupLegacyGL();
    Draw::PushLegacyMode();

    Draw::PushScene();
    Draw::ResetModelView();
    Draw::SetupOrthoI({0, 1, 0, 1});

    glBegin(GL_TRIANGLE_STRIP);

    // bottom-left (near horizon): light, a bit warmer
    glColor4fv(HorizonColor.asFloat4fv());
    glVertex3f(0.0f, horizonY, Infinity);

    // top-left: darker, more saturated
    glColor4fv(SkyColor.asFloat4fv());
    glVertex3f(0.0f, 1.0f, Infinity);

    // bottom-right
    glColor4fv(HorizonColor.asFloat4fv());
    glVertex3f(1.0f, horizonY, Infinity);

    // top-right
    glColor4fv(SkyColor.asFloat4fv());
    glVertex3f(1.0f, 1.0f, Infinity);

    glEnd();

    Draw::PopScene();
}
