//
// Created by joao on 6/30/24.
//

#include "FieldTextureKontraption.h"

namespace Slab::Graphics {

    struct Field2DThingyVertex {
        float x, y;     // position
        float s, t;     // texture
    };

    FieldTextureThingy::FieldTextureThingy(int x_res, int y_res, RectR sub_region) {
        texture = Slab::New<OpenGL::Texture2D_Real>((int) x_res, (int) y_res);
        texture->setSWrap(OpenGL::ClampToEdge);
        texture->setAntiAliasOff();

        auto hPixelSizeInTexCoord = 1. / x_res;
        auto vPixelSizeInTexCoord = 1. / y_res;

        auto hTexturePixelSizeInSpaceCoord = hPixelSizeInTexCoord * sub_region.GetWidth();
        auto vTexturePixelSizeInSpaceCoord = vPixelSizeInTexCoord * sub_region.GetHeight();

        auto si = 0.0f; // - hPixelSizeInTexCoord;
        auto sf = 1.0f; // + hPixelSizeInTexCoord;
        auto ti = 0.0f; // - vPixelSizeInTexCoord;
        auto tf = 1.0f; // + vPixelSizeInTexCoord;

        fix xMin_f = (float) (-.5*hTexturePixelSizeInSpaceCoord + sub_region.xMin);
        fix xMax_f = (float) (+.5*hTexturePixelSizeInSpaceCoord + sub_region.xMax);
        fix yMin_f = (float) (-.5*vTexturePixelSizeInSpaceCoord + sub_region.yMin);
        fix yMax_f = (float) (+.5*vTexturePixelSizeInSpaceCoord + sub_region.yMax);

        fix Lx = xMax_f-xMin_f;

        vertexBuffer = New<OpenGL::VertexBuffer>("vertex:2f,tex_coord:2f");
        GLuint indices[6] = {0, 1, 2, 0, 2, 3};

        fix n=0;
        for(int i=-n; i<=n; ++i) {
            fix Δx = Lx*(float)i;
            Field2DThingyVertex vertices[4] = {
                    {xMin_f+Δx, yMin_f, si, ti},
                    {xMax_f+Δx, yMin_f, sf, ti},
                    {xMax_f+Δx, yMax_f, sf, tf},
                    {xMin_f+Δx, yMax_f, si, tf}};

            vertexBuffer->pushBack(vertices, 4, indices, 6);
        }
    }

    FieldTextureKontraption::FieldTextureKontraption(Resolution full_xres, Resolution full_yres, RectR region)
    : xres(full_xres)
    , yres(full_yres)
    {
        fix max_res = OpenGL::Texture2D::GetMaxTextureSize();

        n = full_xres / max_res + 1;
        m = full_yres / max_res + 1;

        fix x_leftover = (int) full_xres % max_res;
        fix y_leftover = (int) full_yres % max_res;

        fix dx = region.GetWidth()  / (DevFloat)full_xres;
        fix dy = region.GetHeight() / (DevFloat)full_yres;

        fix Δx = dx * (DevFloat)max_res;
        fix Δy = dy * (DevFloat)max_res;

        fix Δx_leftover = dx * DevFloat(full_xres % max_res);
        fix Δy_leftover = dy * DevFloat(full_yres % max_res);

        fix x_min = region.xMin;
        fix y_min = region.yMin;

        blocks.clear();

        assert(n > 0 && m > 0);

        for (int i = 0; i < n; ++i) {
            fix x_res = i == n - 1 ? x_leftover : max_res;
            fix width = i == n - 1 ? Δx_leftover : Δx;
            fix x0 = x_min + i * Δx;


            for (int j = 0; j < m; ++j) {
                fix y_res = j == m - 1 ? y_leftover : max_res;
                fix height = j == m - 1 ? Δy_leftover : Δy;
                fix y0 = y_min + j * Δy;

                RectR sub_region = {x0, x0 + width, y0, y0 + height};

                blocks.emplace_back(New<FieldTextureThingy>(x_res, y_res, sub_region));
            }
        }
    }

    Pointer<FieldTextureThingy> FieldTextureKontraption::getBlock(int i, int j) {
        assert(i < n && j < m);

        return blocks[i + j * n];
    }

    auto FieldTextureKontraption::get_xres() const -> Resolution {
        // TODO: assert below
        //Resolution x_res = 0;
        //for (fix &thingy: blocks)
        //    x_res += thingy->texture->getWidth();
        // assert(x_res==xres);

        return xres;
    }

    auto FieldTextureKontraption::get_yres() const -> Resolution {
        // TODO: assert below
        //Resolution y_res = 0;
        //for (fix &thingy: blocks)
        //    y_res += thingy->texture->getHeight();
        //assert(y_res==yres);

        return yres;
    }

    void FieldTextureKontraption::setValue(int i, int j, DevFloat value) {
        fix max_res = OpenGL::Texture2D::GetMaxTextureSize();

        fix i_block = i/max_res;
        fix j_block = j/max_res;

        fix i_within = i%max_res;
        fix j_within = j%max_res;

        getBlock(i_block, j_block)->texture->setValue(i_within, j_within, (Real32)value);
    }

    bool FieldTextureKontraption::upload(Index j_begin, CountType row_count) {
        fix max_res = OpenGL::Texture2D::GetMaxTextureSize();

        auto j_end = j_begin+row_count;

        fix j_block_begin = j_begin/max_res;
        fix j_block_end   = Common::min(j_end  /max_res, m);

        if(j_block_begin==j_block_end) {
            auto j_block = j_block_begin;
            auto row = j_begin%max_res;

            for(int i_block=0; i_block<n; ++i_block)
                getBlock(i_block, j_block)->texture->upload(row, row_count);

        } else {
            for(int j_block = j_block_begin; j_block<=j_block_end; ++j_block) {
                auto row   = j_block==j_block_begin ?      j_begin%max_res      : 0;
                auto count =  j_block==j_block_end  ? row_count-j_block*max_res : 0; // count=0 uploads all

                for(int i_block=0; i_block<n; ++i_block) {
                    getBlock(i_block, j_block)->texture->upload(row, count);
                }
            }
        }

        return true;
    }

}