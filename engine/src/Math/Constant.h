#pragma once

namespace NXTN {
	inline constexpr float NXTN_PI_FLOAT = 3.14159265358979323846f;
	inline constexpr double NXTN_PI_DOUBLE = 3.1415926535897932384626433832795;
	inline constexpr float NXTN_2_PI_FLOAT = 2.0f * NXTN_PI_FLOAT;
	inline constexpr double NXTN_2_PI_DOUBLE = 2.0 * NXTN_PI_DOUBLE;
	inline constexpr float NXTN_RAD_2_DEG_FLOAT = 180.0f / NXTN_PI_FLOAT;
	inline constexpr double NXTN_RAD_2_DEG_DOUBLE = 180.0 / NXTN_PI_DOUBLE;
	inline constexpr float NXTN_DEG_2_RAD_FLOAT = NXTN_PI_FLOAT / 180.0f;
	inline constexpr double NXTN_DEG_2_RAD_DOUBLE = NXTN_PI_DOUBLE / 180.0;

	inline constexpr float EPSILON = 1e-6f;
	inline constexpr float EPSILON_SQ = 1e-12f;
}
