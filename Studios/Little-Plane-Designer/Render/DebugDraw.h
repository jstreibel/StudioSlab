//
// Created by joao on 10/17/25.
//

#ifndef STUDIOSLAB_DEBUGDRAW_H
#define STUDIOSLAB_DEBUGDRAW_H

#include <cstring>
#include <vector>
#include <box2d/box2d.h>
#include <GL/glew.h>

#include <Graphics/OpenGL/WriterOpenGL.h>

#include "Core/Tools/Resources.h"
#include "Graphics/OpenGL/LegacyGL/LegacyMode.h"

// Minimal, dependency-free immediate-mode debug draw for Box2D v3.1.x
// Usage:
//   LegacyGLDebugDraw dbg;
//   auto* dd = dbg.handle();           // b2DebugDraw*
//   dd->drawShapes = true;             // set flags as desired
//   b2World_Draw(world, dd);           // call each frame after your GL proj/view
//
// Requires a fixed-function GL context (e.g., glMatrixMode, glBegin/glEnd).

constexpr auto b2_pi = M_PI;
constexpr auto ArrowThickness = 4.f;

class LegacyGLDebugDraw {
public:
    explicit LegacyGLDebugDraw(float alpha = 1.0f, int circleSegments = 128)
    : m_alpha(alpha)
    , m_circleSegments(circleSegments)
    , Writer(Slab::New<Slab::Graphics::OpenGL::FWriterOpenGL>(Slab::Core::Resources::GetIndexedFontFileName(3), 28))
    {
        std::memset(&m_dd, 0, sizeof(m_dd));
        m_dd.context = this;

        m_dd.drawShapes = true;
        m_dd.drawJoints = true;
        m_dd.drawJointExtras = true;
        m_dd.drawBounds = true;
        m_dd.drawMass = true;
        m_dd.drawBodyNames = true;
        m_dd.drawContacts = true;
        m_dd.drawGraphColors = true;
        m_dd.drawContactNormals = true;
        m_dd.drawContactImpulses = true;
        m_dd.drawContactFeatures = true;
        m_dd.drawFrictionImpulses = true;
        m_dd.drawIslands = true;

        m_dd.DrawSegmentFcn       = &LegacyGLDebugDraw::DrawSegmentThunk;
        m_dd.DrawPolygonFcn       = &LegacyGLDebugDraw::DrawPolygonThunk;
        m_dd.DrawSolidPolygonFcn  = &LegacyGLDebugDraw::DrawSolidPolygonThunk;
        m_dd.DrawCircleFcn        = &LegacyGLDebugDraw::DrawCircleThunk;
        m_dd.DrawSolidCircleFcn   = &LegacyGLDebugDraw::DrawSolidCircleThunk;
        m_dd.DrawSolidCapsuleFcn  = &LegacyGLDebugDraw::DrawSolidCapsuleThunk;
        m_dd.DrawPointFcn         = &LegacyGLDebugDraw::DrawPointThunk;
        m_dd.DrawTransformFcn     = &LegacyGLDebugDraw::DrawTransformThunk;

        m_dd.DrawStringFcn        = &LegacyGLDebugDraw::DrawStringThunk;
    }

    b2DebugDraw* handle() { return &m_dd; }
    void setAlpha(float a) { m_alpha = a; }
    void setCircleSegments(int n) { m_circleSegments = n > 3 ? n : 3; }

    static void SetupLegacyGL() { Slab::Graphics::OpenGL::Legacy::SetupLegacyGL(); }

    void DrawVector(b2Vec2 f, b2Vec2 p, float scale=1.f, b2HexColor color=b2_colorYellow) const;

    void DrawPseudoVector(float mag, b2Vec2 c, float scale=1.0f, float alpha0=.0f, b2HexColor color=b2_colorOrange, int segments=200) const;

    void Write(std::string s, b2Vec2 p, const b2HexColor c = b2_colorWhite) const;

    Slab::TPointer<Slab::Graphics::OpenGL::FWriterOpenGL> GetWriter() const {
        return Writer;
    }

private:
    // ---- helpers ----
    static Slab::Graphics::FColor ToFColor(b2HexColor hc, float alpha);

    static void glColor(b2HexColor hc, float alpha);

    static void beginLines(float width = 1.0f);

    static void endPrim();

    static void drawPolyLine(const b2Vec2* v, int n, b2HexColor c, float a);

    // ---- thunks (C callbacks -> C++) ----
    static void DrawSegmentThunk(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* ctx);

    static void DrawPolygonThunk(const b2Vec2* verts, int count, b2HexColor color, void* ctx);

    static void DrawSolidPolygonThunk(b2Transform xf, const b2Vec2* localVerts, int count,
                                      float /*radius*/, b2HexColor color, void* ctx);

    static void DrawCircleThunk(b2Vec2 center, float radius, b2HexColor color, void* ctx);

    static void DrawSolidCircleThunk(b2Transform xf, float radius, b2HexColor color, void* ctx);

    static void DrawSolidCapsuleThunk(b2Vec2 p1, b2Vec2 p2, float r, b2HexColor color, void* ctx);

    static void DrawPointThunk(b2Vec2 p, float size, b2HexColor color, void* ctx);

    static void DrawTransformThunk(b2Transform xf, void* ctx);

    static void DrawStringThunk(b2Vec2 p, const char* s, b2HexColor c, void* ctx);

    b2DebugDraw m_dd{};
    float m_alpha{1.0f};
    int   m_circleSegments{128};
    Slab::TPointer<Slab::Graphics::OpenGL::FWriterOpenGL> Writer;

};

#endif //STUDIOSLAB_DEBUGDRAW_H
