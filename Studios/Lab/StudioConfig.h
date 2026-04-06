//
// Created by joao on 8/14/25.
//

#ifndef STUDIOCONFIG_H
#define STUDIOCONFIG_H

class FStudioConfig
{
public:
    static constexpr int SidePaneWidth = 800;
    static constexpr const char* SidePaneId = "##MainWindowSidePane";
};

using StudioConfig [[deprecated("Use FStudioConfig")]] = FStudioConfig;

#endif //STUDIOCONFIG_H
