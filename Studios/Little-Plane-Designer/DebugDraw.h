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

class LegacyGLDebugDraw {
public:
    explicit LegacyGLDebugDraw(float alpha = 1.0f, int circleSegments = 24)
    : m_alpha(alpha)
    , m_circleSegments(circleSegments)
    , Writer(Slab::New<Slab::Graphics::OpenGL::FWriterOpenGL>(Slab::Core::Resources::GetIndexedFontFileName(3), 40))
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
        // Optional:
        m_dd.DrawStringFcn     = &LegacyGLDebugDraw::DrawStringThunk;
    }

    b2DebugDraw* handle() { return &m_dd; }
    void setAlpha(float a) { m_alpha = a; }
    void setCircleSegments(int n) { m_circleSegments = n > 3 ? n : 3; }

    void DrawVector(b2Vec2 f, b2Vec2 p, float scale, b2HexColor color=b2_colorYellow) const
    {
        const float mag = std::sqrt(f.x*f.x + f.y*f.y);
        if (mag <= 1e-6f) return;

        const b2Vec2 tip{ p.x + scale * f.x, p.y + scale * f.y };

        // shaft
        glColor(color, m_alpha);
        beginLines(1.5f);
        ::glVertex2f(p.x,   p.y);
        ::glVertex2f(tip.x, tip.y);
        endPrim();

        // arrow head
        const float headLen = 0.15f * std::max(0.5f, std::min(scale*mag, 2.0f));
        const float ang = std::atan2(f.y, f.x);
        const float a1 = ang + 2.61799388f; // +150 deg
        const float a2 = ang - 2.61799388f; // -150 deg

        beginLines(1.5f);
        ::glVertex2f(tip.x, tip.y);
        ::glVertex2f(tip.x + headLen * std::cos(a1), tip.y + headLen * std::sin(a1));
        ::glVertex2f(tip.x, tip.y);
        ::glVertex2f(tip.x + headLen * std::cos(a2), tip.y + headLen * std::sin(a2));
        endPrim();
    }

    void DrawTorque(b2BodyId bodyId, float torque, float radius=0.5f, int segments=24) const
    {
        if (std::fabs(torque) <= 1e-6f) return;

        // center of mass
        const b2Vec2 c = b2Body_GetWorldCenterOfMass(bodyId);
        const float dir = torque > 0.0f ? 1.0f : -1.0f;

        // ring
        glColor(b2_colorOrange, m_alpha);
        ::glDisable(GL_TEXTURE_2D);
        ::glBegin(GL_LINE_STRIP);
        for (int i = 0; i <= segments; ++i) {
            float a = dir * (2.0f * b2_pi * i / segments);
            ::glVertex2f(c.x + radius * std::cos(a), c.y + radius * std::sin(a));
        }
        ::glEnd();

        // arrow head at angle 0 in direction of rotation
        const float a0 = 0.0f;
        const b2Vec2 end{ c.x + radius * std::cos(a0), c.y + radius * std::sin(a0) };
        const float ha = a0 + (dir > 0.0f ? -b2_pi/6.0f : b2_pi/6.0f);

        beginLines(1.5f);
        ::glVertex2f(end.x, end.y);
        ::glVertex2f(end.x + 0.2f * std::cos(ha), end.y + 0.2f * std::sin(ha));
        endPrim();
    }


