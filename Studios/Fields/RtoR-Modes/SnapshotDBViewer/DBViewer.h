//
// Created by joao on 21/09/23.
//

#ifndef STUDIOSLAB_DBVIEWER_H
#define STUDIOSLAB_DBVIEWER_H

#include "Core/Graphics/Window/WindowContainer/WindowRow.h"
#include "DatabaseParser.h"
#include "Core/Graphics/Window/GUIWindow.h"
#include "Maps/R2toR/View/FlatFieldDisplay.h"

namespace Modes::DatabaseViewer {
    class DBViewer : public WindowRow {
        DBParser::Ptr dbParser;
        GUIWindow guiWindow;
        R2toR::Graphics::FlatFieldDisplay allDataDisplay;
    public:
        explicit DBViewer(DBParser::Ptr dbParser);

        void draw() override;
    };
}


#endif //STUDIOSLAB_DBVIEWER_H
