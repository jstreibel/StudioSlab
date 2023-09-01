//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_GRAPH_H
#define V_SHAPE_GRAPH_H


#include "Core/Graphics/Utils.h"
#include "Core/Graphics/Artists/Artist.h"

#include "Utils/Utils.h"

#include "Core/Graphics/Window/Window.h"

#include "Core/Graphics/Styles/StylesAndColorSchemes.h"
#include "Core/Graphics/OpenGL/Utils.h"
#include "Graph.h"

#include <memory>


template<class FunctionType>
class FunctionGraph : public Core::Graphics::Graph2D {
    typedef std::unique_ptr<const FunctionType> FunctionPtr;
    typedef std::tuple<const FunctionType*, Styles::PlotStyle, Str> FunctionTriple;
    static auto GetFunction ( FunctionTriple triple ) { return std::get<0>(triple); };
    static auto GetStyle    ( FunctionTriple triple ) { return std::get<1>(triple); };
    static auto GetName     ( FunctionTriple triple ) { return std::get<2>(triple); };

    std::vector<FunctionTriple> mFunctions;

protected:
    auto countDisplayItems() const -> Count override;

    virtual void _renderFunction(const FunctionType *func, Styles::PlotStyle color) = 0;

public:

    FunctionGraph(Real xMin=-1, Real xMax=1, Real yMin=-1, Real yMax=1,
                  Str title = "no_title", bool filled = false, int samples = 512);

    /*!
     * Draw presumes OpenGL model view matrix is identity.
     */
    void draw() override;

    void addFunction(const FunctionType* func, Str name="",
                     Styles::PlotStyle style=Styles::GetColorScheme()->funcPlotStyles[0]);

    void clearFunctions();


};

template<class FunctionType>
auto FunctionGraph<FunctionType>::countDisplayItems() const -> Count { return Graph2D::countDisplayItems() + mFunctions.size(); }


template<class FunctionType>
FunctionGraph<FunctionType>::FunctionGraph(Real xMin, Real xMax, Real yMin, Real yMax, Str title, bool filled, int samples)
        : Core::Graphics::Graph2D(xMin, xMax, yMin, yMax, title, samples) {   }



template<class FunctionType>
void FunctionGraph<FunctionType>::draw() {
    Graph2D::draw();

    for(auto &triple : mFunctions){
        auto &func = *GetFunction(triple);
        auto style = GetStyle(triple);
        auto label = GetName(triple);

        if(label != "") nameLabelDraw(style, label);

        this->_renderFunction(&func, style);
    }
}

template<class FunctionType>
void FunctionGraph<FunctionType>::addFunction(const FunctionType *func, Str name, Styles::PlotStyle style) {
    mFunctions.emplace_back(FunctionTriple{func, style, name});
}

template<class FunctionType>
void FunctionGraph<FunctionType>::clearFunctions() {
    mFunctions.clear();
}


#endif //V_SHAPE_GRAPH_H
