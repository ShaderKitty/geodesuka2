#pragma once
#ifndef GEODESUKA_CORE_GCL_PIPELINE_H
#define GEODESUKA_CORE_GCL_PIPELINE_H

#include <vector>

#include "config.h"

#include "variable.h"
#include "device.h"
#include "context.h"
#include "descriptor.h"
#include "buffer.h"
#include "image.h"
#include "shader.h"

/*
Flow Chart of Pipeline Creation.
Start:		     Vertex       Pixel
			[    Source       Source
Shader		[	    |			|
			[      AST         AST
					  \       / 
			[          Program
Rasterizer	[		  /       \
			[     SPIRV       SPIRV
			        |           |
			[     Module      Module
Device		[         \        / 
			[		   Pipeline
*/

namespace geodesuka::core::gcl {

	class pipeline {
	public:

		/*
		The point of these structure is a straightforward api to map vulkan managed resources,
		to slots indicated by the shaders provided.
		*/

		// Pre creation options for a rasterizer pipeline.
		struct rasterizer {

			friend class pipeline;

			// Vulkan Objects
			vk_pipeline_bind_point								BindPoint;
			vk_pipeline_vertex_input_state_create_info			Input;
			vk_pipeline_input_assembly_state_create_info		InputAssembly;
			vk_pipeline_tessellation_state_create_info			Tesselation;
			vk_pipeline_viewport_state_create_info				Viewport;
			vk_pipeline_rasterization_state_create_info			Rasterizer;
			vk_pipeline_multisample_state_create_info			Multisample;
			vk_pipeline_depth_stencil_state_create_info			DepthStencil;
			vk_pipeline_color_blend_state_create_info			ColorBlend;
			vk_pipeline_dynamic_state_create_info				DynamicState;
			vk_graphics_pipeline_create_info					CreateInfo;

			// Uniform
			uint32_t 											DescriptorSetBindingCount;
			std::vector<vk_descriptor_set_layout_binding> 		DescriptorSetBinding[GCL_DESCRIPTOR_SET_COUNT];
			int													UniformBufferCount;
			variable											UniformBuffer[GCL_MAX_DESCRIPTOR_SET_UNIFORM_BUFFERS];
			int													SamplerCount;
			variable											Sampler[GCL_MAX_DESCRIPTOR_SET_SAMPLERS];
			int													StorageBufferCount;
			variable											StorageBuffer[GCL_MAX_DESCRIPTOR_SET_STORAGE_BUFFERS];
			// Vertex Attribute Input
			//uint32_t											VertexBufferBindingCount;
			vk_vertex_input_binding_description					VertexBufferBindingDescription[GCL_VERTEX_BUFFER_BINDING_COUNT];
			//uint32_t											VertexAttributeCount;
			vk_vertex_input_attribute_description				VertexAttributeDescription[GCL_VERTEX_ATTRIBUTE_COUNT];
			variable											VertexAttributeVariable[GCL_VERTEX_ATTRIBUTE_COUNT];
			// Frame Attachment Output
			uint32_t											AttachmentCount;
			vk_attachment_description							AttachmentDescription[GCL_COLOR_ATTACHMENT_COUNT];
			vk_attachment_reference								AttachmentReference[GCL_COLOR_ATTACHMENT_COUNT];
			variable											AttachmentVariable[GCL_COLOR_ATTACHMENT_COUNT];

			rasterizer();
			rasterizer(uint32_t aShaderCount, shader** aShaderList);
			rasterizer(const rasterizer& aInput);
			~rasterizer();

			rasterizer& operator=(const rasterizer& aRhs);

			void bind(uint32_t aBindingReference, size_t aVertexStride, uint32_t aLocation, size_t aVertexOffset);

			std::vector<vk_descriptor_pool_size> descriptor_pool_size();

			void clear();

		private:

			shader*												Shader[GCL_RASTERIZER_STAGE_COUNT];
			glslang::TProgram*									ProgramHandle;
			std::vector<unsigned int>							SPIRVByteCode[GCL_RASTERIZER_STAGE_COUNT];

			bool create(uint32_t aShaderCount, shader** aShaderList);

			// Set shader stage.
			bool set(shader* aShader);
			bool link_stages();
			bool generate_metadata();
			bool generate_spirv();

			void zero_out();

		};

		// Pre creation options for a raytracer pipeline.
		struct raytracer {

			vk_pipeline_bind_point						BindPoint = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
			vk_ray_tracing_pipeline_create_info_khr		CreateInfo{};

			raytracer();
			//raytracer(uint32_t aShaderCount, shader** aShaderList);

		};

		// Pre creation options for a compute pipeline.
		// Requires on a single compute shader.
		struct compute {

			vk_pipeline_bind_point						BindPoint = VK_PIPELINE_BIND_POINT_COMPUTE;
			vk_compute_pipeline_create_info				CreateInfo{};

			compute();
			//compute(shader* aComputeShader);

		};

		vk_pipeline_bind_point 			BindPoint;
		vk_pipeline_layout 				Layout;
		vk_pipeline_cache 				Cache;
		vk_pipeline 					Handle;

		// Used to construct PipelineLayout and Allocate DescriptorSets.
		uint32_t 						DescriptorSetLayoutCount;
		vk_descriptor_set_layout 		DescriptorSetLayout[GCL_DESCRIPTOR_SET_COUNT];

		pipeline();

		// Creates rasterizer pipeline.
		pipeline(context* aContext, rasterizer& aRasterizer, vk_render_pass aRenderPass, uint32_t aSubpassIndex);

		// Creates raytracer pipeline.
		pipeline(context* aContext, raytracer& aRaytracer);

		// Creates compute pipeline.
		pipeline(context* aContext, compute& aCompute);

		pipeline(const pipeline& aInput);

		pipeline(pipeline&& aInput) noexcept;

		// Destructor
		~pipeline();

		pipeline& operator=(const pipeline& aRhs);
		pipeline& operator=(pipeline&& aRhs) noexcept;

		uint32_t descriptor_set_count() const;
		std::vector<vk_descriptor_pool_size> descriptor_pool_size() const;		

	private:

		// Reference to Host Memory
		rasterizer*											Rasterizer;
		raytracer*											Raytracer;
		compute*											Compute;

		// Pipline Construction.
		context*											Context;
		std::vector<vk_pipeline_shader_stage_create_info> 	Stage;

		vk_graphics_pipeline_create_info					RasterizerCreateInfo;
		vk_ray_tracing_pipeline_create_info_khr				RaytracerCreateInfo;
		vk_compute_pipeline_create_info						ComputeCreateInfo;

		vk_result create(context* aContext, rasterizer& aRasterizer, vk_render_pass aRenderPass, uint32_t aSubpassIndex);

		void clear();

		void zero_out();

	};

	// RenderPass.begin(
}

#endif // !GEODESUKA_CORE_GCL_PIPELINE_H
