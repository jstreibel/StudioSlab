//
// Created by joao on 7/5/24.
//

#ifndef STUDIOSLAB_DBVIEWERSEQUENCE_H
#define STUDIOSLAB_DBVIEWERSEQUENCE_H

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

    class DBViewerSequence : public Graphics::WindowRow {
        Graphics::GUIWindow guiWindow;
        WindowRow topRow;

        int current_database = -1;

        Vector<DBParser::Ptr> dbParsers;
        Graphics::PlottingWindow mashupDisplay;

        using SnapshotMashup = Math::R2toR::NumericFunction_CPU;
        Vector<Pointer<SnapshotMashup>> allMashups;
        Vector<Graphics::R2toRFunctionArtist_ptr> mashupArtists;
        Vector<Utils::MaxInfo> maxValues;

        Pointer<SnapshotMashup> currentMashup;
        Graphics::R2toRFunctionArtist_ptr currentMeshupArtist;


        Graphics::PlottingWindow massesGraph;

        Math::PointSet maxValuesPointSet;
        Math::PointSet massesReal_pointSet;
        Math::PointSet massesImag_pointSet;
        Math::PointSet underXHair;
        Graphics::PointSetArtist_ptr mainModesArtist;

        Pointer<Math::PointSet> KGRelation;
        Graphics::PointSetArtist_ptr KGRelation_artist;

        int index_XHair=-1;

        Real KG_mass = 1.0;
        int masses_avg_samples = 1;
        void updateKGDispersion(bool visible);
        void reloadData();
        void computeMasses();
        void drawTable(int specialIndex);

        Core::KeyState shiftKey = Core::Release;
    public:
        explicit DBViewerSequence(const StrVector& dbFilenames, const Str &criticalParam);

        void draw() override;

        bool notifyKeyboard(Core::KeyMap key, Core::KeyState state, Core::ModKeys modKeys) override;
    };
}

#endif //STUDIOSLAB_DBVIEWERSEQUENCE_H
