//
// Created by joao on 21/09/23.
//

#ifndef STUDIOSLAB_DBVIEWER_H
#define STUDIOSLAB_DBVIEWER_H

#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "DatabaseParser.h"
#include "Graphics/Window/GUIWindow.h"
#include "Maps/R2toR/View/FlatFieldDisplay.h"
#include "Maps/RtoR/View/GraphRtoR.h"

namespace Modes::DatabaseViewer {
    class DBViewer : public Graphics::WindowRow {
        Graphics::GUIWindow guiWindow;
        WindowRow topRow;

        DBParser::Ptr dbParser;
        R2toR::Graphics::FlatFieldDisplay allDataDisplay;
        R2toR::Graphics::FlatFieldDisplay fullParticularHistoryDisplay;

        Graphics::Graph2D massesGraph;

        Spaces::PointSet maxValuesPointSet;
        Spaces::PointSet massesReal_pointSet;
        Spaces::PointSet massesImag_pointSet;
        Spaces::PointSet underXHair;

        std::shared_ptr<R2toR::DiscreteFunction_CPU> fullField;
        std::vector<Utils::MaxInfo> maxValues;

        std::map<Str, std::shared_ptr<R2toR::DiscreteFunction_CPU>> fullHistoriesMap;

        int index_XHair=-1;

        Real KG_mass = 1.0;
        void updateKGDispersion(bool visible);
        void drawDominantModes();
        void reloadData();
        void computeMasses(int avRange);
        void drawTable(int specialIndex);
        void loadDataUnderMouse();

        Core::KeyState shiftKey = Core::Release;
    public:
        explicit DBViewer(DBParser::Ptr dbParser);

        void draw() override;

        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

        bool notifyMouseButton(Core::MouseButton button, Core::KeyState state, Core::ModKeys keys) override;

        bool notifyMouseMotion(int x, int y) override;
    };
}


#endif //STUDIOSLAB_DBVIEWER_H
