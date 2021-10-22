//
// Created by joao on 20/04/2021.
//

#ifndef OPENGLSTUDIES_VECTOR_H
#define OPENGLSTUDIES_VECTOR_H

template<class Type>
class Vector2 {
public:
    Vector2(Type x, Type y) : x(x), y(y) {}

    Type x, y;

    Vector2<Type> operator*(float a){
        return Vector2<Type>(x*a, y*a);
    }

    Vector2<Type> operator+(const Vector2<Type> &b){
        return Vector2<Type>(x*b.x, y*b.y);
    }
};

typedef Vector2<float> Vector2f;
typedef Vector2<double> Vector2d;


#endif //OPENGLSTUDIES_VECTOR_H
