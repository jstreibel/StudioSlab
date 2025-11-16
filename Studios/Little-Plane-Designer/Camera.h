//
// Created by joao on 11/6/25.
//

#ifndef STUDIOSLAB_CAMERA_H
#define STUDIOSLAB_CAMERA_H

#include "Graphics/Types2D.h"
#include "Math/Point.h"
#include "Interfaces.h"
#include "Graphics/Interfaces/IRectProvider.h"


struct FCamera final : IUpdateable, Slab::Graphics::IViewProvider
{
    /**
     * Set camera ratio.
     * @param Ratio camera ratio = width / height
     */
    void SetParams_Ratio(float Ratio);
    void SetParams_BaseWidth(float Width);
    void SetParams_Width(float Width);

    void TrackObject(const Slab::TPointer<IMovingEntity>& Object);

    auto SetCenter(const Slab::Math::Point2D& Center) -> void;

    void Zoom(float ZoomFactor);
    void Pan(Slab::Math::Point2D Delta);

    void Update(float ElapsedTimeMsec) override;
    Slab::Graphics::RectR GetView() const override;

private:
    float BaseWidth = 20.0f;
    Slab::Graphics::RectR View = Slab::Graphics::RectR(-BaseWidth*.5f, BaseWidth*.5f, -BaseWidth*.5f, BaseWidth*.5f);
    float AspectRatio = 1.0f;
    Slab::TPointer<IMovingEntity> TrackedObject;
};


#endif //STUDIOSLAB_CAMERA_H