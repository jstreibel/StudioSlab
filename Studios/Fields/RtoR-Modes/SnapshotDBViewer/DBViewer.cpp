//
// Created by joao on 21/09/23.
//

#include "DBViewer.h"

#include <utility>

namespace Modes::DatabaseViewer {

    DBViewer::DBViewer(DBParser::Ptr dbParser)
    : dbParser(std::move(dbParser))
    , guiWindow()
    {
        this->addWindow(DummyPtr(guiWindow));
    }

    void DBViewer::draw() {
        Str paramName = dbParser->getCriticalParameter();
        for(auto &fileEntry : dbParser->getFileSet()) {
            guiWindow.addVolatileStat(fileEntry.second);
            guiWindow.addVolatileStat(paramName + " = " + ToStr(fileEntry.first), {.3,.3, .92});
        }

        WindowRow::draw();
    }
}