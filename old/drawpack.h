#pragma once
#ifndef GEODESUKA_CORE_GCL_DRAWPACK_H
#define GEODESUKA_CORE_GCL_DRAWPACK_H

#include <vulkan/vulkan.h>

#include "context.h"
#include "renderpass.h"
#include "framebuffer.h"

// Forward Declaration.
namespace geodesuka::core::object {
	class render_target;
}

namespace geodesuka::core::gcl {

	class drawpack {
	public:

		context* Context;
		object::render_target* RenderTarget;

		VkResult Result;
		VkRenderPassCreateInfo CreateInfo{};
		VkRenderPass RenderPass;
		// This is created per frame.
		//uint32_t FrameCount;
		VkFramebuffer* Frame;
		VkCommandBuffer* Command;

		drawpack(context *aContext, object::render_target* aRenderTarget, uint32_t aSubpassDescriptionCount, VkSubpassDescription* aSubpassDescriptionList, uint32_t aSubpassDependencyCount, VkSubpassDependency* aSubpassDependencyList);
		~drawpack();

	};

}

#endif // !GEODESUKA_CORE_GCL_DRAWPACK_H
