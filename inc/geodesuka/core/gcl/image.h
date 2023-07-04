#pragma once
#ifndef GEODESUKA_CORE_GCL_IMAGE_H
#define GEODESUKA_CORE_GCL_IMAGE_H

#include "../../config.h"

#include "../io/file.h"

#include "variable.h"

#include "device.h"
#include "context.h"

#include "buffer.h"

namespace geodesuka::core::object {
	class system_window;
}

namespace geodesuka::core::gcl {

	class image : public io::file {
	public:

		friend class geodesuka::engine;
		friend class buffer;
		friend class object::system_window;

		enum sample {
			COUNT_1 = 0x00000001,
			COUNT_2 = 0x00000002,
			COUNT_4 = 0x00000004,
			COUNT_8 = 0x00000008,
			COUNT_16 = 0x00000010,
			COUNT_32 = 0x00000020,
			COUNT_64 = 0x00000040
		};

		enum tiling {
			OPTIMAL = 0,
			LINEAR = 1,
		};

		enum usage {
			TRANSFER_SRC = 0x00000001,
			TRANSFER_DST = 0x00000002,
			SAMPLED = 0x00000004,
			STORAGE = 0x00000008,
			COLOR_ATTACHMENT = 0x00000010,
			DEPTH_STENCIL_ATTACHMENT = 0x00000020,
			TRANSIENT_ATTACHMENT = 0x00000040,
			INPUT_ATTACHMENT = 0x00000080
		};

