#pragma once
#ifndef GEODESUKA_CORE_GCL_CONFIG_H
#define GEODESUKA_CORE_GCL_CONFIG_H

// Engine Wide Config File
#include "../../config.h"

// These values are determined by the Specification minimum.
// https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#limits-required
#define GCL_VERTEX_BUFFER_BINDING_COUNT 16
#define GCL_VERTEX_ATTRIBUTE_COUNT 16
#define GCL_COLOR_ATTACHMENT_COUNT 4
#define GCL_DESCRIPTOR_SET_COUNT 4

#define GCL_RASTERIZER_STAGE_COUNT 5
#define GCL_COMPUTE_STAGE_COUNT 1
#define GCL_RAYTRACER_STAGE_COUNT 6

#define GCL_MAX_DESCRIPTOR_SET_SAMPLERS 96
#define GCL_MAX_DESCRIPTOR_SET_UNIFORM_BUFFERS 72
#define GCL_MAX_DESCRIPTOR_SET_STORAGE_BUFFERS 24
//#define GCL_MAX_DESCRIPTOR_SET_SAMPLED_IMAGES 96
#define GCL_MAX_DESCRIPTOR_SET_STORAGE_IMAGES 24
#define GCL_MAX_DESCRIPTOR_SET_INPUT_ATTACHMENTS 4

/* Vulkan API */
#include <vulkan/vulkan.h>
//#include <vulkan/vulkan.hpp>
/* OpenCL API */
//#define CL_TARGET_OPENCL_VERSION GCL_OPENCL_VERSION
//#ifdef __APPLE__
// #include <OpenCL/opencl.h>
// #include <OpenCL/cl_gl.h>
// #include <OpenCL/cl_gl_ext.h>
//#else
// #include <CL/cl.h>
// #include <CL/cl_gl.h>
// #include <CL/cl_gl_ext.h>
//#endif

/* OpenVR API */
//#include <openvr.h>

// Typedefs for vulkan.


namespace vk {}

// snakeified vulkan api
typedef VkResult									vk_result;
typedef VkBool32									vk_bool_32;
typedef VkStructureType								vk_structure_type;
typedef VkSharingMode								vk_sharing_mode;

typedef VkApplicationInfo							vk_application_info;
typedef VkInstanceCreateInfo						vk_instance_create_info;
typedef VkInstance									vk_instance;

typedef VkPhysicalDevice							vk_physical_device;
typedef VkPhysicalDeviceProperties					vk_physical_device_properties;
typedef VkPhysicalDeviceFeatures					vk_physical_device_features;
typedef VkPhysicalDeviceMemoryProperties			vk_physical_device_memory_properties;
typedef VkExtensionProperties						vk_extension_properties;
typedef VkPhysicalDeviceType						vk_physical_device_type;

typedef VkMemoryRequirements						vk_memory_requirements;

typedef VkQueueFamilyProperties						vk_queue_family_properties;
typedef VkQueueFlags								vk_queue_flags;
typedef VkQueue										vk_queue;

typedef VkCommandPoolCreateFlagBits					vk_command_pool_create_flag_bits;
typedef VkCommandPoolCreateInfo						vk_command_pool_create_info;
typedef VkCommandPool								vk_command_pool;

typedef VkCommandBufferAllocateInfo					vk_command_buffer_allocate_info;
typedef VkCommandBufferLevel						vk_command_buffer_level;
typedef VkCommandBuffer								vk_command_buffer;
typedef VkCommandBufferBeginInfo					vk_command_buffer_begin_info;

typedef VkDeviceQueueCreateInfo						vk_device_queue_create_info;
typedef VkDeviceCreateInfo							vk_device_create_info;
typedef VkDevice									vk_device;

typedef VkMemoryPropertyFlags						vk_memory_property_flags;
typedef VkMemoryPropertyFlagBits					vk_memory_property_flag_bits;

typedef VkMemoryAllocateInfo						vk_memory_allocate_info;
typedef VkDeviceMemory								vk_device_memory;

typedef VkBufferUsageFlags							vk_buffer_usage_flags;
typedef VkBufferUsageFlagBits						vk_buffer_usage_flag_bits;
typedef VkBufferCreateInfo							vk_buffer_create_info;
typedef VkBuffer									vk_buffer;
typedef VkBufferView								vk_buffer_view;

typedef VkImageMemoryBarrier						vk_image_memory_barrier;
typedef VkImageCreateInfo							vk_image_create_info;
typedef VkImage										vk_image;
typedef VkImageViewCreateInfo						vk_image_view_create_info;
typedef VkImageView									vk_image_view;
typedef VkImageLayout								vk_image_layout;
typedef VkImageType									vk_image_type;
typedef VkImageBlit									vk_image_blit;
typedef VkImageFormatProperties						vk_image_format_properties;
typedef VkImageUsageFlags							vk_image_usage_flags;
typedef VkImageCreateFlags							vk_image_create_flags;
typedef VkImageAspectFlags							vk_image_aspect_flags;

typedef VkSampler									vk_sampler;

typedef VkBufferCopy								vk_buffer_copy;
typedef VkImageCopy									vk_image_copy;
typedef VkBufferImageCopy							vk_buffer_image_copy;

typedef VkShaderStageFlagBits						vk_shader_stage_flag_bits;
typedef VkShaderStageFlags							vk_shader_stage_flags;
typedef VkShaderModuleCreateInfo					vk_shader_module_create_info;
typedef VkShaderModule								vk_shader_module;

