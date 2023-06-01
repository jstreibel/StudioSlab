//
// Created by joao on 03/09/2021.
//

#ifndef V_SHAPE_GRAPH_H
#define V_SHAPE_GRAPH_H


#include "Base/Graphics/PlottingUtils.h"
#include "Base/Graphics/Artists/Artist.h"

#include "Common/Workaround/StringStream.h"
#include "Common/BinaryToInt.h"

#include "Base/Graphics/WindowManagement/Window.h"

#include "Base/Graphics/Artists/StylesAndColorSchemes.h"
#include "Base/Graphics/OpenGL/Utils.h"
#include "Base/Graphics/Artists/Graph.h"

#include <memory>


template<class FunctionType>
class FunctionGraph : public Base::Graphics::Graph2D {
    typedef std::unique_ptr<const FunctionType> FunctionPtr;
    typedef std::tuple<const FunctionType*, Styles::PlotStyle, String> FunctionTriple;
    static auto GetFunction ( FunctionTriple triple ) { return std::get<0>(triple); };
    static auto GetStyle    ( FunctionTriple triple ) { return std::get<1>(triple); };
    static auto GetName     ( FunctionTriple triple ) { return std::get<2>(triple); };


    std::vector<FunctionTriple> mFunctions;

public:

    FunctionGraph(double xMin=-1, double xMax=1, double yMin=-1, double yMax=1,
                  String title = "no_title", bool filled = false, int samples = 512);

    /*!
     * Draw presumes OpenGL model view matrix is identity.
     */
    void draw(const Window *window) override;

    void addFunction(const FunctionType* func, String name="",
                     Styles::PlotStyle style=Styles::GetColorScheme()->funcPlotStyles[0]);

    void clearFunctions();

protected:

    virtual void _renderFunction(const FunctionType *func, Styles::PlotStyle color) = 0;

};



template<class FunctionType>
FunctionGraph<FunctionType>::FunctionGraph(double xMin, double xMax, double yMin, double yMax, String title, bool filled, int samples)
        : Base::Graphics::Graph2D(xMin, xMax, yMin, yMax, title, filled, samples) {   }



template<class FunctionType>
void FunctionGraph<FunctionType>::draw(const Window *window) {
    Graph2D::draw(window);

    int i=0;
    for(auto &triple : mFunctions){
        auto &func = *GetFunction(triple);
        auto style = GetStyle(triple);
        auto label = GetName(triple);

        if(label != "") _nameLabelDraw(i++, style, label, window);

        this->_renderFunction(&func, style);
    }

}

template<class FunctionType>
void FunctionGraph<FunctionType>::addFunction(const FunctionType *func, String name, Styles::PlotStyle style) {
    mFunctions.emplace_back(FunctionTriple{func, style, name});
}

template<class FunctionType>
void FunctionGraph<FunctionType>::clearFunctions() {
    mFunctions.clear();
}


#endif //V_SHAPE_GRAPH_H
