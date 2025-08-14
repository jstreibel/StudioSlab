//
// Created by joao on 15/10/2019.
//

#include "Util.h"

#include <Utils/Utils.h>

#include <iostream>

void PrintSpace(const DiscreteSpace& space, UInt resolution, DevFloat scale){
    if(space.getDim().getNDim() == 1){
        const DevFloat ratio = DevFloat(resolution)/space.getDim()[0];
        for(int i=0; i<resolution; i++){
            std::cout << GetDensityChar(scale*abs(space.getX()[Real(i)/ratio]));
        }
    } else {
        throw "I dunno how to print this space";
    }


}