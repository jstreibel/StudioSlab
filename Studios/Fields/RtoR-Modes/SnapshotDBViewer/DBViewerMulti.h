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
#include "Graphics/Backend/Events/KeyMap.h"

namespace Modes::DatabaseViewer {

    using namespace Slab;

    class DBViewerMulti : public Graphics::FWindowRow {
        Graphics::FGUIWindow guiWindow;
        FWindowRow topRow;

        Vector<DBParser::Ptr> dbParsers;
        Graphics::FPlot2DWindow allDataDisplay;
        Graphics::FPlot2DWindow fullParticularHistoryDisplay;
        Graphics::R2toRFunctionArtist_ptr currentFullParticularHistoryArtist;

        Graphics::FPlot2DWindow massesGraph;

        Math::FPointSet maxValuesPointSet;
        Math::FPointSet massesReal_pointSet;
        Math::FPointSet massesImag_pointSet;
        Math::FPointSet underXHair;
        Graphics::PointSetArtist_ptr mainModesArtist;

        TPointer<Math::FPointSet> KGRelation;
        TPointer<Math::FPointSet> KGRelation_high_k;
        Graphics::PointSetArtist_ptr KGRelation_artist;
        Graphics::PointSetArtist_ptr KGRelation_high_k_artist;

        using FullField_ptr = TPointer<Math::R2toR::NumericFunction_CPU>;
        Vector<FullField_ptr> fullFields;
        Vector<Graphics::R2toRFunctionArtist_ptr> fullFieldsArtist;
        Vector<Utils::MaxInfo> maxValues;

        std::map<Str, TPointer<Math::R2toR::NumericFunction_CPU>> fullHistoriesMap;

        int index_XHair=-1;

        DevFloat KG_mass = 1.0;
        int masses_avg_samples = 10;
        void updateKGDispersion(bool visible);
        void reloadData();
        void computeMasses();
        void drawTable(int specialIndex);
        void loadDataUnderMouse();

        Graphics::EKeyState shiftKey = Graphics::Release;
    public:
        explicit DBViewerMulti(const StrVector& dbFilenames, const Str &criticalParam);

        void ImmediateDraw(const Graphics::FPlatformWindow&) override;

        bool NotifyKeyboard(Graphics::EKeyMap key, Graphics::EKeyState state, Graphics::EModKeys modKeys) override;

        bool NotifyMouseButton(Graphics::EMouseButton button, Graphics::EKeyState state, Graphics::EModKeys keys) override;

        bool NotifyMouseMotion(int x, int y, int dx, int dy) override;
    };
}


#endif //STUDIOSLAB_DBVIEWERMULTI_H
