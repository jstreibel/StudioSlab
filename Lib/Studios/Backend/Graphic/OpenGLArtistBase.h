//
// Created by joao on 02/09/2021.
//

#ifndef V_SHAPE_OPENGLARTISTBASE_H
#define V_SHAPE_OPENGLARTISTBASE_H


#include <Studios/Graphics/animation.h>
#include <Studios/Util/Utils.h>
#include <Studios/Util/Timer.h>


namespace Base {

    class OpenGLArtistBase {
    public:
        OpenGLArtistBase();
        virtual ~OpenGLArtistBase() = default;

        virtual void notifyGLUTKeyboard(unsigned char key, int x, int y) {};
        virtual void notifyGLUTKeyboardSpecial(int key, int x, int y) {};
        virtual void notifyGLUTMouseButton(int button, int dir, int x, int y) {};
        virtual void notifyGLUTMouseMotion(int x, int y) {};

        virtual auto getWindowSizeHint() -> IntPair;

        virtual void reshape(int width, int height);

        auto finishFrameAndRender() -> bool;

        void addVolatileStat(const String& stat) { stats.push_back(stat); }

    protected:
        int windowWidth, windowHeight;

        virtual auto needDraw() const -> bool;
        virtual void draw() = 0;

        Timer frameTimer = Timer(false);

    protected:
        void SetWindow(double x, double y, double width, double height, const char *title = "") const;

        void renderStats();
        std::vector<String> stats;

        double winXoffset = 4;
        double winYoffset = 4;
        float statsWindowWidth = 600;
        float statsWindowHeight = 620;


    public:
        void addAnimation(Animation *animation) {animations.push_back(animation); }

    private:
        std::vector<Animation*> animations;

    };
}


#endif //V_SHAPE_OPENGLARTISTBASE_H
