//
// Created by joao on 11/6/21.
//

#ifndef STUDIOSLAB_MATHOBJECT_H
#define STUDIOSLAB_MATHOBJECT_H


#include "Information.h"

/*
 * TODO: where do the mathematical formalism of my interest meet C++?
 */


typedef Information MathObject; // A math object is information




/**
 * A Link is a Mathematical object that links two things (aka C++ objects) A and B.
 * @tparam A
 * @tparam B
 */
template <class A, class B>
struct Link {
    A obj1;
    B obj2;

    Link(A obj1, B obj2) : obj1(obj1), obj2(obj2) {};
};

/**
 * An Arrow is a Link with direction between two Mathematical Objects.
 */
template <class A, class B>
struct Arrow : private Link<A, B> {
public:
    Arrow(A from, B to) : Link<A,B>(from, to), from(from), to(to) {};

    A &from;
    B &to;
};


/**
 * A Mathematical Object With Arrows (MOWA) is set of Arrow's in which all arrows
 * point from the same Mathematical Object (the MOWA instance itself) to another Mathematical Object.
 *
 * @tparam ObjectIPointTo
 */

#include "Set.h"
// Some forwards:
template<class ObjectIPointTo> struct MOWA; // A MOWA is Mathematical Object With Arrows/

/*
class ObjectIPointTo;
typedef Arrow<MOWA<ObjectIPointTo*>*, ObjectIPointTo> ArrowFromMe;
template<class ObjectIPointTo>
struct MOWA : public Set<ArrowFromMe> {

    void pointToward(ObjectIPointTo object) {
        arrows.insert(ArrowFromMe(this, object));
    };

private:
    std::set<ArrowFromMe> arrows;
};
*/

/**
 * A MOWA that points to another MOWA is an object that has a Category (because the definition of category is a set of
 * linked MOWA).
 *
 * I'll call it a MOWALink
 */
 /*
class MOWALink;

class MOWALink : MOWA<MOWALink> {

};
*/


#endif //STUDIOSLAB_MATHOBJECT_H
