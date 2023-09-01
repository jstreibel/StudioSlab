//
// Created by joao on 31/08/23.
//

#ifndef STUDIOSLAB_LABELINGHELPER_H
#define STUDIOSLAB_LABELINGHELPER_H

#include "Utils/Types.h"

namespace Math {
    namespace Graphics {

        class LabelingHelper {
            Count currItem=0;
            Count cols = 1;

        public:
            void setTotalItems(Count tot);

            Count row() const;
            Count col() const;

            LabelingHelper& operator++();
        };

    } // Math
} // Graphics

#endif //STUDIOSLAB_LABELINGHELPER_H
