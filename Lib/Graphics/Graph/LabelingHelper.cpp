//
// Created by joao on 31/08/23.
//

#include "LabelingHelper.h"

namespace Math {
    namespace Graphics {
        void LabelingHelper::setTotalItems(Count tot) {
            currItem=0;

            switch (tot) {
                case 1:
                case 2:
                case 3:
                    cols=1;
                    break;
                case 4:
                case 5:
                case 6:
                    cols=2;
                    break;
                case 7:
                case 8:
                case 9:
                    cols=3;
                    break;
                default:
                    cols=4;
            }
        }

        Count LabelingHelper::row() const { return currItem/cols; }
        Count LabelingHelper::col() const { return currItem%cols; }

        LabelingHelper &LabelingHelper::operator++() { currItem++; return *this; }
    } // Graphics
} // Math