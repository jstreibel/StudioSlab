//
// Created by joao on 10/20/24.
//

#ifndef STUDIOSLAB_BACKGROUNDARTIST_H
#define STUDIOSLAB_BACKGROUNDARTIST_H

#include "Artist.h"

namespace Slab::Graphics {

    class FBackgroundArtist final : public FArtist {
    public:
        FBackgroundArtist();

        bool Draw(const FPlot2DWindow &window) override;
    };

} // Slab::Graphics

#endif //STUDIOSLAB_BACKGROUNDARTIST_H
