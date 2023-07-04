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
			glm::vec3 Ambient;
			glm::vec3 Diffuse;
			glm::vec3 Specular;
		};

		glm::vec3 Direction;
		color Color;
		float a0, a1, a2;

	};

}

#endif // !GEODESUKA_CORE_OBJECT_LIGHT_H
