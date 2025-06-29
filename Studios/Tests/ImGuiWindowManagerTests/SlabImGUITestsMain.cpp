//
// Created by joao on 6/29/25.
//

#include "Application.h"
#include "StudioSlab.h"

class FImGuiTest final : public Slab::FApplication
{
public:
    FImGuiTest(int argc, const char* argv[])
        : FApplication("Slab ImGui WindowManager Tests", argc, argv)
    {

    }
};

int main(int argc, const char** argv)
{
    Slab::Run<FImGuiTest>(argc, argv);

    return 0;
}