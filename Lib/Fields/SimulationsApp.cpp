//
// Created by joao on 04/10/2019.
//

#include "SimulationsApp.h"

#include <Apps/Simulations/OutputStructureBuilders/OutputStructureBuilderBase.h>

#include "Controller/Console/CommandLineInterfaceDefinitions.h"

#include "Core/Util/Workaround/StringStream.h"

#include "Core/Device.h"

#include "Controller/Console/CommandLineInterfaceManager.h"

SimulationsApp::SimulationsApp(int argc, const char **argv)
    : AppBase(argc, argv) { }
