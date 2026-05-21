#include "pch.h"

#include "Camera.h"

namespace NXTN {
	Camera::Camera(bool isOrthographic_, float size_, float aspectRatio_, float nearPlane_, float farPlane_)
		: isOrthographic(isOrthographic_), size(size_), aspectRatio(aspectRatio_), nearPlane(nearPlane_), farPlane(farPlane_)
	{

	}

	void Camera::ResizeViewport(float size_, float aspectRatio_) {
		size = size_;
		aspectRatio = aspectRatio_;
	}

	mat4 Camera::GetPMatrix() {
		float invFarMNear = 1.0f / (farPlane - nearPlane);

		if (isOrthographic)  // Orthogonal
		{
			return mat4(
				1 / (size * aspectRatio), 0.0f, 0.0f, 0.0f,
				0.0f, 1 / size, 0.0f, 0.0f,
				// NDC Z in [0, 1] for engine side cliping
				// API-specific fixes in Renderer
				0.0f, 0.0f, invFarMNear, -nearPlane * invFarMNear,
				0.0f, 0.0f, 0.0f, 1.0f
			);
		}
		else  // Perspective
		{
			float invTanHFOV = 1 / std::tan(0.5f * size);
			return mat4(
				invTanHFOV / aspectRatio, 0.0f, 0.0f, 0.0f,
				0.0f, invTanHFOV, 0.0f, 0.0f,
				0.0f, 0.0f,	farPlane * invFarMNear, -farPlane * nearPlane * invFarMNear,
				0.0f, 0.0f, 1.0f, 0.0f
			);
		}
	}
}