		enum format {
			UNDEFINED = 0,
			R4G4_UNORM_PACK8 = 1,
			R4G4B4A4_UNORM_PACK16 = 2,
			B4G4R4A4_UNORM_PACK16 = 3,
			R5G6B5_UNORM_PACK16 = 4,
			B5G6R5_UNORM_PACK16 = 5,
			R5G5B5A1_UNORM_PACK16 = 6,
			B5G5R5A1_UNORM_PACK16 = 7,
			A1R5G5B5_UNORM_PACK16 = 8,
			R8_UNORM = 9,
			R8_SNORM = 10,
			R8_USCALED = 11,
			R8_SSCALED = 12,
			R8_UINT = 13,
			R8_SINT = 14,
			R8_SRGB = 15,
			R8G8_UNORM = 16,
			R8G8_SNORM = 17,
			R8G8_USCALED = 18,
			R8G8_SSCALED = 19,
			R8G8_UINT = 20,
			R8G8_SINT = 21,
			R8G8_SRGB = 22,
			R8G8B8_UNORM = 23,
			R8G8B8_SNORM = 24,
			R8G8B8_USCALED = 25,
			R8G8B8_SSCALED = 26,
			R8G8B8_UINT = 27,
			R8G8B8_SINT = 28,
			R8G8B8_SRGB = 29,
			B8G8R8_UNORM = 30,
			B8G8R8_SNORM = 31,
			B8G8R8_USCALED = 32,
			B8G8R8_SSCALED = 33,
			B8G8R8_UINT = 34,
			B8G8R8_SINT = 35,
			B8G8R8_SRGB = 36,
			R8G8B8A8_UNORM = 37,
			R8G8B8A8_SNORM = 38,
			R8G8B8A8_USCALED = 39,
			R8G8B8A8_SSCALED = 40,
			R8G8B8A8_UINT = 41,
			R8G8B8A8_SINT = 42,
			R8G8B8A8_SRGB = 43,
			B8G8R8A8_UNORM = 44,
			B8G8R8A8_SNORM = 45,
			B8G8R8A8_USCALED = 46,
			B8G8R8A8_SSCALED = 47,
			B8G8R8A8_UINT = 48,
			B8G8R8A8_SINT = 49,
			B8G8R8A8_SRGB = 50,
			A8B8G8R8_UNORM_PACK32 = 51,
			A8B8G8R8_SNORM_PACK32 = 52,
			A8B8G8R8_USCALED_PACK32 = 53,
			A8B8G8R8_SSCALED_PACK32 = 54,
			A8B8G8R8_UINT_PACK32 = 55,
			A8B8G8R8_SINT_PACK32 = 56,
			A8B8G8R8_SRGB_PACK32 = 57,
			A2R10G10B10_UNORM_PACK32 = 58,
			A2R10G10B10_SNORM_PACK32 = 59,
			A2R10G10B10_USCALED_PACK32 = 60,
			A2R10G10B10_SSCALED_PACK32 = 61,
			A2R10G10B10_UINT_PACK32 = 62,
			A2R10G10B10_SINT_PACK32 = 63,
			A2B10G10R10_UNORM_PACK32 = 64,
			A2B10G10R10_SNORM_PACK32 = 65,
			A2B10G10R10_USCALED_PACK32 = 66,
			A2B10G10R10_SSCALED_PACK32 = 67,
			A2B10G10R10_UINT_PACK32 = 68,
			A2B10G10R10_SINT_PACK32 = 69,
			R16_UNORM = 70,
			R16_SNORM = 71,
			R16_USCALED = 72,
			R16_SSCALED = 73,
			R16_UINT = 74,
			R16_SINT = 75,
			R16_SFLOAT = 76,
			R16G16_UNORM = 77,
			R16G16_SNORM = 78,
			R16G16_USCALED = 79,
			R16G16_SSCALED = 80,
			R16G16_UINT = 81,
			R16G16_SINT = 82,
			R16G16_SFLOAT = 83,
			R16G16B16_UNORM = 84,
			R16G16B16_SNORM = 85,
			R16G16B16_USCALED = 86,
			R16G16B16_SSCALED = 87,
			R16G16B16_UINT = 88,
			R16G16B16_SINT = 89,
			R16G16B16_SFLOAT = 90,
			R16G16B16A16_UNORM = 91,
			R16G16B16A16_SNORM = 92,
			R16G16B16A16_USCALED = 93,
			R16G16B16A16_SSCALED = 94,
			R16G16B16A16_UINT = 95,
			R16G16B16A16_SINT = 96,
			R16G16B16A16_SFLOAT = 97,
			R32_UINT = 98,
			R32_SINT = 99,
			R32_SFLOAT = 100,
			R32G32_UINT = 101,
			R32G32_SINT = 102,
			R32G32_SFLOAT = 103,
			R32G32B32_UINT = 104,
			R32G32B32_SINT = 105,
			R32G32B32_SFLOAT = 106,
			R32G32B32A32_UINT = 107,
			R32G32B32A32_SINT = 108,
			R32G32B32A32_SFLOAT = 109,
			R64_UINT = 110,
			R64_SINT = 111,
			R64_SFLOAT = 112,
			R64G64_UINT = 113,
			R64G64_SINT = 114,
			R64G64_SFLOAT = 115,
			R64G64B64_UINT = 116,
			R64G64B64_SINT = 117,
			R64G64B64_SFLOAT = 118,
			R64G64B64A64_UINT = 119,
			R64G64B64A64_SINT = 120,
			R64G64B64A64_SFLOAT = 121,
			B10G11R11_UFLOAT_PACK32 = 122,
			E5B9G9R9_UFLOAT_PACK32 = 123,
			D16_UNORM = 124,
			X8_D24_UNORM_PACK32 = 125,
			D32_SFLOAT = 126,
			S8_UINT = 127,
			D16_UNORM_S8_UINT = 128,
			D24_UNORM_S8_UINT = 129,
			D32_SFLOAT_S8_UINT = 130,
			BC1_RGB_UNORM_BLOCK = 131,
			BC1_RGB_SRGB_BLOCK = 132,
			BC1_RGBA_UNORM_BLOCK = 133,
			BC1_RGBA_SRGB_BLOCK = 134,
			BC2_UNORM_BLOCK = 135,
			BC2_SRGB_BLOCK = 136,
			BC3_UNORM_BLOCK = 137,
			BC3_SRGB_BLOCK = 138,
			BC4_UNORM_BLOCK = 139,
			BC4_SNORM_BLOCK = 140,
			BC5_UNORM_BLOCK = 141,
			BC5_SNORM_BLOCK = 142,
			BC6H_UFLOAT_BLOCK = 143,
			BC6H_SFLOAT_BLOCK = 144,
			BC7_UNORM_BLOCK = 145,
			BC7_SRGB_BLOCK = 146,
			ETC2_R8G8B8_UNORM_BLOCK = 147,
			ETC2_R8G8B8_SRGB_BLOCK = 148,
			ETC2_R8G8B8A1_UNORM_BLOCK = 149,
			ETC2_R8G8B8A1_SRGB_BLOCK = 150,
			ETC2_R8G8B8A8_UNORM_BLOCK = 151,
			ETC2_R8G8B8A8_SRGB_BLOCK = 152,
			EAC_R11_UNORM_BLOCK = 153,
			EAC_R11_SNORM_BLOCK = 154,
			EAC_R11G11_UNORM_BLOCK = 155,
			EAC_R11G11_SNORM_BLOCK = 156,
			ASTC_4x4_UNORM_BLOCK = 157,
			ASTC_4x4_SRGB_BLOCK = 158,
			ASTC_5x4_UNORM_BLOCK = 159,
			ASTC_5x4_SRGB_BLOCK = 160,
			ASTC_5x5_UNORM_BLOCK = 161,
			ASTC_5x5_SRGB_BLOCK = 162,
			ASTC_6x5_UNORM_BLOCK = 163,
			ASTC_6x5_SRGB_BLOCK = 164,
			ASTC_6x6_UNORM_BLOCK = 165,
			ASTC_6x6_SRGB_BLOCK = 166,
			ASTC_8x5_UNORM_BLOCK = 167,
			ASTC_8x5_SRGB_BLOCK = 168,
			ASTC_8x6_UNORM_BLOCK = 169,
			ASTC_8x6_SRGB_BLOCK = 170,
			ASTC_8x8_UNORM_BLOCK = 171,
			ASTC_8x8_SRGB_BLOCK = 172,
			ASTC_10x5_UNORM_BLOCK = 173,
			ASTC_10x5_SRGB_BLOCK = 174,
			ASTC_10x6_UNORM_BLOCK = 175,
			ASTC_10x6_SRGB_BLOCK = 176,
			ASTC_10x8_UNORM_BLOCK = 177,
			ASTC_10x8_SRGB_BLOCK = 178,
			ASTC_10x10_UNORM_BLOCK = 179,
			ASTC_10x10_SRGB_BLOCK = 180,
			ASTC_12x10_UNORM_BLOCK = 181,
			ASTC_12x10_SRGB_BLOCK = 182,
			ASTC_12x12_UNORM_BLOCK = 183,
			ASTC_12x12_SRGB_BLOCK = 184,
			G8B8G8R8_422_UNORM = 1000156000,
			B8G8R8G8_422_UNORM = 1000156001,
			G8_B8_R8_3PLANE_420_UNORM = 1000156002,
			G8_B8R8_2PLANE_420_UNORM = 1000156003,
			G8_B8_R8_3PLANE_422_UNORM = 1000156004,
			G8_B8R8_2PLANE_422_UNORM = 1000156005,
			G8_B8_R8_3PLANE_444_UNORM = 1000156006,
			R10X6_UNORM_PACK16 = 1000156007,
			R10X6G10X6_UNORM_2PACK16 = 1000156008,
			R10X6G10X6B10X6A10X6_UNORM_4PACK16 = 1000156009,
			G10X6B10X6G10X6R10X6_422_UNORM_4PACK16 = 1000156010,
			B10X6G10X6R10X6G10X6_422_UNORM_4PACK16 = 1000156011,
			G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16 = 1000156012,
			G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16 = 1000156013,
			G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16 = 1000156014,
			G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16 = 1000156015,
			G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16 = 1000156016,
			R12X4_UNORM_PACK16 = 1000156017,
			R12X4G12X4_UNORM_2PACK16 = 1000156018,
			R12X4G12X4B12X4A12X4_UNORM_4PACK16 = 1000156019,
			G12X4B12X4G12X4R12X4_422_UNORM_4PACK16 = 1000156020,
			B12X4G12X4R12X4G12X4_422_UNORM_4PACK16 = 1000156021,
			G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16 = 1000156022,
			G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16 = 1000156023,
			G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16 = 1000156024,
			G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16 = 1000156025,
			G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16 = 1000156026,
			G16B16G16R16_422_UNORM = 1000156027,
			B16G16R16G16_422_UNORM = 1000156028,
			G16_B16_R16_3PLANE_420_UNORM = 1000156029,
			G16_B16R16_2PLANE_420_UNORM = 1000156030,
			G16_B16_R16_3PLANE_422_UNORM = 1000156031,
			G16_B16R16_2PLANE_422_UNORM = 1000156032,
			G16_B16_R16_3PLANE_444_UNORM = 1000156033,
			G8_B8R8_2PLANE_444_UNORM = 1000330000,
			G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16 = 1000330001,
			G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16 = 1000330002,
			G16_B16R16_2PLANE_444_UNORM = 1000330003,
			A4R4G4B4_UNORM_PACK16 = 1000340000,
			A4B4G4R4_UNORM_PACK16 = 1000340001,
			ASTC_4x4_SFLOAT_BLOCK = 1000066000,
			ASTC_5x4_SFLOAT_BLOCK = 1000066001,
			ASTC_5x5_SFLOAT_BLOCK = 1000066002,
			ASTC_6x5_SFLOAT_BLOCK = 1000066003,
			ASTC_6x6_SFLOAT_BLOCK = 1000066004,
			ASTC_8x5_SFLOAT_BLOCK = 1000066005,
			ASTC_8x6_SFLOAT_BLOCK = 1000066006,
			ASTC_8x8_SFLOAT_BLOCK = 1000066007,
			ASTC_10x5_SFLOAT_BLOCK = 1000066008,
			ASTC_10x6_SFLOAT_BLOCK = 1000066009,
			ASTC_10x8_SFLOAT_BLOCK = 1000066010,
			ASTC_10x10_SFLOAT_BLOCK = 1000066011,
			ASTC_12x10_SFLOAT_BLOCK = 1000066012,
			ASTC_12x12_SFLOAT_BLOCK = 1000066013,
			PVRTC1_2BPP_UNORM_BLOCK_IMG = 1000054000,
			PVRTC1_4BPP_UNORM_BLOCK_IMG = 1000054001,
			PVRTC2_2BPP_UNORM_BLOCK_IMG = 1000054002,
			PVRTC2_4BPP_UNORM_BLOCK_IMG = 1000054003,
			PVRTC1_2BPP_SRGB_BLOCK_IMG = 1000054004,
			PVRTC1_4BPP_SRGB_BLOCK_IMG = 1000054005,
			PVRTC2_2BPP_SRGB_BLOCK_IMG = 1000054006,
			PVRTC2_4BPP_SRGB_BLOCK_IMG = 1000054007,
		};

