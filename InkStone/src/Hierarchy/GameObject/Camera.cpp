#include "pch.h"

#include "Camera.h"

namespace NXTN {
	Camera::Camera(const float& fov, const vec2i& resolution, const bool& isPerspective, float nearPlane, float farPlane)
		: m_FOV(fov), m_IsPerspective(isPerspective), m_FarNear(farPlane, nearPlane)
	{
		m_AspectRatio = resolution.x / (float)resolution.y;

		m_FarNear.y = max(m_FarNear.y, 0.01f);
		m_FarNear.x = max(m_FarNear.x, m_FarNear.y);
	}

	mat4 Camera::GetViewMatrix()
	{
		vec3 position = transform.GetPosition();

		// Z should be, but is not, negated for OpenGL
		vec3 z(normalize(-position));  // Look direction
		vec3 x(normalize(vec3(z.z, 0.0f, -z.x)));  // Cross((0, 1, 0), z)
		vec3 y(normalize(cross(z, x)));

		return mat4(
			x.x, x.y, x.z, -dot(x, position),
			y.x, y.y, y.z, -dot(y, position),
			z.x, z.y, z.z, -dot(z, position),
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	mat4 Camera::GetProjMatrix()
	{
		if (m_IsPerspective)  // Perspective
		{
			return mat4();
		}
		else  // Orthogonal
		{
			// m22 should be, but is not, negated for OpenGL
			return mat4(
				1 / m_FOV / m_AspectRatio, 0.0f, 0.0f, 0.0f,
				0.0f, 1 / m_FOV, 0.0f, 0.0f,
				0.0f, 0.0f, 2.0f / (m_FarNear.x - m_FarNear.y), -(m_FarNear.x + m_FarNear.y) / (m_FarNear.x - m_FarNear.y),
				0.0f, 0.0f, 0.0f, 1.0f
			);
		}
	}

	mat4 Camera::GetViewProjMatrix()
	{
		return mul(GetProjMatrix(), GetViewMatrix());
	}
}
