//
// Created by joao on 6/8/24.
//

#include "ShapeRenderer.h"

#include "SceneSetup.h"
#include "Graphics/OpenGL/Shader.h"


namespace Slab::Graphics::OpenGL::Legacy {
    void SetColor(const FColor& Color) {
        glColor4f(Color.r, Color.g, Color.b, Color.a);
    }

    void DrawRectangle(const FRectangleShape &RectangleShape) {
        fix tl = RectangleShape.top_left;
        fix br = RectangleShape.bottom_right;

        glBegin(GL_QUADS);
        {
            glVertex2f(tl.x, tl.y);
            glVertex2f(br.x, tl.y);
            glVertex2f(br.x, br.y);
            glVertex2f(tl.x, br.y);
        }
        glEnd();
    }

void DrawRectangleWithTexture(
    const FRectangleShape& Shape,
    const FTexture& Texture,
    const DevFloat ZIndex)
{
    // World-space half-size (2x2 square)
    const float HalfWidth = Shape.GetWidth() / 2;
    const float HalfHeight = Shape.GetHeight() / 2;

    auto xform = [&](const float lx, const float ly) -> std::pair<double,double> {

        const auto& [xBody, yBody] = Shape.GetCenter();

        // Apply body rotation and translation
        const double X = xBody + lx;
        const double Y = yBody + ly;
        return {X, Y};
    };

    // Local corners (counter-clockwise): bottom-left, bottom-right, top-right, top-left
    const auto [blx, bly] = xform(-HalfWidth, -HalfHeight);
    const auto [brx, bry] = xform( +HalfWidth, -HalfHeight);
    const auto [trx, try_] = xform( +HalfWidth,  +HalfHeight);
    const auto [tlx, tly] = xform(-HalfWidth,  +HalfHeight);

    // Bind texture and render immediate-mode quad
    FTexture::EnableTextures();
    Texture.Activate();
    Texture.Bind();

    // White modulation to preserve original texture colors
    glColor4f(1.f, 1.f, 1.f, 1.f);

    glBegin(GL_QUADS);
    {
        // Use flipped V to compensate for typical top-left image origin
        glTexCoord2f(0.f, 1.f); glVertex3d(blx, bly, ZIndex);
        glTexCoord2f(1.f, 1.f); glVertex3d(brx, bry, ZIndex);
        glTexCoord2f(1.f, 0.f); glVertex3d(trx, try_, ZIndex);
        glTexCoord2f(0.f, 0.f); glVertex3d(tlx, tly, ZIndex);
    }
    glEnd();

    // Cleanup state
    FTexture::Deactivate();
    FTexture::DisableTextures();
}

    void DrawLine(const Real2D& start, const Real2D& end, const FColor& color, const DevFloat &thickness) {
        SetColor(color);

        glLineWidth(thickness);

        glBegin(GL_LINES);
        glVertex2d(start.x, start.y);
        glVertex2d(end.x, end.y);
        glEnd();
    }

}
