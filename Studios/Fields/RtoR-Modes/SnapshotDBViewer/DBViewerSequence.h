//
// Created by joao on 7/5/24.
//

#ifndef STUDIOSLAB_DBVIEWERSEQUENCE_H
#define STUDIOSLAB_DBVIEWERSEQUENCE_H

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

    class DBViewerSequence final : public Graphics::FWindowRow {
        Graphics::FGUIWindow guiWindow;
        FWindowRow topRow;

        int current_database = -1;

        Vector<DBParser::Ptr> dbParsers;
        Graphics::FPlot2DWindow mashupDisplay;

        using SnapshotMashup = Math::R2toR::NumericFunction_CPU;
        Vector<Pointer<SnapshotMashup>> allMashups;
        Vector<Graphics::R2toRFunctionArtist_ptr> mashupArtists;
        Vector<Utils::MaxInfo> maxValues;

        Pointer<SnapshotMashup> currentMashup;
        Graphics::R2toRFunctionArtist_ptr currentMeshupArtist;


        Graphics::FPlot2DWindow massesGraph;

        Math::PointSet maxValuesPointSet;
        Math::PointSet massesReal_pointSet;
        Math::PointSet massesImag_pointSet;
        Math::PointSet underXHair;
        Graphics::PointSetArtist_ptr mainModesArtist;

        Pointer<Math::PointSet> KGRelation;
        Graphics::PointSetArtist_ptr KGRelation_artist;

        int index_XHair=-1;

        DevFloat KG_mass = 1.0;
        int masses_avg_samples = 1;
        void updateKGDispersion(bool visible);
        void reloadData();
        void computeMasses();
        void drawTable(int specialIndex) const;

        Graphics::EKeyState shiftKey = Graphics::Release;
    public:
        explicit DBViewerSequence(const StrVector& dbFilenames, const Str &criticalParam);

        void ImmediateDraw(const Graphics::FPlatformWindow&) override;

        bool NotifyKeyboard(Graphics::EKeyMap key, Graphics::EKeyState state, Graphics::EModKeys modKeys) override;
    };
}

#endif //STUDIOSLAB_DBVIEWERSEQUENCE_H
