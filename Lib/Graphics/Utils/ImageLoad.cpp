//
// Created by joao on 11/1/24.
//

#include "ImageLoad.h"

#include <catch2/internal/catch_clara.hpp>

#include "3rdParty/stb_image.h"
#include "Core/Tools/Log.h"
#include "Graphics/OpenGL/RawTextures.h"

namespace Slab::Graphics::Image {

    TResult<StbiImageInfo> LoadImageFile(const Str& image_file) {
        int width = 0, height = 0, channels_in_file = 0;
        if (const auto data = stbi_load(image_file.c_str(), &width, &height, &channels_in_file, 4))
        {
            Core::Log::Info() << "Loaded image " << image_file;

            return TResult<StbiImageInfo>::OkInPlace(
                TPointer<StbiImage>(data, [](StbiImage* image) { stbi_image_free(image); }),
                width,
                height,
                channels_in_file
            );
        }

        return TResult<StbiImageInfo>::Fail("Failed to load image " + image_file);
    }
}