		struct create_info {
			int Sample;
			int Tiling;
			int Memory;
			int Usage;
			create_info();
			create_info(int aSample, int aTiling, int aMemory, int aUsage);
		};

		// Will yield the number of bits per pixel.
		static format t2f(type::id aID);
		static size_t bytesperpixel(int aFormat);
		static size_t bitsperpixel(int aFormat);
		static vk_image_aspect_flags aspect_flag(int aFormat);

		// -------------------- Host Memory -------------------- //

		// Host memory images.
		image();
		image(const util::string& aFilePath);
		image(const char* aFilePath);

		image(type::id aTypeID, unsigned int aResolution);
		image(type::id aTypeID, glm::uvec2 aResolution);
		image(type::id aTypeID, glm::uvec3 aResolution);
		image(unsigned int aArrayLayers, type::id aTypeID, unsigned int aResolution);
		image(unsigned int aArrayLayers, type::id aTypeID, glm::uvec2 aResolution);
		image(unsigned int aArrayLayers, type::id aTypeID, glm::uvec3 aResolution);

		image(type::id aTypeID, unsigned int aResolution, void* aTextureData);
		image(type::id aTypeID, glm::uvec2 aResolution, void* aTextureData);
		image(type::id aTypeID, glm::uvec3 aResolution, void* aTextureData);
		image(unsigned int aArrayLayers, type::id aTypeID, unsigned int aResolution, void* aTextureData);
		image(unsigned int aArrayLayers, type::id aTypeID, glm::uvec2 aResolution, void* aTextureData);
		image(unsigned int aArrayLayers, type::id aTypeID, glm::uvec3 aResolution, void* aTextureData);

