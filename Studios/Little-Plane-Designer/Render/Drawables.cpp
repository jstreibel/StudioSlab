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

void FDecal::Draw(const Slab::Graphics::FDraw2DParams&) {

    fix Rect = Slab::Graphics::FRectangleShapeBuilder{}
    .WithWidth(Size.x)
    .WithHeight(Size.y)
    .At(Position)
    .Build();

    Slab::Graphics::OpenGL::Legacy::DrawRectangleWithTexture(Rect, *Texture, ZIndex);
}

FRuler::FRuler(
    const Slab::Graphics::FPoint2D &Loc,
    const float &Unit,
    const Slab::DevFloat ZIndex): Loc(Loc), Unit(Unit), ZIndex(ZIndex) {}

void FRuler::Draw(const Slab::Graphics::FDraw2DParams&) {
    const Slab::Graphics::FColor SafeYellow = Slab::Graphics::FColor::FromHex("#FFC000");

    glColor4f(SafeYellow.r, SafeYellow.g, SafeYellow.b, SafeYellow.a);

    const float HalfWidth = RulerWidth * .5f;
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

void FSky::Draw(const Slab::Graphics::FDraw2DParams&) {

    constexpr float horizonY = .0f;
    namespace LegacyGL = Slab::Graphics::OpenGL::Legacy;

    LegacyGL::SetupLegacyGL();
    LegacyGL::PushLegacyMode();

    LegacyGL::PushScene();
    LegacyGL::ResetModelView();
    LegacyGL::SetupOrthoI({0, 1, 0, 1});

    glBegin(GL_TRIANGLE_STRIP);

    constexpr float Infinity = -0.9f;

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

    LegacyGL::PopScene();
}
