//
// Created by joao on 22/09/23.
//

#ifndef STUDIOSLAB_IMPLEMENTATIONS_H
#define STUDIOSLAB_IMPLEMENTATIONS_H

namespace Core {
    enum BackendImplementation {
        Uninitialized,

        Headless,

        GLFW,
        GLUT,
        SFML,
        VTK
    };
}

#endif //STUDIOSLAB_IMPLEMENTATIONS_H
