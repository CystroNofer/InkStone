#pragma once

#include "src/ECS/Component.h"

namespace NXTN {
    class Camera : public BaseComponent
    {
    public:
        Camera(bool isOrthographic_, float size_, float aspectRatio_, float nearPlane_, float farPlane_);

		void ResizeViewport(float size_, float aspectRatio_);

        mat4 GetPMatrix();

        bool isOrthographic;
        float size;  // FOV for perspective, half height for ortho
        float aspectRatio;  // W:H
        float nearPlane;
        float farPlane;
    };
}

