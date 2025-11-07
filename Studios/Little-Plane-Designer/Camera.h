//
// Created by joao on 11/6/25.
//

#ifndef STUDIOSLAB_CAMERA_H
#define STUDIOSLAB_CAMERA_H

#include "Graphics/Types2D.h"
#include "Math/Point.h"


struct FCamera
{
    /**
     * Set camera ratio.
     * @param Ratio camera ratio = width / height
     */
    void SetParams_Ratio(float Ratio);
    void SetParams_Width(float Width);

    auto SetCenter(const Slab::Math::Point2D& Center) -> void;

    void Zoom(float ZoomFactor);
    void Pan(Slab::Math::Point2D Delta);

    const Slab::Graphics::RectR& GetView() const { return View; }

private:
    Slab::Graphics::RectR View = Slab::Graphics::RectR(-10.f, 10.f, -10.f, 10.f);
    float AspectRatio = 1.0f;
};


#endif //STUDIOSLAB_CAMERA_H