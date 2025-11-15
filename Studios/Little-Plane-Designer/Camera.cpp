//
// Created by joao on 11/6/25.
//

#include "Camera.h"

#include "Interfaces.h"

void FCamera::SetParams_Ratio(float Ratio) {
    const auto Width = View.GetWidth();
    const auto Height = Width / Ratio;
    const auto y = View.yCenter();

    View.yMin = y - Height*.5f;
    View.yMax = y + Height*.5f;
}

void FCamera::SetParams_Width(const float Width) {
    const auto x = View.xCenter();
    const auto y = View.yCenter();

    View.xMin = x - Width*.5f;
    View.xMax = x + Width*.5f;

    const auto Height = Width / AspectRatio;
    View.yMin = y - Height*.5f;
    View.yMax = y + Height*.5f;
}

void FCamera::TrackObject(const Slab::TPointer<IMovingEntity>& Object) {
    TrackedObject = Object;
}

auto FCamera::SetCenter(const Slab::Math::Point2D& Center) -> void {
    const auto w = View.GetWidth();
    const auto h = View.GetHeight();

    View.xMin = Center.x - w*.5f;
    View.xMax = Center.x + w*.5f;
    View.yMin = Center.y - h*.5f;
    View.yMax = Center.y + h*.5f;
}

void FCamera::Zoom(const float ZoomFactor) {
    const auto NewViewWidth = View.GetWidth() * ZoomFactor;
    const auto NewViewHeight = View.GetHeight() * ZoomFactor;

    const auto xCenter = View.xCenter();
    const auto yCenter = View.yCenter();
    View.xMin = xCenter - NewViewWidth*.5f;
    View.xMax = xCenter + NewViewWidth*.5f;
    View.yMin = yCenter - NewViewHeight*.5f;
    View.yMax = yCenter + NewViewHeight*.5f;
}

void FCamera::Pan(Slab::Math::Point2D Delta) {
    fix dWidth = Delta.x;
    fix dHeight = Delta.y;

    View.xMin += dWidth;
    View.xMax += dWidth;

    View.yMin += dHeight;
    View.yMax += dHeight;
}

void FCamera::Update(float ElapsedTimeMsec) {
    if (TrackedObject == nullptr) return;

    fix w = View.GetWidth();

    auto [x, y] = TrackedObject->GetPosition();
    SetCenter({x-w*.25f, y});
}

Slab::Graphics::RectR FCamera::GetRect() const {
    return View;
}