private:
    // ---- helpers ----
    static inline Slab::Graphics::FColor ToFColor(b2HexColor hc, float alpha)
    {
        const uint32_t rgb = static_cast<uint32_t>(hc);
        const float r = ((rgb >> 16) & 0xFF) / 255.0f;
        const float g = ((rgb >>  8) & 0xFF) / 255.0f;
        const float b = ((rgb      ) & 0xFF) / 255.0f;

        return Slab::Graphics::FColor(r, g, b, alpha);
    }
    static inline void glColor(b2HexColor hc, float alpha) {
        const uint32_t rgb = static_cast<uint32_t>(hc);
        const float r = ((rgb >> 16) & 0xFF) / 255.0f;
        const float g = ((rgb >>  8) & 0xFF) / 255.0f;
        const float b = ((rgb      ) & 0xFF) / 255.0f;

        ::glColor4f(r, g, b, alpha);
    }

    static inline void beginLines(float width = 1.0f) {
        ::glDisable(GL_TEXTURE_2D);
        ::glEnable(GL_BLEND);
        ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ::glLineWidth(width);
        ::glBegin(GL_LINES);
    }

    static inline void endPrim() { ::glEnd(); }

    static inline void drawPolyLine(const b2Vec2* v, int n, b2HexColor c, float a) {
        glColor(c, a);
        beginLines();
        for (int i = 0; i < n; ++i) {
            const b2Vec2 p1 = v[i];
            const b2Vec2 p2 = v[(i + 1) % n];
            ::glVertex2f(p1.x, p1.y);
            ::glVertex2f(p2.x, p2.y);
        }
        endPrim();
    }

    // ---- thunks (C callbacks -> C++) ----
    static void DrawSegmentThunk(b2Vec2 p1, b2Vec2 p2, b2HexColor color, void* ctx) {
        auto* self = static_cast<LegacyGLDebugDraw*>(ctx);
        glColor(color, self->m_alpha);
        beginLines();
        ::glVertex2f(p1.x, p1.y);
        ::glVertex2f(p2.x, p2.y);
        endPrim();
    }

    static void DrawPolygonThunk(const b2Vec2* verts, int count, b2HexColor color, void* ctx) {
        auto* self = static_cast<LegacyGLDebugDraw*>(ctx);
        drawPolyLine(verts, count, color, self->m_alpha);
    }

    static void DrawSolidPolygonThunk(b2Transform xf, const b2Vec2* localVerts, int count,
                                      float /*radius*/, b2HexColor color, void* ctx) {
        auto* self = static_cast<LegacyGLDebugDraw*>(ctx);
        // Transform to world
        std::vector<b2Vec2> v(count);
        for (int i = 0; i < count; ++i) v[i] = b2TransformPoint(xf, localVerts[i]);

        // Fill
        glColor(color, 0.35f * self->m_alpha);
        ::glDisable(GL_TEXTURE_2D);
        ::glEnable(GL_BLEND);
        ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ::glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i < count; ++i) ::glVertex2f(v[i].x, v[i].y);
        ::glEnd();

        // Outline
        drawPolyLine(v.data(), count, color, self->m_alpha);
    }

    static void DrawCircleThunk(b2Vec2 center, float radius, b2HexColor color, void* ctx) {
        auto* self = static_cast<LegacyGLDebugDraw*>(ctx);
        glColor(color, self->m_alpha);
        ::glDisable(GL_TEXTURE_2D);
        ::glBegin(GL_LINE_LOOP);
        for (int i = 0; i < self->m_circleSegments; ++i) {
            const float a = (2.0f * b2_pi * i) / self->m_circleSegments;
            ::glVertex2f(center.x + radius * std::cos(a),
                         center.y + radius * std::sin(a));
        }
        ::glEnd();
    }

    static void DrawSolidCircleThunk(b2Transform xf, float radius, b2HexColor color, void* ctx) {
        auto* self = static_cast<LegacyGLDebugDraw*>(ctx);
        const b2Vec2 c = xf.p; // center
        // Fill
        glColor(color, 0.35f * self->m_alpha);
        ::glDisable(GL_TEXTURE_2D);
        ::glEnable(GL_BLEND);
        ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ::glBegin(GL_TRIANGLE_FAN);
        for (int i = 0; i < self->m_circleSegments; ++i) {
            const float a = (2.0f * b2_pi * i) / self->m_circleSegments;
            ::glVertex2f(c.x + radius * std::cos(a), c.y + radius * std::sin(a));
        }
        ::glEnd();
        // Outline
        DrawCircleThunk(c, radius, color, ctx);
    }

    static void DrawSolidCapsuleThunk(b2Vec2 p1, b2Vec2 p2, float r, b2HexColor color, void* ctx) {
        auto* self = static_cast<LegacyGLDebugDraw*>(ctx);
        b2Vec2 d = {p2.x - p1.x, p2.y - p1.y};
        const float len = std::sqrt(d.x * d.x + d.y * d.y);
        if (len > 0.0f) { d.x /= len; d.y /= len; }
        const b2Vec2 n = {-d.y, d.x};

        // Rectangle body (two long edges)
        const b2Vec2 v0 = {p1.x + n.x * r, p1.y + n.y * r};
        const b2Vec2 v1 = {p2.x + n.x * r, p2.y + n.y * r};
        const b2Vec2 v2 = {p2.x - n.x * r, p2.y - n.y * r};
        const b2Vec2 v3 = {p1.x - n.x * r, p1.y - n.y * r};

        // Fill
        glColor(color, 0.35f * self->m_alpha);
        ::glDisable(GL_TEXTURE_2D);
        ::glEnable(GL_BLEND);
        ::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        ::glBegin(GL_TRIANGLE_FAN); // cap at p1
        for (int i = 0; i <= self->m_circleSegments; ++i) {
            const float a = b2_pi + (b2_pi * i) / self->m_circleSegments;
            ::glVertex2f(p1.x + r * (d.x * std::cos(a) - n.x * std::sin(a)),
                         p1.y + r * (d.y * std::cos(a) - n.y * std::sin(a)));
        }
        ::glEnd();
        ::glBegin(GL_TRIANGLE_FAN); // cap at p2
        for (int i = 0; i <= self->m_circleSegments; ++i) {
            const float a = (b2_pi * i) / self->m_circleSegments;
            ::glVertex2f(p2.x + r * (d.x * std::cos(a) - n.x * std::sin(a)),
                         p2.y + r * (d.y * std::cos(a) - n.y * std::sin(a)));
        }
        ::glEnd();
        ::glBegin(GL_TRIANGLES); // body
        ::glVertex2f(v0.x, v0.y); ::glVertex2f(v1.x, v1.y); ::glVertex2f(v2.x, v2.y);
        ::glVertex2f(v0.x, v0.y); ::glVertex2f(v2.x, v2.y); ::glVertex2f(v3.x, v3.y);
        ::glEnd();

        // Outline
        b2Vec2 outline[4] = {v0, v1, v2, v3};
        drawPolyLine(outline, 4, color, self->m_alpha);
        DrawCircleThunk(p1, r, color, ctx);
        DrawCircleThunk(p2, r, color, ctx);
    }

    static void DrawPointThunk(b2Vec2 p, float size, b2HexColor color, void* ctx) {
        auto* self = static_cast<LegacyGLDebugDraw*>(ctx);
        glColor(color, self->m_alpha);
        ::glDisable(GL_TEXTURE_2D);
        ::glPointSize(size);
        ::glBegin(GL_POINTS);
        ::glVertex2f(p.x, p.y);
        ::glEnd();
    }

    static void DrawTransformThunk(b2Transform xf, void* ctx) {
        auto* self = static_cast<LegacyGLDebugDraw*>(ctx);
        const float L = 0.5f; // axis length in world units
        const b2Vec2 px = b2TransformPoint(xf, b2Vec2{ L, 0.0f});
        const b2Vec2 py = b2TransformPoint(xf, b2Vec2{0.0f,  L});
        const b2Vec2 p  = xf.p;

        // X axis in red
        glColor(b2_colorRed, self->m_alpha);
        beginLines(2.0f);
        ::glVertex2f(p.x,  p.y); ::glVertex2f(px.x, px.y);
        endPrim();
        // Y axis in green
        glColor(b2_colorLime, self->m_alpha);
        beginLines(2.0f);
        ::glVertex2f(p.x,  p.y); ::glVertex2f(py.x, py.y);
        endPrim();
    }

    static void DrawStringThunk(b2Vec2 p, const char* s, b2HexColor c, void* ctx)
    {
        auto* self = static_cast<LegacyGLDebugDraw*>(ctx);
        auto alpha = self->m_alpha;
        self->Writer->Write(std::string(s), {p.x, p.y}, ToFColor(c, alpha));

        Slab::Graphics::OpenGL::Texture::deactivate();
        Slab::Graphics::OpenGL::FShader::LegacyGL();
    }

    b2DebugDraw m_dd{};
    float m_alpha{1.0f};
    int   m_circleSegments{24};

public:
    Slab::TPointer<Slab::Graphics::OpenGL::FWriterOpenGL> Writer;
};

#endif //STUDIOSLAB_DEBUGDRAW_H