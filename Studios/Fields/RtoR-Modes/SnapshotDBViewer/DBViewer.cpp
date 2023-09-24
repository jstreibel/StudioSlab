//
// Created by joao on 21/09/23.
//

#include "DBViewer.h"
#include "Math/Constants.h"

#include <utility>

namespace Modes::DatabaseViewer {

    DBViewer::DBViewer(DBParser::Ptr _dbParser)
    : dbParser(std::move(_dbParser))
    , guiWindow()
    , allDataDisplay("All data", 0.0, 0.5)
    {
        this->addWindow(DummyPtr(guiWindow));
        this->addWindow(DummyPtr(allDataDisplay), .8);

        auto fullField = dbParser->buildFullField();
        allDataDisplay.setVerticalUnit(Constants::π);
        allDataDisplay.setHorizontalUnit(Constants::π);
        allDataDisplay.setFunction(fullField);
    }

    void DBViewer::draw() {
        Str paramName = dbParser->getCriticalParameter();
        fix unit = Constants::π;
        auto &fileSet = dbParser->getFileSet();
        auto i=1;
        auto slash_n = "/" + ToStr(fileSet.size()) + ": ";
        for(auto &fileEntry : fileSet) {
            // guiWindow.addVolatileStat(fileEntry.second);
            // auto value = ToStr(fileEntry.first);
            // value = unit(fileEntry.first, 2);
            guiWindow.addVolatileStat(ToStr(i) + slash_n + paramName + " = " + unit(fileEntry.first, 4), {.3,.3, .92});
            ++i;
        }

        WindowRow::draw();
    }
}