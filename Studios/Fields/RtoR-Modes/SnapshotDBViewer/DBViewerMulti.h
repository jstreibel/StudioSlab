//
// Created by joao on 21/09/23.
//

#ifndef STUDIOSLAB_DBVIEWERMULTI_H
#define STUDIOSLAB_DBVIEWERMULTI_H

#include "DatabaseParser.h"

#include "Graphics/Window/WindowContainer/WindowRow.h"
#include "Graphics/Plot2D/Artists/PointSetArtist.h"
#include "Graphics/Window/GUIWindow.h"
#include "Graphics/Plot2D/Plot2DWindow.h"
#include "Graphics/Plot2D/Artists/RtoRFunctionArtist.h"
#include "Graphics/Plot2D/Artists/R2toRFunctionArtist.h"
#include "Math/Function/R2toR/Model/R2toRNumericFunctionCPU.h"
#include "Core/Backend/Events/KeyMap.h"

namespace Modes::DatabaseViewer {

    using namespace Slab;

    class DBViewerMulti : public Graphics::WindowRow {
        Graphics::GUIWindow guiWindow;
        WindowRow topRow;

        Vector<DBParser::Ptr> dbParsers;
        Graphics::Plot2DWindow allDataDisplay;
        Graphics::Plot2DWindow fullParticularHistoryDisplay;
        Graphics::R2toRFunctionArtist_ptr currentFullParticularHistoryArtist;

        Graphics::Plot2DWindow massesGraph;

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
        int masses_avg_samples = 10;
        void updateKGDispersion(bool visible);
        void reloadData();
        void computeMasses();
        void drawTable(int specialIndex);
        void loadDataUnderMouse();

        Core::KeyState shiftKey = Core::Release;
    public:
        explicit DBViewerMulti(const StrVector& dbFilenames, const Str &criticalParam);

        void draw() override;

        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;

        bool notifyMouseButton(Core::MouseButton button, Core::KeyState state, Core::ModKeys keys) override;

        bool notifyMouseMotion(int x, int y) override;
    };
}


#endif //STUDIOSLAB_DBVIEWERMULTI_H
