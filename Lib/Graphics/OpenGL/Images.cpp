//
// Created by joao on 10/23/24.
//

#include "Images.h"

// #include <stb/stb_image.h>

#define STB_IMAGE_IMPLEMENTATION
#include "3rdParty/stb_image.h"


namespace Slab::Graphics{

    Vector<SlabTexture>     m_Textures;

    SlabTextureID LoadTexture(const Str& path) {
        int width = 0, height = 0, component = 0;
        if (auto data = stbi_load(path.c_str(), &width, &height, &component, 4))
        {
            auto texture = CreateTexture(data, width, height);
            stbi_image_free(data);
            return texture;
        }
        else
            return nullptr;
    }

    SlabTextureID CreateTexture(const void *data, int width, int height) {
        m_Textures.resize(m_Textures.size() + 1);
        SlabTexture& texture = m_Textures.back();

        // Upload texture to graphics system
        GLint last_texture = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGenTextures(1, &texture.TextureID);
        glBindTexture(GL_TEXTURE_2D, texture.TextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glBindTexture(GL_TEXTURE_2D, last_texture);

        texture.Width  = width;
        texture.Height = height;

        return reinterpret_cast<SlabTextureID>(static_cast<std::intptr_t>(texture.TextureID));
    }

    void DestroyTexture(SlabTextureID texture)
    {
        auto textureIt = FindTexture(texture);
        if (textureIt == m_Textures.end())
            return;

        glDeleteTextures(1, &textureIt->TextureID);

        m_Textures.erase(textureIt);
    }

    int GetTextureWidth(SlabTextureID texture)
    {
        auto textureIt = FindTexture(texture);
        if (textureIt != m_Textures.end())
            return textureIt->Width;
        return 0;
    }

    int GetTextureHeight(SlabTextureID texture)
    {
        auto textureIt = FindTexture(texture);
        if (textureIt != m_Textures.end())
            return textureIt->Height;
        return 0;
    }

    Vector<SlabTexture>::iterator FindTexture(SlabTextureID texture)
    {
        auto textureID = static_cast<GLuint>(reinterpret_cast<std::intptr_t>(texture));

        return std::find_if(m_Textures.begin(), m_Textures.end(), [textureID](SlabTexture& texture)
        {
            return texture.TextureID == textureID;
        });
    }
}