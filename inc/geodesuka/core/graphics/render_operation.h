#pragma once
#ifndef GEODESUKA_CORE_GRAPHICS_RENDER_OPERATION_H
#define GEODESUKA_CORE_GRAPHICS_RENDER_OPERATION_H

#include "../gcl/config.h"

#include "../gcl/command_list.h"

#include "model.h"

// render_operation is designed to be a component
// of object_t which is extensible in describing
// how an object_t wishes to draw itself.
//

namespace geodesuka::core::object {
	class render_target;
}

namespace geodesuka::core::graphics {

	class render_operation {
	public:

		gcl::context*											Context;
		object::render_target*									RenderTarget;
		vk_descriptor_pool										DescriptorPool;
		std::vector<std::vector<vk_descriptor_set>>				DescriptorSet;
		std::vector<gcl::command_list>							DrawCommand;

		~render_operation();

		gcl::command_list& operator[](int aIndex);

	protected:

		render_operation();

	};

}

#endif // !GEODESUKA_CORE_GRAPHICS_RENDER_OPERATION_H
