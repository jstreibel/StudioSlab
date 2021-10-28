//
// Created by joao on 11/09/2019.
//

#ifndef V_SHAPE_UTILS_H
#define V_SHAPE_UTILS_H

#include "Studios/Util/STDLibInclude.h"

#ifdef NDEBUG // Release
#define cast(NAME, TO_TYPE, OBJECT) \
TO_TYPE NAME = static_cast<TO_TYPE>(OBJECT);
#else
#define cast(NAME, TO_TYPE, OBJECT) \
TO_TYPE NAME = dynamic_cast<TO_TYPE>(OBJECT); \
if(&NAME == nullptr) throw "Bad cast.";
#endif


#define let auto
#define letc const auto
#define letcp const auto*
#define letr auto&
#define letcr const auto&


void PrintThere(int x, int y, const char *format, ...);

auto GetDensityChar(float dens) -> char;

void PrintDensityThere(int x, int y, float dens);

template <typename T>
std::string ToString(const T a_value, const int n = 2)
{
    std::ostringstream out;
    out.precision(n);
    out << std::fixed << a_value;
    return out.str();
}


#endif //V_SHAPE_UTILS_H
