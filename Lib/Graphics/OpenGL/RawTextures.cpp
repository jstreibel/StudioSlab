//
// Created by joao on 10/23/24.
//

#include "RawTextures.h"

// #include <stb/stb_image.h>

#define STB_IMAGE_IMPLEMENTATION
#include "3rdParty/stb_image.h"
#include "Graphics/Utils/ImageLoad.h"


namespace Slab::Graphics{

    Vector<SlabTexture> m_Textures;

    SlabTextureID SlabTexture::CreateTexture(Image::StbiImageInfo image_info) {
        m_Textures.resize(m_Textures.size() + 1);
        auto& [TextureID, Width, Height] = m_Textures.back();

        // Upload texture to graphics system
        GLint last_texture = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGenTextures(1, &TextureID);
        glBindTexture(GL_TEXTURE_2D, TextureID);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,
            0,
            GL_RGBA,
            image_info.width,
            image_info.height,
            0, GL_RGBA, GL_UNSIGNED_BYTE, image_info.data.get());
        glBindTexture(GL_TEXTURE_2D, last_texture);

        Width  = image_info.width;
        Height = image_info.height;

        return reinterpret_cast<SlabTextureID>(static_cast<std::intptr_t>(TextureID));
    }

    SlabTextureID SlabTexture::LoadTextureFromImage(const Str& path) {

        if (const auto data = Image::LoadImageFile(path); data.IsValid())
            return CreateTexture(data);

        return nullptr;
    }

    void SlabTexture::DestroyTexture(SlabTextureID texture)
    {
        auto textureIt = FindTexture(texture);
        if (textureIt == m_Textures.end())
            return;

        glDeleteTextures(1, &textureIt->TextureID);

        m_Textures.erase(textureIt);
    }

    int SlabTexture::GetTextureWidth(SlabTextureID texture)
    {
        auto textureIt = FindTexture(texture);
        if (textureIt != m_Textures.end())
            return textureIt->Width;
        return 0;
    }

    int SlabTexture::GetTextureHeight(SlabTextureID texture)
    {
        auto textureIt = FindTexture(texture);
        if (textureIt != m_Textures.end())
            return textureIt->Height;
        return 0;
    }

    Vector<SlabTexture>::iterator SlabTexture::FindTexture(SlabTextureID texture)
    {
        auto textureID = static_cast<GLuint>(reinterpret_cast<std::intptr_t>(texture));

        return std::find_if(m_Textures.begin(), m_Textures.end(), [textureID](SlabTexture& texture)
        {
            return texture.TextureID == textureID;
        });
    }
}