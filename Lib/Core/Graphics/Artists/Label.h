//
// Created by joao on 01/06/23.
//

#ifndef STUDIOSLAB_LABEL_H
#define STUDIOSLAB_LABEL_H

#include "Common/Types.h"
#include "Artist.h"

struct LabelSource : public Artist {
    virtual ~LabelSource() {}

    virtual const char* get() = 0;
};

struct StringLabelSource : public LabelSource {
public:
    StringLabelSource(Str annotation) : annotation(annotation) {}
    const char* get() { return annotation.c_str(); }

private:
    Str annotation;
} __attribute__((aligned(32)));

struct VariableLabelSource : public LabelSource {
public:
    VariableLabelSource(Str varName, const Real *source)
            : varName(varName), source(source)
    {
        tempBuffer.setf(std::ios::fixed,std::ios::floatfield);
        tempBuffer.precision(6);
    }

    const char* get() {
        tempBuffer.str("");
        tempBuffer << varName << " = " << *source;

        str = tempBuffer.str();

        return str.c_str();
    }
private:
    std::ostringstream tempBuffer;
    Str str;
    char text[256];

    Str varName;
    const Real *source;

};

struct Label{
    Label(Point2D pos, LabelSource *source, bool isPosAbsolute,
          bool keepSource = true)
            : pos(pos), annotation(source), isPosAbsolute(isPosAbsolute),
              keepSource(keepSource)
    {    }
    virtual ~Label(){
        if(keepSource) delete annotation;
    }

protected:
    /*
     * virtual void _draw(const double x, const double y,
                       const double pixelW, const double pixelH) = 0;
                       */
public:
    void draw(const Real Sx, const Real Sy, const Real Tx, const Real Ty) const {
        const Real vTick = Sy;
        const Real hTick = Sx;

        Real x = pos.x, y = pos.y;
        if(this->isPosAbsolute){
            x = x*Sx + Tx;
            y = y*Sy + Ty;
        }

        glColor3f(0,0,0);
        /*
        glBegin(GL_LINES);
        {
            glVertex2f(x, y);
            glVertex2f(x+hTick, y+vTick);
        }
        glEnd();
        */

        throw "Not implemented";
        //write(x+hTick,  y+vTick, this->getText(), GLUT_BITMAP_TIMES_ROMAN_24);
    }

    const char *getText() const { return annotation->get(); }

    Point2D pos;
private:
    LabelSource *annotation;
public:
    bool isPosAbsolute;

    bool keepSource;
};

#endif //STUDIOSLAB_LABEL_H
