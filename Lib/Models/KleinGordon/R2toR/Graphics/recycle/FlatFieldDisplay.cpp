//
// Created by joao on 4/8/23.
//

#include "FlatFieldDisplay.h"
#include "Core/Backend/Modules/Animator/Animator.h"

#include <utility>
#include <limits>

#define ODD true

Graphics::FlatFieldDisplay::FlatFieldDisplay(Str title)
: ::Graphics::PlottingWindow(-1, 1, -1, 1, std::move(title))
{
}

auto Graphics::FlatFieldDisplay::addFunction(R2toR::Function_constptr function, const Str& name, zOrder_t zOrder)
    -> R2toRFunctionArtist_ptr {
    if(ContainsFunc(function)) return {};

    bool firstTime = funcsMap.empty();

    if(firstTime){
        auto &discrFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*function);
        computeGraphRanges(discrFunc.getDomain());
    }

    auto artist = Slab::New<R2toRFunctionArtist>();
    artist->setLabel(name);
    artist->setFunction(function);
    artist->setColorMap(currColorMap);

    funcsMap.emplace(zOrder, std::move(function));
    ff2dArtists.emplace_back(artist);
    addArtist(artist, zOrder);

    return artist;
}

bool Graphics::FlatFieldDisplay::removeFunction(R2toR::Function_constptr function) {
    if(function== nullptr) return false;

    auto haveErased = false;


    for(auto item = funcsMap.begin(); item!=funcsMap.end();) {
        if(item->second==function){
            item=funcsMap.erase(item);
            haveErased = true;
        }
        else ++item;
    }

    auto newEnd = std::remove_if(
            ff2dArtists.begin(),
            ff2dArtists.end(),
            [&function,this,&haveErased](R2toRFunctionArtist_ptr &x)
            {
                if(x->getFunction() == function){
                    removeArtist(x);
                    haveErased = true;
                    return true;
                };

                return false;
            });

    return haveErased;
}

bool Graphics::FlatFieldDisplay::notifyMouseWheel(double dx, double dy) {

    constexpr const Real factor = 1.1;
    const Real d = pow(factor, -dy);

    static auto targetRegion = getRegion();

    auto &region = getRegion();
    if(!Core::Animator::Contains(region.xMin)
    && !Core::Animator::Contains(region.xMax)
    && !Core::Animator::Contains(region.yMin)
    && !Core::Animator::Contains(region.yMax)) {
        targetRegion = region;
    }

    {
        const Real x0 = targetRegion.xCenter();
        const Real hw = .5 * targetRegion.width() * d;

        targetRegion.xMin = x0-hw;
        targetRegion.xMax = x0+hw;

        set_xMin(targetRegion.xMin);
        set_xMax(targetRegion.xMax);


        const Real y0 = targetRegion.yCenter();
        const Real hh = .5 * targetRegion.height() * d;

        targetRegion.yMin = y0-hh;
        targetRegion.yMax = y0+hh;

        set_yMin(targetRegion.yMin);
        set_yMax(targetRegion.yMax);
    }

    return true;
}

void Graphics::FlatFieldDisplay::notifyReshape(int newWinW, int newWinH) {
    PlottingWindow::notifyReshape(newWinW, newWinH);

    // computeGraphRanges();
}

void Graphics::FlatFieldDisplay::computeGraphRanges(const R2toR::Domain &domain) {

    fix vp = getViewport();

    fix dom_xMin = domain.xMin;
    fix dom_xMax = domain.xMax;
    fix dom_yMin = domain.yMin;
    fix dom_yMax = domain.yMax;

    fix Δx = dom_xMax-dom_xMin;
    fix Δy = dom_yMax-dom_yMin;

    fix windowRatio = (double) vp.height() / vp.width();
    fix fieldRatio = Δy/Δx;
    fix scaleFactor = windowRatio/fieldRatio;

    fix Δx_scaled = Δx/scaleFactor;
    fix xMid = .5*(dom_xMax+dom_xMin);

    let _xMin = xMid - .5*Δx_scaled;
    let _xMax = xMid + .5*Δx_scaled;
    let _yMin = dom_yMin;
    let _yMax = dom_yMin + Δy;

    set_xMin(_xMin);
    set_xMax(_xMax);
    set_yMin(_yMin);
    set_yMax(_yMax);
}

auto Graphics::FlatFieldDisplay::getFunctionsMap() const -> FuncsMap { return funcsMap; }

Str Graphics::FlatFieldDisplay::getXHairLabel(const ::Graphics::Point2D &coords) const {
    auto label = PlottingWindow::getXHairLabel(coords);

    int zOrder_min = std::numeric_limits<int>::min();

    for(auto &[zOrder, func] : funcsMap) {

        if(zOrder < zOrder_min) continue;

        zOrder_min = zOrder;

        auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction &>(*func);

        auto xRes = discreteFunc.getN();
        auto yRes = discreteFunc.getM();

        auto domain = discreteFunc.getDomain();

        auto hPixelSizeInTexCoord = 1. / xRes;
        auto vPixelSizeInTexCoord = 1. / yRes;

        auto hTexturePixelSizeInSpaceCoord = hPixelSizeInTexCoord * domain.getLx();
        auto vTexturePixelSizeInSpaceCoord = vPixelSizeInTexCoord * domain.getLy();

        fix r = Real2D{coords.x, coords.y};
        fix rMin = Real2D{coords.x + .5 * hTexturePixelSizeInSpaceCoord,
                          coords.y + .5 * vTexturePixelSizeInSpaceCoord};
        fix rMax = Real2D{coords.x - .5 * hTexturePixelSizeInSpaceCoord,
                          coords.y - .5 * vTexturePixelSizeInSpaceCoord};
        if (func->domainContainsPoint(r)) {
            fix val = (*func)(r);
            label = "f" + label + " = " + funcUnit(val, 5);
        } else if (func->domainContainsPoint(rMin)) {
            fix val = (*func)(rMin);
            label = "f" + label + " = " + funcUnit(val, 5);
        } else if (func->domainContainsPoint(rMax)) {
            fix val = (*func)(rMax);
            label = "f" + label + " = " + funcUnit(val, 5);
        }
    }

    return label;
}

void Graphics::FlatFieldDisplay::setColorMap(const ColorMap &colorMap) {
    for(auto &artist : ff2dArtists) {
        artist->setColorMap(colorMap);
    }

    currColorMap = colorMap;
}

bool Graphics::FlatFieldDisplay::ContainsFunc(const R2toR::Function_constptr& func){
    typedef Pair<zOrder_t, R2toR::Function_constptr> Pair;
    auto pred = [&func](const Pair &toComp){ return toComp.second==func; };
    return std::find_if(funcsMap.begin(), funcsMap.end(), pred) != funcsMap.end();
}

void Graphics::FlatFieldDisplay::set_xPeriodicOn() {
    for(auto &artist : ff2dArtists)
        artist->set_xPeriodicOn();
}


