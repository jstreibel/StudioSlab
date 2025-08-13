//
// Created by joao on 7/2/25.
//

#include "ImGui-Multi-SlabWindow-Wrapper.h"
#include <numeric>

#define CountLessThanZero(vec) std::count_if(begin(vec), end(vec), [](float x) { return x < 0; });
#define SumLargerThanZero(vec) std::accumulate(begin(vec), end(vec), 0.0f, [this](float acc, float x) { return x > 0 ? acc + (x<=1?x:x/Width) : acc; });

namespace Slab::Graphics
{

}