		image(format aFormat, unsigned int aResolution);
		image(format aFormat, glm::uvec2 aResolution);
		image(format aFormat, glm::uvec3 aResolution);
		image(unsigned int aArrayLayers, format aFormat, unsigned int aResolution);
		image(unsigned int aArrayLayers, format aFormat, glm::uvec2 aResolution);
		image(unsigned int aArrayLayers, format aFormat, glm::uvec3 aResolution);

		image(format aFormat, unsigned int aResolution, void* aTextureData);
		image(format aFormat, glm::uvec2 aResolution, void* aTextureData);
		image(format aFormat, glm::uvec3 aResolution, void* aTextureData);
		image(unsigned int aArrayLayers, format aFormat, unsigned int aResolution, void* aTextureData);
		image(unsigned int aArrayLayers, format aFormat, glm::uvec2 aResolution, void* aTextureData);
		image(unsigned int aArrayLayers, format aFormat, glm::uvec3 aResolution, void* aTextureData);

		// -------------------- Device Context Memory -------------------- //

		// Load image and create device cont
		image(context* aContext, create_info aCreateInfo, const util::string& aFilePath);
		image(context* aContext, create_info aCreateInfo, const char* aFilePath);

		// Make an image a device resource, usint a host image.
		image(context* aContext, create_info aCreateInfo, const image* aHostImage);

