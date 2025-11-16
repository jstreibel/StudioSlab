//
// Created by joao on 11/1/24.
//

#ifndef STUDIOSLAB_IMAGELOAD_H
#define STUDIOSLAB_IMAGELOAD_H

#include "Utils/Pointer.h"
#include "Utils/Result.h"
#include "Utils/String.h"

namespace Slab::Graphics::Image {

    // The following should be equal to stbi_uc defined in stb_image.h
    using StbiImage = unsigned char;

    struct StbiImageInfo {
        const TPointer<StbiImage> data = nullptr;
        const int width, height, channels;

        bool IsValid() const { return data != nullptr; }
        StbiImageInfo(const TPointer<StbiImage>& d, int w, int h, int c)
            : data(d), width(w), height(h), channels(c) {}
    };

    TResult<StbiImageInfo> LoadImageFile(const Str& image_file);
}

#endif //STUDIOSLAB_IMAGELOAD_H
