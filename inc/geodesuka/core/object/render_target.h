#pragma once
#ifndef GEODESUKA_CORE_OBJECT_RENDER_TARGET_H
#define GEODESUKA_CORE_OBJECT_RENDER_TARGET_H

#include <vector>

#include "../gcl/config.h"
#include "../gcl/context.h"
#include "../gcl/image.h"

#include "../gcl/command_list.h"
#include "../gcl/command_pool.h"
#include "../gcl/command_batch.h"

#include "../logic/timer.h"

#include "../object_t.h"
#include "../object_list.h"

/// <summary>
/// render_target is an extendable class which the engine user can use to create custom
/// render targets for whatever the user may need. For something to be considered a render
/// target, it must have frame attachments which can be used as targets for rendering commands
/// by derived object classes.
/// </summary>

namespace geodesuka::core::object {

	class render_target : public object_t {
	public:

		friend class geodesuka::engine;
		friend class core::stage_t;

		// This renderer overide is needed for canvas compositor
		class render_target_renderer : public graphics::render_operation {
		public:

			render_target_renderer(gcl::context* aContext, window* aWindow, render_target* aRenderTarget);

		private:

		};

		struct frame {
			std::vector<gcl::command_list>				DrawCommand;
			std::vector<gcl::image>						Image;
			std::vector<vk_image_view>					Attachment;
			vk_framebuffer								Buffer;
			frame(int aAttachmentCount);
		};

		// ----- Render Target Resources ----- //

		gcl::command_pool							CommandPool;
		double										FrameRate;
		glm::uvec3										FrameResolution; 
		uint32_t									FrameReadIndex;
		uint32_t									FrameDrawIndex;
		std::vector<frame>							Frame;
		std::vector<vk_attachment_description>		AttachmentDescription;

		// ----- Render Target Renderer ----- //
		// Render Target Defined Render Operations. (As opposed to object defined)

		// Default Renderer Info (Filled out by render_target implementations.)
		vk_viewport									DefaultViewport;
		vk_rect_2d 									DefaultScissor;
		vk_rect_2d 									RenderArea;
		vk_render_pass 								RenderPass;
		std::vector<gcl::pipeline> 					Pipeline;

		// ----- Render Target Canvas Variables ----- ///

		// Used for Canvas Rendering of Render Target Contents
		glm::vec2										ScreenPosition;
		glm::vec2										ScreenSize;
		gcl::buffer 								RenderTargetUniformBuffer;

		~render_target();
		
		// Used for runtime rendertarget discrimination.
		virtual int id() = 0;

		// -------------------- Called By stage_t ------------------------- \\

		// When this is called, it will return true if draw commands need
		// to be rebuilt. Example case, is when a system_window has been
		// resized.
		virtual bool refresh_signal();

		// Overridable method that will signal when the engine should
		// commence rendering operations.
		virtual bool render_signal();

		// Must be called by stage backend. Can be overriden for engine user to write
		// stage specific rendering operations.
		//virtual gcl::command_group render(stage_t* aStage);
		virtual gcl::command_batch render(stage_t* aStage);


		// -------------------- Called Internally by render_target ------------------------- \\

		// Will acquire next frame index, if semephore is not VK_NULL_HANDLE, 
		// use as wait semaphore for render operations. This really only applies
		// to a system_window.
		virtual vk_result next_frame();

		// Propose a collection of objects (Most likely from a stage), to 
		// draw those objects to the render target. The objects will
		// produce user implemented draw commands to the rendertarget
		// for aggregation and eventual execution by the geodesuka engine.
		virtual std::vector<gcl::command_list> draw(const std::vector<object_t*>& aObject);

		// This will present 
		// . Must use a semaphore to make presentation
		// dependant on rendering operations to complete.
		virtual vk_present_info_khr present_frame();

		// Calculates total descriptor set bindings for all pipelines in renderer.
		uint32_t descriptor_set_count() const;

		// Calculates total descriptor pool sizes for all pipelines in renderer.
		std::vector<vk_descriptor_pool_size> descriptor_pool_sizes() const;

	protected:

		// Used to back store aggregated draw commands.
		logic::timer FrameRateTimer;

		render_target(gcl::context* aContext, stage_t* aStage, const char* aName, glm::uvec3 aFrameResolution, double aFrameRate, uint32_t aFrameCount, uint32_t aAttachmentCount);

		vk_result create_framebuffers();
		
	private:

	};
}

#endif // !GEODESUKA_CORE_OBJECT_RENDER_TARGET_H