		// No Data Upload.
		image(context* aContext, create_info aCreateInfo, type::id aTypeID, unsigned int aResolution);
		image(context* aContext, create_info aCreateInfo, type::id aTypeID, glm::uvec2 aResolution);
		image(context* aContext, create_info aCreateInfo, type::id aTypeID, glm::uvec3 aResolution);
		image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, type::id aTypeID, unsigned int aResolution);
		image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, type::id aTypeID, glm::uvec2 aResolution);
		image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, type::id aTypeID, glm::uvec3 aResolution);

		// Data Upload
		image(context* aContext, create_info aCreateInfo, type::id aTypeID, unsigned int aResolution, void* aTextureData);
		image(context* aContext, create_info aCreateInfo, type::id aTypeID, glm::uvec2 aResolution, void* aTextureData);
		image(context* aContext, create_info aCreateInfo, type::id aTypeID, glm::uvec3 aResolution, void* aTextureData);
		image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, type::id aTypeID, unsigned int aResolution, void* aTextureData);
		image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, type::id aTypeID, glm::uvec2 aResolution, void* aTextureData);
		image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, type::id aTypeID, glm::uvec3 aResolution, void* aTextureData);

		// No Data Upload.
		image(context* aContext, create_info aCreateInfo, format aFormat, unsigned int aResolution);
		image(context* aContext, create_info aCreateInfo, format aFormat, glm::uvec2 aResolution);
		image(context* aContext, create_info aCreateInfo, format aFormat, glm::uvec3 aResolution);
		image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, format aFormat, unsigned int aResolution);
		image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, format aFormat, glm::uvec2 aResolution);
		image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, format aFormat, glm::uvec3 aResolution);

		// Data Upload
		image(context* aContext, create_info aCreateInfo, format aFormat, unsigned int aResolution, void* aTextureData);
		image(context* aContext, create_info aCreateInfo, format aFormat, glm::uvec2 aResolution, void* aTextureData);
		image(context* aContext, create_info aCreateInfo, format aFormat, glm::uvec3 aResolution, void* aTextureData);
		image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, format aFormat, unsigned int aResolution, void* aTextureData);
		image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, format aFormat, glm::uvec2 aResolution, void* aTextureData);
		image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, format aFormat, glm::uvec3 aResolution, void* aTextureData);

		// Copy Constructor.
		image(image& aInput);
		// Move Constructor.
		image(image&& aInput) noexcept;
		// Destructor
		~image();

		// Copy Assignment.
		image& operator=(image& aRhs);
		// Move Assignment.
		image& operator=(image&& aRhs) noexcept;
		// TRANSFER
		vk_command_buffer operator<<(image& aRhs);
		// TRANSFER
		vk_command_buffer operator>>(image& aRhs);
		// TRANSFER
		vk_command_buffer operator<<(buffer& aRhs);
		// TRANSFER
		vk_command_buffer operator>>(buffer& aRhs);
		// GRAPHICS
		vk_command_buffer generate_mipmaps(vk_filter aFilter);

		// Transistions all mip levels and array layers into the desired image layout.
		// Queue Support: T, G, C, D, E.
		void cmd_transition(vk_command_buffer aCommandBuffer,
			vk_access_flags aSrcAccessMask, vk_access_flags aDstAccessMask,
			vk_pipeline_stage_flags aSrcStage, vk_pipeline_stage_flags aDstStage,
			vk_image_layout aNewLayout
		);

		// Transistions selected mip levels and array layers into the desired image layout.
		// Queue Support: T, G, C, D, E.
		void cmd_transition(vk_command_buffer aCommandBuffer,
			vk_access_flags aSrcAccessMask, vk_access_flags aDstAccessMask,
			vk_pipeline_stage_flags aSrcStage, vk_pipeline_stage_flags aDstStage,
			uint32_t aMipLevel, uint32_t aMipLevelCount,
			uint32_t aArrayLayer, uint32_t aArrayLayerCount,
			vk_image_layout aNewLayout
		);

		vk_attachment_description description(vk_attachment_load_op aLoadOp, vk_attachment_store_op aStoreOp);
		vk_attachment_description description(vk_attachment_load_op aLoadOp, vk_attachment_store_op aStoreOp, vk_image_layout aFinalLayout);

		// Insure that all MipLevels and ArrayLayers have the same image layout before using a description.
		// Insure that all mip levels, and array layers, have the same layout before using a description.
		vk_attachment_description description(
			vk_attachment_load_op aLoadOp, vk_attachment_store_op aStoreOp,
			vk_attachment_load_op aStencilLoadOp, vk_attachment_store_op aStencilStoreOp,
			vk_image_layout aInitialLayout, vk_image_layout aFinalLayout
		);

		// Generates image views from texture instance. (YOU ARE RESPONSIBLE FOR DESTROYING VIEWS)
		vk_image_view view();
		//vk_image_view view(vk_image_viewType aType, VkImageSubresourceRange aRange);
		//vk_image_view view(vk_image_viewType aType, VkComponentMapping aComponentMapping, VkImageSubresourceRange aRange);
		
		// Write to an image.
		vk_result write(size_t aMemorySize, void* aData);

		// Read to an image.
		vk_result read(size_t aMemorySize, void* aData);

		// Total memory size of the image. (Does not include mip levels)
		size_t get_memory_size() const;

		vk_image handle();

		void clear();

	private:

		static bool initialize();
		static void terminate();

		// Host Memory
		//size_t						Size;
		//void*						Data;
		unsigned int				Layers;
		int							Format;
		glm::uvec3					Resolution;

		// Device Memory
		context*					Context;
		size_t						DeviceSize;
		vk_image_create_info		CreateInfo;
		vk_image					Handle;
		vk_memory_allocate_info		AllocateInfo;
		vk_device_memory			MemoryHandle;
		vk_image_layout**			Layout; 
		vk_extent_3d*				MipExtent;

		bool load_host_image(const char* aFilePath);

		bool create_host_image(uint32_t aArrayLayers, int aFormat, uint32_t aWidth, uint32_t aHeight, uint32_t aDepth, void* aTextureData);

		void clear_host_memory();

		// Calculates number of possible mip levels.
		uint32_t mip_level_count(vk_image_type aImageType, vk_extent_3d aExtent);

		// Creates Device Image.
		vk_result create_device_image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, int aFormat, glm::uvec3 aResolution);
		vk_result create_device_image(context* aDeviceContext, vk_image_create_info aCreateInfo, int aMemoryType);
		vk_result get_limits(vk_image_format_properties * aReturn, device* aDevice, create_info aCreateInfo, int aFormat, glm::uvec3 aResolution);
		vk_image_create_info pack_create_info(vk_image_format_properties aImageLimits, create_info aCreateInfo, unsigned int aArrayLayers, int aFormat, glm::uvec3 aResolution);
		vk_memory_allocate_info pack_allocate_info(context* aContext, device* aDevice, int aMemoryType);
		vk_result generate_miplevels(unsigned int aMipLevel, unsigned int aArrayLayers, vk_image_layout aInitialLayout, vk_image_type aImageType, vk_extent_3d aExtent);

		// Destroys device memory image.
		void clear_device_memory();

		// Clears garbage memory.
		void zero_out();

		// Used by system_window
		vk_image_view create_system_window_frame(context* aContext, vk_format aFormat, vk_image_usage_flags aUsageFlags, vk_image aImageHandle, glm::uvec3 aImageResolution);

	};

}

#endif // !GEODESUKA_CORE_GCL_IMAGE_H
