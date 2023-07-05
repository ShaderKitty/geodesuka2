#pragma once
#ifndef GEODESUKA_CORE_OBJECT_CAMERA3D_H
#define GEODESUKA_CORE_OBJECT_CAMERA3D_H

#include "../object_t.h"
#include "camera.h"

namespace geodesuka::core::object {

	class camera3d : public camera {
	public:

		/*
		* The output of a pixel shader using this as a render target
		* must use the following form.
		* layout (location = 0) out vec4 PixelColor;
		* layout (location = 1) out vec3 PixelPosition;
		* layout (location = 2) out vec3 PixelNormal;
		* layout (location = 3) out vec3 PixelSpecular;
		*/

		struct geometry_buffer {
			gcl::image DepthBuffer;
			gcl::image PixelColor;
			gcl::image PixelPosition;
			gcl::image PixelNormal;
			gcl::image PixelSpecular;
			geometry_buffer(gcl::context* aContext, glm::uvec2 aResolution);
		};

		static constexpr int				ID = 5;

		gcl::pipeline::rasterizer 			Rasterizer;
		std::vector<geometry_buffer> 		GeometryBuffer;

		// ----- object_t methods ----- //

		// ----- rendertarget methods ----- //

		virtual int id() override;

		// ----- camera methods ----- //

		// ----- camera3d methods ----- //

		camera3d(gcl::context* aContext, stage::scene3d* aScene3D, const char* aName, glm::vec3 aPosition, glm::ivec2 aResolution, double aFrameRate, uint32_t aFrameCount);
		//~camera3d();

	protected:

		// ----- object_t methods ----- //

		virtual VkSubmitInfo update(double aDeltaTime) override;

		// ----- rendertarget methods ----- //

		virtual gcl::command_batch render(stage_t* aStage) override;

		//virtual std::vector<gcl::command_list> draw(const std::vector<object_t*>& aObject) override;

	private:

		// Generates the Perspective Projection Matrix.
		float					FieldOfView;
		float					AspectRatio;
		float					MinDistance;
		float					MaxDistance;

		// Orientation Info.
		float					Theta;
		float					Phi;

		// Camera Transform Info.
		glm::mat4				PRT;
		glm::mat4				Projection;
		glm::mat4				Rotation;
		glm::mat4				Translation;

		gcl::buffer 			CameraUniformBuffer;

		// Subpass 0:

		std::vector<VkPipelineColorBlendAttachmentState> BlendingOperation;

		// Opaque Objects:
		// The depth list is a list of sorted objects based
		// on the distance from the camera they are. The opaque
		// objects nearest to the camera will be rendered first.

		// Transparent Objects:
		// Objects that have transparancey will be rendered
		// furthest from the camera first, to nearest to the camera
		// last.
		
		// Translucent Objects:
		// Objects with that have translucency (i.e. Alpha != 1.0) 
		// will be drawn where the furthest objects are rendered 
		// first, and lastly objectes nearest to the camera will
		// be last.

		// These are temporary grouping, only used to generate 
		// command lists.
		object_list OpaqueObject;
		object_list TransparentObject;
		object_list TranslucentObject;

		gcl::command_list *OpaqueObjectCommandList;
		gcl::command_list *TransparentObjectCommandList;
		gcl::command_list *TranslucentObjectCommandList;

		VkResult create_images();

		// Describes render pass
		VkResult create_render_pass();

		VkResult create_pipelines();


	};

}
#endif // !GEODESUKA_CORE_OBJECT_CAMERA3D_H
