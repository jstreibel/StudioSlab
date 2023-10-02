//
// Created by joao on 4/8/23.
//

#include "FlatFieldDisplay.h"
#include "Core/Tools/Animator.h"
#include <utility>

#define ODD true

Graphics::FlatFieldDisplay::FlatFieldDisplay(Str title, Real phiMin, Real phiMax)
: ::Graphics::Graph2D(-1, 1, -1, 1, std::move(title))
, colorBar({50,150, 50, 750})
, flatField2DArtist(phiMin, phiMax)
{
    addArtist(DummyPtr(flatField2DArtist), -10);
    addArtist(DummyPtr(colorBar));
};

void Graphics::FlatFieldDisplay::setFunction(R2toR::Function::ConstPtr function, const Unit& unit) {
    if(func == function) return;

    bool firstTime = (func == nullptr);
    func = std::move(function);
    funcUnit = unit;

    flatField2DArtist.setFunction(func, unit);
    auto &discrFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

    if(firstTime) computeGraphRanges(discrFunc.getDomain());
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
    Graph2D::notifyReshape(newWinW, newWinH);

    colorBar.setLocation({newWinW-100, newWinW-50, newWinH-700, newWinH-200});

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

auto Graphics::FlatFieldDisplay::getFunction() const -> R2toR::Function::ConstPtr { return func; }

Str Graphics::FlatFieldDisplay::getXHairLabel(const ::Graphics::Point2D &coords) const {
    auto label = Graph2D::getXHairLabel(coords);

    if(func == nullptr) return label;

    auto &discreteFunc = dynamic_cast<const R2toR::DiscreteFunction&>(*func);

    auto xRes = discreteFunc.getN();
    auto yRes = discreteFunc.getM();

    auto domain = discreteFunc.getDomain();

    auto hPixelSizeInTexCoord = 1. / xRes;
    auto vPixelSizeInTexCoord = 1. / yRes;

    auto hTexturePixelSizeInSpaceCoord = hPixelSizeInTexCoord * domain.getLx();
    auto vTexturePixelSizeInSpaceCoord = vPixelSizeInTexCoord * domain.getLy();

    fix r = Real2D{coords.x, coords.y};
    fix rMin = Real2D{coords.x+.5*hTexturePixelSizeInSpaceCoord, coords.y+.5*vTexturePixelSizeInSpaceCoord};
    fix rMax = Real2D{coords.x-.5*hTexturePixelSizeInSpaceCoord, coords.y-.5*vTexturePixelSizeInSpaceCoord};
    if(func->domainContainsPoint(r)) {
        fix val = (*func)(r);
        label = "f" + label + " = " + funcUnit(val, 5);
    } else if(func->domainContainsPoint(rMin)) {
        fix val = (*func)(rMin);
        label = "f" + label + " = " + funcUnit(val, 5);
    } else if(func->domainContainsPoint(rMax)) {
        fix val = (*func)(rMax);
        label = "f" + label + " = " + funcUnit(val, 5);
    }

    return label;
}

void Graphics::FlatFieldDisplay::setColorMap(const Styles::ColorMap &colorMap) {
    flatField2DArtist.setColorMap(colorMap);
    colorBar.setTexture(flatField2DArtist.getColorMapTexture());
}

void Graphics::FlatFieldDisplay::set_xPeriodicOn() {
    flatField2DArtist.set_xPeriodicOn();
}


