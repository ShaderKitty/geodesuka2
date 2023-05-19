#pragma once
#ifndef GEODESUKA_CORE_OBJECT_LIGHT_H
#define GEODESUKA_CORE_OBJECT_LIGHT_H

#include "../object_t.h"

namespace geodesuka::core::object {

	class light : public object_t {
	public:

		enum type {
			AMBIENT,
			DIRECTIONAL,
			SPOT,
			POINT,
		};

		struct color {
			float3 Ambient;
			float3 Diffuse;
			float3 Specular;
		};

		float3 Direction;
		color Color;
		float a0, a1, a2;

	};

}

#endif // !GEODESUKA_CORE_OBJECT_LIGHT_H
