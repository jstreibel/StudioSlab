//
// Created by joao on 11/6/25.
//

#ifndef STUDIOSLAB_CAMERA_H
#define STUDIOSLAB_CAMERA_H

#include "Graphics/Types2D.h"
#include "Math/Point.h"
#include "../Interfaces.h"
#include "Graphics/Interfaces/IRectProvider.h"


struct FTrackerCamera final : IUpdateable, Slab::Graphics::IViewProvider
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
    void Pan(const Slab::Math::Point2D& Delta);

    void Tick(Miliseconds ElapsedTime) override;

    void TogglePause() override;
    float GetWidth() const;

    Slab::Graphics::RectR GetView() const override;

private:
    Slab::Graphics::RectR View {-10.,10.,-10.,10.};

    Slab::TPointer<IMovingEntity> TrackedObject;
    float BaseWidth = 20.0f;
    float AspectRatio = 1.0f;
};


#endif //STUDIOSLAB_CAMERA_H