typedef VkAttachmentDescription						vk_attachment_description;
typedef VkAttachmentReference						vk_attachment_reference;
typedef VkSubpassDescription						vk_subpass_description;
typedef VkSubpassDependency							vk_subpass_dependency;
typedef VkRenderPassCreateInfo						vk_render_pass_create_info;
typedef VkRenderPassBeginInfo						vk_render_pass_begin_info;
typedef VkRenderPass								vk_render_pass;

typedef VkFramebufferCreateInfo						vk_framebuffer_create_info;
typedef VkFramebuffer								vk_framebuffer;

typedef VkDescriptorSetLayoutBinding				vk_descriptor_set_layout_binding;
typedef VkDescriptorSetLayoutCreateInfo				vk_descriptor_set_layout_create_info;
typedef VkDescriptorSetLayout						vk_descriptor_set_layout;
typedef VkDescriptorPoolCreateInfo					vk_descriptor_pool_create_info;
typedef VkDescriptorPool							vk_descriptor_pool;
typedef VkDescriptorPoolSize						vk_descriptor_pool_size;
typedef VkDescriptorSetAllocateInfo					vk_descriptor_set_allocate_info;
typedef VkDescriptorSet								vk_descriptor_set;
typedef VkWriteDescriptorSet						vk_write_descriptor_set;

typedef VkDescriptorImageInfo						vk_descriptor_image_info;
typedef VkDescriptorBufferInfo						vk_descriptor_buffer_info;



typedef VkVertexInputBindingDescription				vk_vertex_input_binding_description;
typedef VkVertexInputAttributeDescription			vk_vertex_input_attribute_description;

typedef VkIndexType									vk_index_type;

typedef VkPipelineShaderStageCreateInfo				vk_pipeline_shader_stage_create_info;
typedef VkPipelineVertexInputStateCreateInfo		vk_pipeline_vertex_input_state_create_info;
typedef VkPipelineInputAssemblyStateCreateInfo		vk_pipeline_input_assembly_state_create_info;
typedef VkPipelineTessellationStateCreateInfo		vk_pipeline_tessellation_state_create_info;
typedef VkPipelineViewportStateCreateInfo			vk_pipeline_viewport_state_create_info;
typedef VkPipelineRasterizationStateCreateInfo		vk_pipeline_rasterization_state_create_info;
typedef VkPipelineMultisampleStateCreateInfo		vk_pipeline_multisample_state_create_info;
typedef VkPipelineDepthStencilStateCreateInfo		vk_pipeline_depth_stencil_state_create_info;
typedef VkPipelineColorBlendStateCreateInfo			vk_pipeline_color_blend_state_create_info;
typedef VkPipelineDynamicStateCreateInfo			vk_pipeline_dynamic_state_create_info;
typedef VkPipelineColorBlendAttachmentState			vk_pipeline_color_blend_attachment_state;

typedef VkViewport 									vk_viewport;
typedef VkRect2D 									vk_rect_2d;

typedef VkGraphicsPipelineCreateInfo				vk_graphics_pipeline_create_info;
typedef VkRayTracingPipelineCreateInfoKHR			vk_ray_tracing_pipeline_create_info_khr;
typedef VkComputePipelineCreateInfo					vk_compute_pipeline_create_info;

typedef VkPipelineLayoutCreateInfo					vk_pipeline_layout_create_info;
typedef VkPipelineStageFlags						vk_pipeline_stage_flags;
typedef VkPipelineBindPoint							vk_pipeline_bind_point;
typedef VkPipelineLayout							vk_pipeline_layout;
typedef VkPipelineCache								vk_pipeline_cache;
typedef VkPipeline									vk_pipeline;

typedef VkPrimitiveTopology							vk_primitive_topology;

// ----- Synchronization ----- //

typedef VkFenceCreateInfo							vk_fence_create_info;
typedef VkFenceCreateFlags							vk_fence_create_flags;
typedef VkFence										vk_fence;

typedef VkSemaphoreCreateInfo						vk_semaphore_create_info;
typedef VkSemaphore									vk_semaphore;

// ----- KHR Extensions ----- //

typedef VkSurfaceKHR								vk_surface_khr;
typedef VkSurfaceCapabilitiesKHR					vk_surface_capabilities_khr;
typedef VkSurfaceFormatKHR							vk_surface_format_khr;
typedef VkPresentModeKHR							vk_present_mode_khr;
typedef VkColorSpaceKHR								vk_color_space_khr;
typedef VkCompositeAlphaFlagBitsKHR					vk_composite_alpha_flag_bits_khr;

typedef VkSwapchainCreateInfoKHR					vk_swapchain_create_info_khr;
typedef VkSwapchainKHR								vk_swapchain_khr;

typedef VkSubmitInfo								vk_submit_info;
typedef VkPresentInfoKHR							vk_present_info_khr;

typedef VkExtent3D									vk_extent_3d;
typedef VkExtent2D									vk_extent_2d;

typedef VkFormat									vk_format;

typedef VkSampleCountFlagBits						vk_sample_count_flag_bits;
typedef VkImageTiling								vk_image_tiling;
typedef VkFilter									vk_filter;
typedef VkAccessFlags								vk_access_flags;

typedef VkAttachmentLoadOp							vk_attachment_load_op;
typedef VkAttachmentStoreOp							vk_attachment_store_op;

namespace geodesuka::core::object {
	class render_target;
	class system_display;
}


#endif // !GEODESUKA_CORE_GCL_CONFIG_H

