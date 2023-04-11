//
// Created by joao on 3/25/23.
//

#include "StylesAndColorSchemes.h"


namespace ColorSchemeDark {

    Color graphTitleFont = {1,1,1,1};
    Color graphTicksFont = {1,1,1,.8};
    Color background = {0,0,0,1};

    Color axisColor = {1,1,1,1};
    Color tickColor = {1,1,1,1};

    Color defaultGraphColor = {.9,.9,.9,1};

    Color graph1a = {.65,.65,.99,1};
    Color graph1b = {.8,.2,.2, 1};
    Color graph1c = {.2,.8,.2, 1};

    std::vector<Color> graphs = {graph1a, graph1b, graph1c, {.0,.0,.9,1}};
}

namespace ColorSchemeBWDark {

    Color graphTitleFont = {1,1,1,1};
    Color graphTicksFont = {1,1,1,0.75};
    Color background = {0,0,0,1};

    Color axisColor = {1,1,1,1};
    Color tickColor = {.9,.9,.9,1};

    Color defaultGraphColor = {.9,.9,.9,1};

    Color graph1a = {.5,.5,.5,1};
    Color graph1b = {1,1,1,1};
}

namespace ColorSchemeTest {

    Color graphTitleFont = {1.f, .4f, 1.f, 0.6f};
    Color graphTicksFont = {1.f, .0f, 0.f, 1.0f};
    Color background = {0.35, 0.35, 0.7, 1.0};

    Color axisColor = {0,0,0,1};
    Color tickColor = {0,0,1,1};

    Color defaultGraphColor = {.9,.9,.9,1};

    Color graph1a = {.5,.5,.5,1};
    Color graph1b = {1,1,1,1};

}