//
// Created by joao on 29/04/2021.
//

#include "ChaosGameSet.h"
#include <cmath>

ChaosGame::ChaosGame(float radius, int NSides) {
    const float ang = 2*M_PI/NSides;
    Vector<glm::vec3> points;
    for(int i=0; i<NSides; i++)
        points.push_back({sin(i*ang) * radius, cos(i*ang), .0});

    basePointsSet.initialize(points);
}

void ChaosGame::generatePointSet(const glm::vec3 seed, const int n, bool allowRepeat, float bias) {
    const int bps = basePointsSet.getSize();

    glm::vec3 p = seed;
    const Vector<glm::vec3> &basePointsVec = basePointsSet.getPointSet();
    Vector<glm::vec3> pointSetData;
    int last_i = rand()%bps;
    for(int i=0; i<n; i++){
        int si;
        do{ si = rand()%bps; } while(si == last_i && !allowRepeat);
        last_i = si;

        const glm::vec3 &bp = basePointsVec[si];
        const glm::vec3 midp = (p + bp)*bias;

        pointSetData.emplace_back(midp.x, midp.y, .0);

        p = midp;
    }

    chaosPointSet.initialize(pointSetData);
}

void ChaosGame::draw(const int n) {
    basePointsSet.draw();

    chaosPointSet.setEnd(n);
    chaosPointSet.draw();
}
