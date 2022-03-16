
#include "SoftDisk.h"
#include "IO/UserInput.h"
#include "Simulation/SimParams.h"

#include <math.h>


inline Point2D SoftDisk::mdUdr(const Point2D &q1, const Point2D &q2)
{
    const Real SIGMA_SQR = σ * σ;
    Real normSqr;

    const Point2D points[] = {
        q2-q1,
        Point2D(q2.x - SPACE_L, q2.y                   ) - q1,
        Point2D(q2.x + SPACE_L, q2.y                   ) - q1,
        Point2D(   q2.x, q2.y - SPACE_L) - q1,
        Point2D(   q2.x, q2.y + SPACE_L) - q1,

        Point2D(q2.x - SPACE_L, q2.y - SPACE_L) - q1,
        Point2D(q2.x - SPACE_L, q2.y + SPACE_L) - q1,
        Point2D(q2.x + SPACE_L, q2.y - SPACE_L) - q1,
        Point2D(q2.x + SPACE_L, q2.y + SPACE_L) - q1
    };

    Point2D resultForce = {.0, .0};
    for(auto r : points){
        normSqr = r.lengthSqr();

        if(normSqr < SIGMA_SQR){
            const Real norm = r.length();
            const DoubleAccess arg = {1.0- norm / σ};
            const Real mag = ε / σ * pow(arg.val, ALPHA - 1.0) * arg.isPositive();

            const Point2D force = -(mag/norm)*r;

            resultForce = resultForce + force;
        }
    }

    return resultForce;
}

Real SoftDisk::U(const Point2D &q1, const Point2D &q2)
{
    const Real SIGMA_SQR = σ * σ;
    Real normSqr;

    const Point2D points[] = {
            q2-q1,
            /*
            Point2D(q2.x-SIMSPACE_SIDE_SIZE, q2.y)-q1,
            Point2D(q2.x+SIMSPACE_SIDE_SIZE, q2.y)-q1,
            Point2D(q2.x, q2.y-SIMSPACE_SIDE_SIZE)-q1,
            Point2D(q2.x, q2.y+SIMSPACE_SIDE_SIZE)-q1,

            Point2D(q2.x-SIMSPACE_SIDE_SIZE, q2.y-SIMSPACE_SIDE_SIZE)-q1,
            Point2D(q2.x-SIMSPACE_SIDE_SIZE, q2.y+SIMSPACE_SIDE_SIZE)-q1,
            Point2D(q2.x+SIMSPACE_SIDE_SIZE, q2.y-SIMSPACE_SIDE_SIZE)-q1,
            Point2D(q2.x+SIMSPACE_SIDE_SIZE, q2.y+SIMSPACE_SIDE_SIZE)-q1
             */
    };

    for(auto r : points){
        normSqr = r.lengthSqr();
        if(normSqr < SIGMA_SQR){
            const Real norm = sqrt(normSqr);
            const DoubleAccess arg = {1- norm / σ};

            return ε / ALPHA * pow(arg.val, ALPHA) * arg.isPositive();
        }
    }

    return 0.0;
}

void SoftDisk::KeyPressed(sf::Event::KeyEvent &keyEvent, int value) {
    switch (keyEvent.code){
        case sf::Keyboard::Left:
            break;
        case sf::Keyboard::Right:
            break;
        case sf::Keyboard::Up:
            break;
    }
}
