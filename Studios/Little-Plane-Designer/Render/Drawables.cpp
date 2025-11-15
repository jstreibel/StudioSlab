//
// Created by joao on 11/14/25.
//

#include "Drawables.h"

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
    return Slab::Graphics::OpenGL::FTexture2D_Color::FromImageFile(Loc);
}

void FGuy::Draw(const Slab::Graphics::IDrawProviders&) {
    if (!GuyTexture)
        GuyTexture = GetTexture("Guy.png");

    fix Rect = Slab::Graphics::FRectangleShapeBuilder{}
    .WithWidth(2.0)
    .WithHeight(2.0)
    .At(Position)
    .Build();

    Slab::Graphics::OpenGL::Legacy::DrawRectangleWithTexture(Rect, *GuyTexture);
}

void FCat::Draw(const Slab::Graphics::IDrawProviders&) {
    if (!CatTexture)
        CatTexture = GetTexture("Cat.png");

    fix Rect = Slab::Graphics::FRectangleShapeBuilder{}
    .WithWidth(.32)
    .WithHeight(.32)
    .At(Position)
    .Build();

    Slab::Graphics::OpenGL::Legacy::DrawRectangleWithTexture(Rect, *CatTexture);
}

FRuler::FRuler(const Slab::Graphics::Point2D &Loc, const float &Unit): Loc(Loc), Unit(Unit) {}

void FRuler::Draw(const Slab::Graphics::IDrawProviders&) {
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

void FSky::Draw(const Slab::Graphics::IDrawProviders& ResProvider) {

    constexpr float horizonY = .0f;
    namespace LegacyGL = Slab::Graphics::OpenGL::Legacy;

    LegacyGL::SetupLegacyGL();
    LegacyGL::PushLegacyMode();

    LegacyGL::PushScene();
    LegacyGL::ResetModelView();
    LegacyGL::SetupOrthoI({0, 1, 0, 1});

    glBegin(GL_TRIANGLE_STRIP);

    // bottom-left (near horizon): light, a bit warmer
    glColor3f(0.65f, 0.80f, 0.95f);
    glVertex2f(0.0f, horizonY);

    // top-left: darker, more saturated
    glColor3f(0.10f, 0.30f, 0.55f);
    glVertex2f(0.0f, 1.0f);

    // bottom-right
    glColor3f(0.65f, 0.80f, 0.95f);
    glVertex2f(1.0f, horizonY);

    // top-right
    glColor3f(0.10f, 0.30f, 0.55f);
    glVertex2f(1.0f, 1.0f);

    glEnd();

    LegacyGL::PopScene();
}
