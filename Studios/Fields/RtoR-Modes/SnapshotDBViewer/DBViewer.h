//
// Created by joao on 21/09/23.
//

#ifndef STUDIOSLAB_DBVIEWER_H
#define STUDIOSLAB_DBVIEWER_H

#include "DatabaseParser.h"

#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "Graphics/Graph/Artists/PointSetArtist.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Graph/PlottingWindow.h"
#include "Graphics/Graph/Artists/RtoRFunctionArtist.h"
#include "Graphics/Graph/Artists/R2toRFunctionArtist.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Core/Backend/Events/KeyMap.h"

namespace Modes::DatabaseViewer {

    using namespace Slab;

    class DBViewer : public Graphics::WindowRow {
        Graphics::GUIWindow guiWindow;
        WindowRow topRow;

        Vector<DBParser::Ptr> dbParsers;
        Graphics::PlottingWindow allDataDisplay;
        Graphics::PlottingWindow fullParticularHistoryDisplay;
        Graphics::R2toRFunctionArtist_ptr currentFullParticularHistoryArtist;

        Graphics::PlottingWindow massesGraph;

        Math::PointSet maxValuesPointSet;
        Math::PointSet massesReal_pointSet;
        Math::PointSet massesImag_pointSet;
        Math::PointSet underXHair;
        Graphics::PointSetArtist_ptr mainModesArtist;

        Pointer<Math::PointSet> KGRelation;
        Pointer<Math::PointSet> KGRelation_high_k;
        Graphics::PointSetArtist_ptr KGRelation_artist;
        Graphics::PointSetArtist_ptr KGRelation_high_k_artist;

        using FullField_ptr = Pointer<Math::R2toR::NumericFunction_CPU>;
        Vector<FullField_ptr> fullFields;
        Vector<Graphics::R2toRFunctionArtist_ptr> fullFieldsArtist;
        Vector<Utils::MaxInfo> maxValues;

        std::map<Str, Pointer<Math::R2toR::NumericFunction_CPU>> fullHistoriesMap;

        int index_XHair=-1;

        Real KG_mass = 1.0;
        void updateKGDispersion(bool visible);
        void reloadData();
        void computeMasses(int avRange);
        void drawTable(int specialIndex);
        void loadDataUnderMouse();

        Core::KeyState shiftKey = Core::Release;
    public:
        explicit DBViewer(const StrVector& dbFilenames, const Str &criticalParam);

        void draw() override;

        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

        bool notifyMouseButton(Core::MouseButton button, Core::KeyState state, Core::ModKeys keys) override;

        bool notifyMouseMotion(int x, int y) override;
    };
}


#endif //STUDIOSLAB_DBVIEWER_H
