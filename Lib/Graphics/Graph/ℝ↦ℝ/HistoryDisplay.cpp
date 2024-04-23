//
// Created by joao on 18/08/23.
//

#include "HistoryDisplay.h"

#include <utility>

#define min(a, b) ((a)<(b)?(a):(b))

#define CHECK_GL_ERRORS(count) OpenGLUtils::checkGLErrors(Str(__PRETTY_FUNCTION__) + " from " + Common::getClassName(this) + " (" + ToStr((count)) + ")");

Graphics::HistoryDisplay::HistoryDisplay(Str name)
: Graphics::FlatFieldDisplay(std::move(name))
{

}

void Graphics::HistoryDisplay::set_t(Real t_) {
    if(helper == nullptr){
        auto firstArtist = ff2dArtists.begin();
        if(firstArtist == ff2dArtists.end()) return;

        auto func = (*firstArtist)->getFunction();
        auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

        auto textureData = (*firstArtist)->getFieldTextureData();

        helper = Pointer(new HistoryTexture2DUploadHelper(DummyPtr(discreteFunc), textureData));
    }

    helper->uploadUpTo(t_);
}


