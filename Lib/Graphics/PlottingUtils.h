#ifndef PLOTTINGUTILS_H
#define PLOTTINGUTILS_H

#include "Core/Util/Workaround/STDLibInclude.h"

#include <GL/gl.h>
#include <GL/glut.h>

#define FONT1 GLUT_STROKE_ROMAN
#define FONT2 GLUT_STROKE_MONO_ROMAN
#define FONT3 GLUT_BITMAP_9_BY_15
#define FONT4 GLUT_BITMAP_8_BY_13
#define FONT5 GLUT_BITMAP_TIMES_ROMAN_10
#define FONT6 GLUT_BITMAP_TIMES_ROMAN_24
#define FONT7 GLUT_BITMAP_HELVETICA_10
#define FONT8 GLUT_BITMAP_HELVETICA_12
#define FONT9 GLUT_BITMAP_HELVETICA_18

#define DEFAULT_FONT FONT6

void write(float x, float y, String str,
                  void *font = DEFAULT_FONT);

class Color
{
public:
    Color(float r, float g, float b, float a=1) : r(r), g(g), b(b), a(a) {}
    float r, g, b, a;
};

struct Point2D
{
    Point2D(float x, float y) : x(x), y(y) {}

    float x, y;
} __attribute__((aligned(8)));

struct AARect2D {
    AARect2D(Point2D topLeft, Point2D bottomRight);

    void setLeft(float xMin);
    void setRight(float xMax);
    void setTop(float yMax);
    void setBottom(float yMin);

    auto width() const -> float;
    auto height() const -> float;

private:
    Point2D topLeft, bottomRight;
} __attribute__((aligned(16)));

struct LabelSource{
    virtual ~LabelSource() {}

    virtual const char* get() = 0;
};

struct StringLabelSource : public LabelSource {
public:
    StringLabelSource(String annotation) : annotation(annotation) {}
    const char* get() { return annotation.c_str(); }

private:
    String annotation;
} __attribute__((aligned(32))) __attribute__((packed));

struct VariableLabelSource : public LabelSource {
public:
    VariableLabelSource(String varName, const double *source)
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
    String str;
    char text[256];

    String varName;
    const double *source;

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
    void draw(const double Sx, const double Sy, const double Tx, const double Ty) const {
        const double vTick = Sy;
        const double hTick = Sx;

        double x = pos.x, y = pos.y;
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

        write(x+hTick,  y+vTick, this->getText(), GLUT_BITMAP_TIMES_ROMAN_24);
    }

    const char *getText() const { return annotation->get(); }

    Point2D pos;
private:
    LabelSource *annotation;
public:
    bool isPosAbsolute;

    bool keepSource;
} __attribute__((aligned(32)));

#endif // PLOTTINGUTILS_H
