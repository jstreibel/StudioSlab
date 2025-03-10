//
// Created by joao on 11/4/21.
//

#ifndef STUDIOSLAB_CATEGORY_H
#define STUDIOSLAB_CATEGORY_H

#include "Set.h"

#include "MathObject.h"

#include <ostream>
#include "Utils/Types.h"


namespace Slab::Math {

    /**
     * Wikipedia (https://en.wikipedia.org/wiki/Category_(mathematics) , Nov 6th 2021).
     * T
     * Transcription
     *
     *  In mathematics, a category (sometimes called an abstract category to distinguish
     *  it from a concrete category) is a Set of "objects" that are linked by "arrows".
     *  A category has two basic properties: the ability to compose the arrows associatively
     *  and the existence of an identity arrow for each object.
     *
     *  A simple example is the category of sets whose objects are sets and whose arrows are
     *  functions."
     *
     */
    class Category // : public Set<ObjectWithArrows<ObjectWithArrows>>
    {
    public:
        virtual ~Category() = default;

        virtual Str ToString() const { return "<Category::ToString not implemented>"; }
    };

    std::ostream &operator<<(std::ostream &os, const Category &obj);

}

#endif //STUDIOSLAB_CATEGORY_H
