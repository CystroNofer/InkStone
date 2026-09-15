#pragma once

#include "ECS/Component.h"
#include "Math/Math.h"

namespace NXTN {
    class Camera : public BaseComponent
    {
    public:
		// Perspective size is specified in degrees and stored in radians.
		// Orthographic size is the half-height in world units.
        Camera(bool isOrthographic_, float size_, float aspectRatio_, float nearPlane_, float farPlane_);

		void ResizeViewport(float aspectRatio_);

        mat4 GetPMatrix();

        bool isOrthographic;
		float size;  // Vertical FOV in radians for perspective, half-height for ortho
        float aspectRatio;  // W:H
        float nearPlane;
        float farPlane;
    };
}

