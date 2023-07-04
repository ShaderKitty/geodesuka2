#include <geodesuka/core/gcl/image.h>

// Standard C Libs
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdint.h>
#include <float.h>

#include <assert.h>

#include <vector>

#include <geodesuka/core/math.h>

#include <geodesuka/core/gcl/variable.h>

// Image Loading
#define FREEIMAGE_LIB
#include <FreeImage.h>

// So gross.
// Group these later based on spec.
// https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#texel-block-size

/*
Size table;

8
16
24
32
48
64
96
128
192
256
*/

namespace geodesuka::core::gcl {

	using namespace util;

	static FREE_IMAGE_FORMAT extid_to_fifid(io::file::extension ExtensionID) {
		switch (ExtensionID) {
		default:								return FIF_UNKNOWN;
		case io::file::extension::IMAGE_BMP:	return FIF_BMP;
		case io::file::extension::IMAGE_ICO:	return FIF_ICO;
		case io::file::extension::IMAGE_JPEG:	return FIF_JPEG;
		case io::file::extension::IMAGE_JNG:	return FIF_JNG;
		case io::file::extension::IMAGE_KOALA:	return FIF_KOALA;
		case io::file::extension::IMAGE_LBM:	return FIF_LBM;
		case io::file::extension::IMAGE_IFF:	return FIF_IFF;
		case io::file::extension::IMAGE_MNG:	return FIF_MNG;
		case io::file::extension::IMAGE_PBM:	return FIF_PBM;
		case io::file::extension::IMAGE_PBMRAW: return FIF_PBMRAW;
		case io::file::extension::IMAGE_PCD:	return FIF_PCD;
		case io::file::extension::IMAGE_PCX:	return FIF_PCX;
		case io::file::extension::IMAGE_PGM:	return FIF_PGM;
		case io::file::extension::IMAGE_PGMRAW: return FIF_PGMRAW;
		case io::file::extension::IMAGE_PNG:	return FIF_PNG;
		case io::file::extension::IMAGE_PPM:	return FIF_PPM;
		case io::file::extension::IMAGE_PPMRAW: return FIF_PPMRAW;
		case io::file::extension::IMAGE_RAS:	return FIF_RAS;
		case io::file::extension::IMAGE_TARGA:	return FIF_TARGA;
		case io::file::extension::IMAGE_TIFF:	return FIF_TIFF;
		case io::file::extension::IMAGE_WBMP:	return FIF_WBMP;
		case io::file::extension::IMAGE_PSD:	return FIF_PSD;
		case io::file::extension::IMAGE_CUT:	return FIF_CUT;
		case io::file::extension::IMAGE_XBM:	return FIF_XBM;
		case io::file::extension::IMAGE_XPM:	return FIF_XPM;
		case io::file::extension::IMAGE_DDS:	return FIF_DDS;
		case io::file::extension::IMAGE_GIF:	return FIF_GIF;
		case io::file::extension::IMAGE_HDR:	return FIF_HDR;
		case io::file::extension::IMAGE_FAXG3:	return FIF_FAXG3;
		case io::file::extension::IMAGE_SGI:	return FIF_SGI;
		case io::file::extension::IMAGE_EXR:	return FIF_EXR;
		case io::file::extension::IMAGE_J2K:	return FIF_J2K;
		case io::file::extension::IMAGE_JP2:	return FIF_JP2;
		case io::file::extension::IMAGE_PFM:	return FIF_PFM;
		case io::file::extension::IMAGE_PICT:	return FIF_PICT;
		case io::file::extension::IMAGE_RAW:	return FIF_RAW;
		case io::file::extension::IMAGE_WEBP:	return FIF_WEBP;
		case io::file::extension::IMAGE_JXR:	return FIF_JXR;
		}
	}

	image::create_info::create_info() {
		this->Sample = image::sample::COUNT_1;
		this->Tiling = image::tiling::OPTIMAL;
		this->Memory = 0;
		this->Usage = image::usage::TRANSFER_DST | image::usage::TRANSFER_SRC;
	}

	image::create_info::create_info(int aSample, int aTiling, int aMemory, int aUsage) {
		this->Sample = aSample;
		this->Tiling = aTiling;
		this->Memory = aMemory;
		this->Usage = aUsage;
	}

	image::format image::t2f(type::id aID) {
		switch (aID) {
		default:					return image::format::UNDEFINED;
			// Scalar Types
		case type::id::UCHAR:		return image::format::R8_UINT;
		case type::id::USHORT:		return image::format::R16_UINT;
		case type::id::UINT:		return image::format::R32_UINT;
		case type::id::CHAR:		return image::format::R8_SINT;
		case type::id::SHORT:		return image::format::R16_SINT;
		case type::id::INT:			return image::format::R32_SINT;
		case type::id::FLOAT:		return image::format::R32_SFLOAT;
		case type::id::DOUBLE:		return image::format::R64_SFLOAT;
			// Vector Types.
		case type::id::UCHAR2:		return image::format::R8G8_UINT;
		case type::id::UCHAR3:		return image::format::R8G8B8_UINT;
		case type::id::UCHAR4:		return image::format::R8G8B8A8_UINT;
		case type::id::USHORT2:		return image::format::R16G16_UINT;
		case type::id::USHORT3:		return image::format::R16G16B16_UINT;
		case type::id::USHORT4:		return image::format::R16G16B16A16_UINT;
		case type::id::UINT2:		return image::format::R32G32_UINT;
		case type::id::UINT3:		return image::format::R32G32B32_UINT;
		case type::id::UINT4:		return image::format::R32G32B32A32_UINT;
		case type::id::CHAR2:		return image::format::R8G8_SINT;
		case type::id::CHAR3:		return image::format::R8G8B8_SINT;
		case type::id::CHAR4:		return image::format::R8G8B8A8_SINT;
		case type::id::SHORT2:		return image::format::R16G16_SINT;
		case type::id::SHORT3:		return image::format::R16G16B16_SINT;
		case type::id::SHORT4:		return image::format::R16G16B16A16_SINT;
		case type::id::INT2:		return image::format::R32G32_SINT;
		case type::id::INT3:		return image::format::R32G32B32_SINT;
		case type::id::INT4:		return image::format::R32G32B32A32_SINT;
		case type::id::FLOAT2:		return image::format::R32G32_SFLOAT;
		case type::id::FLOAT3:		return image::format::R32G32B32_SFLOAT;
		case type::id::FLOAT4:		return image::format::R32G32B32A32_SFLOAT;
			//case type::id::DOUBLE2:		return image::format::R64G64_SFLOAT;
			//case type::id::DOUBLE3:		return image::format::R64G64B64_SFLOAT;
			//case type::id::DOUBLE4:		return image::format::R64G64B64A64_SFLOAT;
			// Matrix Types (Must take multiple slots.)
			//case type::id::FLOAT2X2:	return image::format::UNDEFINED;
			//case type::id::FLOAT2X3:	return image::format::UNDEFINED;
			//case type::id::FLOAT2X4:	return image::format::UNDEFINED;
			//case type::id::FLOAT3X2:	return image::format::UNDEFINED;
			//case type::id::FLOAT3X3:	return image::format::UNDEFINED;
			//case type::id::FLOAT3X4:	return image::format::UNDEFINED;
			//case type::id::FLOAT4X2:	return image::format::UNDEFINED;
			//case type::id::FLOAT4X3:	return image::format::UNDEFINED;
			//case type::id::FLOAT4X4:	return image::format::UNDEFINED;
		}
	}

	size_t image::bytesperpixel(int aFormat) {
		return (image::bitsperpixel(aFormat) / 8);
	}

	size_t image::bitsperpixel(int aFormat) {

		switch (aFormat) {
		default: return 0;
		case VK_FORMAT_R4G4_UNORM_PACK8: return 8;
		case VK_FORMAT_R4G4B4A4_UNORM_PACK16: return 16;
		case VK_FORMAT_B4G4R4A4_UNORM_PACK16: return 16;
		case VK_FORMAT_R5G6B5_UNORM_PACK16: return 16;
		case VK_FORMAT_B5G6R5_UNORM_PACK16: return 16;
		case VK_FORMAT_R5G5B5A1_UNORM_PACK16: return 16;
		case VK_FORMAT_B5G5R5A1_UNORM_PACK16: return 16;
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16: return 16;
		case VK_FORMAT_R8_UNORM: return 8;
		case VK_FORMAT_R8_SNORM: return 8;
		case VK_FORMAT_R8_USCALED: return 8;
		case VK_FORMAT_R8_SSCALED: return 8;
		case VK_FORMAT_R8_UINT: return 8;
		case VK_FORMAT_R8_SINT: return 8;
		case VK_FORMAT_R8_SRGB: return 8;
		case VK_FORMAT_R8G8_UNORM: return 16;
		case VK_FORMAT_R8G8_SNORM: return 16;
		case VK_FORMAT_R8G8_USCALED: return 16;
		case VK_FORMAT_R8G8_SSCALED: return 16;
		case VK_FORMAT_R8G8_UINT: return 16;
		case VK_FORMAT_R8G8_SINT: return 16;
		case VK_FORMAT_R8G8_SRGB: return 16;
		case VK_FORMAT_R8G8B8_UNORM: return 24;
		case VK_FORMAT_R8G8B8_SNORM: return 24;
		case VK_FORMAT_R8G8B8_USCALED: return 24;
		case VK_FORMAT_R8G8B8_SSCALED: return 24;
		case VK_FORMAT_R8G8B8_UINT: return 24;
		case VK_FORMAT_R8G8B8_SINT: return 24;
		case VK_FORMAT_R8G8B8_SRGB: return 24;
		case VK_FORMAT_B8G8R8_UNORM: return 24;
		case VK_FORMAT_B8G8R8_SNORM: return 24;
		case VK_FORMAT_B8G8R8_USCALED: return 24;
		case VK_FORMAT_B8G8R8_SSCALED: return 24;
		case VK_FORMAT_B8G8R8_UINT: return 24;
		case VK_FORMAT_B8G8R8_SINT: return 24;
		case VK_FORMAT_B8G8R8_SRGB: return 24;
		case VK_FORMAT_R8G8B8A8_UNORM: return 32;
		case VK_FORMAT_R8G8B8A8_SNORM: return 32;
		case VK_FORMAT_R8G8B8A8_USCALED: return 32;
		case VK_FORMAT_R8G8B8A8_SSCALED: return 32;
		case VK_FORMAT_R8G8B8A8_UINT: return 32;
		case VK_FORMAT_R8G8B8A8_SINT: return 32;
		case VK_FORMAT_R8G8B8A8_SRGB: return 32;
		case VK_FORMAT_B8G8R8A8_UNORM: return 32;
		case VK_FORMAT_B8G8R8A8_SNORM: return 32;
		case VK_FORMAT_B8G8R8A8_USCALED: return 32;
		case VK_FORMAT_B8G8R8A8_SSCALED: return 32;
		case VK_FORMAT_B8G8R8A8_UINT: return 32;
		case VK_FORMAT_B8G8R8A8_SINT: return 32;
		case VK_FORMAT_B8G8R8A8_SRGB: return 32;
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32: return 32;
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32: return 32;
		case VK_FORMAT_A8B8G8R8_USCALED_PACK32: return 32;
		case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: return 32;
		case VK_FORMAT_A8B8G8R8_UINT_PACK32: return 32;
		case VK_FORMAT_A8B8G8R8_SINT_PACK32: return 32;
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32: return 32;
		case VK_FORMAT_A2R10G10B10_UNORM_PACK32: return 32;
		case VK_FORMAT_A2R10G10B10_SNORM_PACK32: return 32;
		case VK_FORMAT_A2R10G10B10_USCALED_PACK32: return 32;
		case VK_FORMAT_A2R10G10B10_SSCALED_PACK32: return 32;
		case VK_FORMAT_A2R10G10B10_UINT_PACK32: return 32;
		case VK_FORMAT_A2R10G10B10_SINT_PACK32: return 32;
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32: return 32;
		case VK_FORMAT_A2B10G10R10_SNORM_PACK32: return 32;
		case VK_FORMAT_A2B10G10R10_USCALED_PACK32: return 32;
		case VK_FORMAT_A2B10G10R10_SSCALED_PACK32: return 32;
		case VK_FORMAT_A2B10G10R10_UINT_PACK32: return 32;
		case VK_FORMAT_A2B10G10R10_SINT_PACK32: return 32;
		case VK_FORMAT_R16_UNORM: return 16;
		case VK_FORMAT_R16_SNORM: return 16;
		case VK_FORMAT_R16_USCALED: return 16;
		case VK_FORMAT_R16_SSCALED: return 16;
		case VK_FORMAT_R16_UINT: return 16;
		case VK_FORMAT_R16_SINT: return 16;
		case VK_FORMAT_R16_SFLOAT: return 16;
		case VK_FORMAT_R16G16_UNORM: return 16;
		case VK_FORMAT_R16G16_SNORM: return 16;
		case VK_FORMAT_R16G16_USCALED: return 16;
		case VK_FORMAT_R16G16_SSCALED: return 16;
		case VK_FORMAT_R16G16_UINT: return 16;
		case VK_FORMAT_R16G16_SINT: return 16;
		case VK_FORMAT_R16G16_SFLOAT: return 16;
		case VK_FORMAT_R16G16B16_UNORM: return 48;
		case VK_FORMAT_R16G16B16_SNORM: return 48;
		case VK_FORMAT_R16G16B16_USCALED: return 48;
		case VK_FORMAT_R16G16B16_SSCALED: return 48;
		case VK_FORMAT_R16G16B16_UINT: return 48;
		case VK_FORMAT_R16G16B16_SINT: return 48;
		case VK_FORMAT_R16G16B16_SFLOAT: return 48;
		case VK_FORMAT_R16G16B16A16_UNORM: return 64;
		case VK_FORMAT_R16G16B16A16_SNORM: return 64;
		case VK_FORMAT_R16G16B16A16_USCALED: return 64;
		case VK_FORMAT_R16G16B16A16_SSCALED: return 64;
		case VK_FORMAT_R16G16B16A16_UINT: return 64;
		case VK_FORMAT_R16G16B16A16_SINT: return 64;
		case VK_FORMAT_R16G16B16A16_SFLOAT: return 64;
		case VK_FORMAT_R32_UINT: return 32;
		case VK_FORMAT_R32_SINT: return 32;
		case VK_FORMAT_R32_SFLOAT: return 32;
		case VK_FORMAT_R32G32_UINT: return 64;
		case VK_FORMAT_R32G32_SINT: return 64;
		case VK_FORMAT_R32G32_SFLOAT: return 64;
		case VK_FORMAT_R32G32B32_UINT: return 96;
		case VK_FORMAT_R32G32B32_SINT: return 96;
		case VK_FORMAT_R32G32B32_SFLOAT: return 96;
		case VK_FORMAT_R32G32B32A32_UINT: return 128;
		case VK_FORMAT_R32G32B32A32_SINT: return 128;
		case VK_FORMAT_R32G32B32A32_SFLOAT: return 128;
		case VK_FORMAT_R64_UINT: return 64;
		case VK_FORMAT_R64_SINT: return 64;
		case VK_FORMAT_R64_SFLOAT: return 64;
		case VK_FORMAT_R64G64_UINT: return 128;
		case VK_FORMAT_R64G64_SINT: return 128;
		case VK_FORMAT_R64G64_SFLOAT: return 128;
		case VK_FORMAT_R64G64B64_UINT: return 192;
		case VK_FORMAT_R64G64B64_SINT: return 192;
		case VK_FORMAT_R64G64B64_SFLOAT: return 192;
		case VK_FORMAT_R64G64B64A64_UINT: return 256;
		case VK_FORMAT_R64G64B64A64_SINT: return 256;
		case VK_FORMAT_R64G64B64A64_SFLOAT: return 256;
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32: return 32;
		case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32: return 32;
		case VK_FORMAT_D16_UNORM: return 16;
		case VK_FORMAT_X8_D24_UNORM_PACK32: return 32;
		case VK_FORMAT_D32_SFLOAT: return 32;
		case VK_FORMAT_S8_UINT: return 8;
		case VK_FORMAT_D16_UNORM_S8_UINT: return 24;
		case VK_FORMAT_D24_UNORM_S8_UINT: return 32;
		case VK_FORMAT_D32_SFLOAT_S8_UINT: return 40;
			// I might comment this section out.
			/*
		case VK_FORMAT_BC1_RGB_UNORM_BLOCK                              : return 0;
		case VK_FORMAT_BC1_RGB_SRGB_BLOCK                               : return 0;
		case VK_FORMAT_BC1_RGBA_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_BC1_RGBA_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_BC2_UNORM_BLOCK                                  : return 0;
		case VK_FORMAT_BC2_SRGB_BLOCK                                   : return 0;
		case VK_FORMAT_BC3_UNORM_BLOCK                                  : return 0;
		case VK_FORMAT_BC3_SRGB_BLOCK                                   : return 0;
		case VK_FORMAT_BC4_UNORM_BLOCK                                  : return 0;
		case VK_FORMAT_BC4_SNORM_BLOCK                                  : return 0;
		case VK_FORMAT_BC5_UNORM_BLOCK                                  : return 0;
		case VK_FORMAT_BC5_SNORM_BLOCK                                  : return 0;
		case VK_FORMAT_BC6H_UFLOAT_BLOCK                                : return 0;
		case VK_FORMAT_BC6H_SFLOAT_BLOCK                                : return 0;
		case VK_FORMAT_BC7_UNORM_BLOCK                                  : return 0;
		case VK_FORMAT_BC7_SRGB_BLOCK                                   : return 0;
		case VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK                          : return 0;
		case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK                           : return 0;
		case VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK                        : return 0;
		case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK                         : return 0;
		case VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK                        : return 0;
		case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK                         : return 0;
		case VK_FORMAT_EAC_R11_UNORM_BLOCK                              : return 0;
		case VK_FORMAT_EAC_R11_SNORM_BLOCK                              : return 0;
		case VK_FORMAT_EAC_R11G11_UNORM_BLOCK                           : return 0;
		case VK_FORMAT_EAC_R11G11_SNORM_BLOCK                           : return 0;
		case VK_FORMAT_ASTC_4x4_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_4x4_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_5x4_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_5x4_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_5x5_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_5x5_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_6x5_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_6x5_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_6x6_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_6x6_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_8x5_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_8x5_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_8x6_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_8x6_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_8x8_UNORM_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_8x8_SRGB_BLOCK                              : return 0;
		case VK_FORMAT_ASTC_10x5_UNORM_BLOCK                            : return 0;
		case VK_FORMAT_ASTC_10x5_SRGB_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_10x6_UNORM_BLOCK                            : return 0;
		case VK_FORMAT_ASTC_10x6_SRGB_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_10x8_UNORM_BLOCK                            : return 0;
		case VK_FORMAT_ASTC_10x8_SRGB_BLOCK                             : return 0;
		case VK_FORMAT_ASTC_10x10_UNORM_BLOCK                           : return 0;
		case VK_FORMAT_ASTC_10x10_SRGB_BLOCK                            : return 0;
		case VK_FORMAT_ASTC_12x10_UNORM_BLOCK                           : return 0;
		case VK_FORMAT_ASTC_12x10_SRGB_BLOCK                            : return 0;
		case VK_FORMAT_ASTC_12x12_UNORM_BLOCK                           : return 0;
		case VK_FORMAT_ASTC_12x12_SRGB_BLOCK                            : return 0;
		case VK_FORMAT_G8B8G8R8_422_UNORM                               : return 0;
		case VK_FORMAT_B8G8R8G8_422_UNORM                               : return 0;
		case VK_FORMAT_G8_B8_R8_3PLANE_420_UNORM                        : return 0;
		case VK_FORMAT_G8_B8R8_2PLANE_420_UNORM                         : return 0;
		case VK_FORMAT_G8_B8_R8_3PLANE_422_UNORM                        : return 0;
		case VK_FORMAT_G8_B8R8_2PLANE_422_UNORM                         : return 0;
		case VK_FORMAT_G8_B8_R8_3PLANE_444_UNORM                        : return 0;
		case VK_FORMAT_R10X6_UNORM_PACK16                               : return 0;
		case VK_FORMAT_R10X6G10X6_UNORM_2PACK16                         : return 0;
		case VK_FORMAT_R10X6G10X6B10X6A10X6_UNORM_4PACK16               : return 0;
		case VK_FORMAT_G10X6B10X6G10X6R10X6_422_UNORM_4PACK16           : return 0;
		case VK_FORMAT_B10X6G10X6R10X6G10X6_422_UNORM_4PACK16           : return 0;
		case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_420_UNORM_3PACK16       : return 0;
		case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_420_UNORM_3PACK16        : return 0;
		case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_422_UNORM_3PACK16       : return 0;
		case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_422_UNORM_3PACK16        : return 0;
		case VK_FORMAT_G10X6_B10X6_R10X6_3PLANE_444_UNORM_3PACK16       : return 0;
		case VK_FORMAT_R12X4_UNORM_PACK16                               : return 0;
		case VK_FORMAT_R12X4G12X4_UNORM_2PACK16                         : return 0;
		case VK_FORMAT_R12X4G12X4B12X4A12X4_UNORM_4PACK16               : return 0;
		case VK_FORMAT_G12X4B12X4G12X4R12X4_422_UNORM_4PACK16           : return 0;
		case VK_FORMAT_B12X4G12X4R12X4G12X4_422_UNORM_4PACK16           : return 0;
		case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_420_UNORM_3PACK16       : return 0;
		case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_420_UNORM_3PACK16        : return 0;
		case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_422_UNORM_3PACK16       : return 0;
		case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_422_UNORM_3PACK16        : return 0;
		case VK_FORMAT_G12X4_B12X4_R12X4_3PLANE_444_UNORM_3PACK16       : return 0;
		case VK_FORMAT_G16B16G16R16_422_UNORM                           : return 0;
		case VK_FORMAT_B16G16R16G16_422_UNORM                           : return 0;
		case VK_FORMAT_G16_B16_R16_3PLANE_420_UNORM                     : return 0;
		case VK_FORMAT_G16_B16R16_2PLANE_420_UNORM                      : return 0;
		case VK_FORMAT_G16_B16_R16_3PLANE_422_UNORM                     : return 0;
		case VK_FORMAT_G16_B16R16_2PLANE_422_UNORM                      : return 0;
		case VK_FORMAT_G16_B16_R16_3PLANE_444_UNORM                     : return 0;
		case VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG                      : return 0;
		case VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG                      : return 0;
		case VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG                      : return 0;
		case VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG                      : return 0;
		case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG                       : return 0;
		case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG                       : return 0;
		case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG                       : return 0;
		case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG                       : return 0;
		case VK_FORMAT_ASTC_4x4_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_5x4_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_5x5_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_6x5_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_6x6_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_8x5_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_8x6_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_8x8_SFLOAT_BLOCK_EXT                        : return 0;
		case VK_FORMAT_ASTC_10x5_SFLOAT_BLOCK_EXT                       : return 0;
		case VK_FORMAT_ASTC_10x6_SFLOAT_BLOCK_EXT                       : return 0;
		case VK_FORMAT_ASTC_10x8_SFLOAT_BLOCK_EXT                       : return 0;
		case VK_FORMAT_ASTC_10x10_SFLOAT_BLOCK_EXT                      : return 0;
		case VK_FORMAT_ASTC_12x10_SFLOAT_BLOCK_EXT                      : return 0;
		case VK_FORMAT_ASTC_12x12_SFLOAT_BLOCK_EXT                      : return 0;
		case VK_FORMAT_G8_B8R8_2PLANE_444_UNORM_EXT                     : return 0;
		case VK_FORMAT_G10X6_B10X6R10X6_2PLANE_444_UNORM_3PACK16_EXT    : return 0;
		case VK_FORMAT_G12X4_B12X4R12X4_2PLANE_444_UNORM_3PACK16_EXT    : return 0;
		case VK_FORMAT_G16_B16R16_2PLANE_444_UNORM_EXT                  : return 0;
		case VK_FORMAT_A4R4G4B4_UNORM_PACK16_EXT                        : return 0;
		case VK_FORMAT_A4B4G4R4_UNORM_PACK16_EXT                        : return 0;
			*/
		}
	}

	vk_image_aspect_flags image::aspect_flag(int aFormat) {
		vk_image_aspect_flags AspectFlag = 0;
		switch (aFormat) {
		default: 
			AspectFlag = 0;
			break;
		case VK_FORMAT_R4G4_UNORM_PACK8:
		case VK_FORMAT_R4G4B4A4_UNORM_PACK16: 
		case VK_FORMAT_B4G4R4A4_UNORM_PACK16: 
		case VK_FORMAT_R5G6B5_UNORM_PACK16: 
		case VK_FORMAT_B5G6R5_UNORM_PACK16: 
		case VK_FORMAT_R5G5B5A1_UNORM_PACK16: 
		case VK_FORMAT_B5G5R5A1_UNORM_PACK16: 
		case VK_FORMAT_A1R5G5B5_UNORM_PACK16: 
		case VK_FORMAT_R8_UNORM:
		case VK_FORMAT_R8_SNORM:
		case VK_FORMAT_R8_USCALED:
		case VK_FORMAT_R8_SSCALED:
		case VK_FORMAT_R8_UINT:
		case VK_FORMAT_R8_SINT:
		case VK_FORMAT_R8_SRGB:
		case VK_FORMAT_R8G8_UNORM: 
		case VK_FORMAT_R8G8_SNORM: 
		case VK_FORMAT_R8G8_USCALED: 
		case VK_FORMAT_R8G8_SSCALED: 
		case VK_FORMAT_R8G8_UINT: 
		case VK_FORMAT_R8G8_SINT: 
		case VK_FORMAT_R8G8_SRGB: 
		case VK_FORMAT_R8G8B8_UNORM: 
		case VK_FORMAT_R8G8B8_SNORM: 
		case VK_FORMAT_R8G8B8_USCALED: 
		case VK_FORMAT_R8G8B8_SSCALED: 
		case VK_FORMAT_R8G8B8_UINT: 
		case VK_FORMAT_R8G8B8_SINT: 
		case VK_FORMAT_R8G8B8_SRGB: 
		case VK_FORMAT_B8G8R8_UNORM: 
		case VK_FORMAT_B8G8R8_SNORM: 
		case VK_FORMAT_B8G8R8_USCALED: 
		case VK_FORMAT_B8G8R8_SSCALED: 
		case VK_FORMAT_B8G8R8_UINT: 
		case VK_FORMAT_B8G8R8_SINT: 
		case VK_FORMAT_B8G8R8_SRGB: 
		case VK_FORMAT_R8G8B8A8_UNORM: 
		case VK_FORMAT_R8G8B8A8_SNORM: 
		case VK_FORMAT_R8G8B8A8_USCALED: 
		case VK_FORMAT_R8G8B8A8_SSCALED: 
		case VK_FORMAT_R8G8B8A8_UINT: 
		case VK_FORMAT_R8G8B8A8_SINT: 
		case VK_FORMAT_R8G8B8A8_SRGB: 
		case VK_FORMAT_B8G8R8A8_UNORM: 
		case VK_FORMAT_B8G8R8A8_SNORM: 
		case VK_FORMAT_B8G8R8A8_USCALED: 
		case VK_FORMAT_B8G8R8A8_SSCALED: 
		case VK_FORMAT_B8G8R8A8_UINT: 
		case VK_FORMAT_B8G8R8A8_SINT: 
		case VK_FORMAT_B8G8R8A8_SRGB: 
		case VK_FORMAT_A8B8G8R8_UNORM_PACK32: 
		case VK_FORMAT_A8B8G8R8_SNORM_PACK32: 
		case VK_FORMAT_A8B8G8R8_USCALED_PACK32: 
		case VK_FORMAT_A8B8G8R8_SSCALED_PACK32: 
		case VK_FORMAT_A8B8G8R8_UINT_PACK32: 
		case VK_FORMAT_A8B8G8R8_SINT_PACK32: 
		case VK_FORMAT_A8B8G8R8_SRGB_PACK32: 
		case VK_FORMAT_A2R10G10B10_UNORM_PACK32: 
		case VK_FORMAT_A2R10G10B10_SNORM_PACK32: 
		case VK_FORMAT_A2R10G10B10_USCALED_PACK32: 
		case VK_FORMAT_A2R10G10B10_SSCALED_PACK32: 
		case VK_FORMAT_A2R10G10B10_UINT_PACK32: 
		case VK_FORMAT_A2R10G10B10_SINT_PACK32: 
		case VK_FORMAT_A2B10G10R10_UNORM_PACK32: 
		case VK_FORMAT_A2B10G10R10_SNORM_PACK32: 
		case VK_FORMAT_A2B10G10R10_USCALED_PACK32: 
		case VK_FORMAT_A2B10G10R10_SSCALED_PACK32: 
		case VK_FORMAT_A2B10G10R10_UINT_PACK32: 
		case VK_FORMAT_A2B10G10R10_SINT_PACK32: 
		case VK_FORMAT_R16_UNORM: 
		case VK_FORMAT_R16_SNORM: 
		case VK_FORMAT_R16_USCALED: 
		case VK_FORMAT_R16_SSCALED: 
		case VK_FORMAT_R16_UINT: 
		case VK_FORMAT_R16_SINT: 
		case VK_FORMAT_R16_SFLOAT: 
		case VK_FORMAT_R16G16_UNORM: 
		case VK_FORMAT_R16G16_SNORM: 
		case VK_FORMAT_R16G16_USCALED: 
		case VK_FORMAT_R16G16_SSCALED: 
		case VK_FORMAT_R16G16_UINT: 
		case VK_FORMAT_R16G16_SINT: 
		case VK_FORMAT_R16G16_SFLOAT: 
		case VK_FORMAT_R16G16B16_UNORM: 
		case VK_FORMAT_R16G16B16_SNORM: 
		case VK_FORMAT_R16G16B16_USCALED: 
		case VK_FORMAT_R16G16B16_SSCALED: 
		case VK_FORMAT_R16G16B16_UINT: 
		case VK_FORMAT_R16G16B16_SINT: 
		case VK_FORMAT_R16G16B16_SFLOAT: 
		case VK_FORMAT_R16G16B16A16_UNORM: 
		case VK_FORMAT_R16G16B16A16_SNORM: 
		case VK_FORMAT_R16G16B16A16_USCALED: 
		case VK_FORMAT_R16G16B16A16_SSCALED: 
		case VK_FORMAT_R16G16B16A16_UINT: 
		case VK_FORMAT_R16G16B16A16_SINT: 
		case VK_FORMAT_R16G16B16A16_SFLOAT: 
		case VK_FORMAT_R32_UINT: 
		case VK_FORMAT_R32_SINT: 
		case VK_FORMAT_R32_SFLOAT: 
		case VK_FORMAT_R32G32_UINT: 
		case VK_FORMAT_R32G32_SINT: 
		case VK_FORMAT_R32G32_SFLOAT: 
		case VK_FORMAT_R32G32B32_UINT: 
		case VK_FORMAT_R32G32B32_SINT: 
		case VK_FORMAT_R32G32B32_SFLOAT: 
		case VK_FORMAT_R32G32B32A32_UINT: 
		case VK_FORMAT_R32G32B32A32_SINT: 
		case VK_FORMAT_R32G32B32A32_SFLOAT: 
		case VK_FORMAT_R64_UINT: 
		case VK_FORMAT_R64_SINT: 
		case VK_FORMAT_R64_SFLOAT: 
		case VK_FORMAT_R64G64_UINT: 
		case VK_FORMAT_R64G64_SINT: 
		case VK_FORMAT_R64G64_SFLOAT: 
		case VK_FORMAT_R64G64B64_UINT: 
		case VK_FORMAT_R64G64B64_SINT: 
		case VK_FORMAT_R64G64B64_SFLOAT: 
		case VK_FORMAT_R64G64B64A64_UINT: 
		case VK_FORMAT_R64G64B64A64_SINT: 
		case VK_FORMAT_R64G64B64A64_SFLOAT: 
		case VK_FORMAT_B10G11R11_UFLOAT_PACK32: 
		case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:
			AspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
			break;
		case VK_FORMAT_D16_UNORM: 
		case VK_FORMAT_X8_D24_UNORM_PACK32: 
		case VK_FORMAT_D32_SFLOAT: 
			AspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT;
			break;
		case VK_FORMAT_D16_UNORM_S8_UINT: 
		case VK_FORMAT_D24_UNORM_S8_UINT: 
		case VK_FORMAT_D32_SFLOAT_S8_UINT: 
		case VK_FORMAT_S8_UINT:
			AspectFlag = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
			break;
		}
		return AspectFlag;
	}

	image::image() {
		this->zero_out();
	}

	image::image(const util::string& aFilePath) : io::file(aFilePath) {
		this->load_host_image(aFilePath.ptr());
		this->Context = nullptr;
		this->Handle = VK_NULL_HANDLE;
		this->MemoryHandle = VK_NULL_HANDLE;
		this->Layout = NULL;
		this->MipExtent = NULL;
	}

	image::image(const char* aFilePath) : io::file(aFilePath) {
		this->load_host_image(aFilePath);
		this->Context = nullptr;
		this->Handle = VK_NULL_HANDLE;
		this->MemoryHandle = VK_NULL_HANDLE;
		this->Layout = NULL;
		this->MipExtent = NULL;
	}

	image::image(type::id aTypeID, unsigned int aResolution) {
		this->zero_out();
		assert(!this->create_host_image(1u, t2f(aTypeID), aResolution, 1u, 1u, NULL));
	}

	image::image(type::id aTypeID, glm::uvec2 aResolution) {
		this->zero_out();
		assert(!this->create_host_image(1u, t2f(aTypeID), aResolution.x, aResolution.y, 1u, NULL));
	}

	image::image(type::id aTypeID, glm::uvec3 aResolution) {
		this->zero_out();
		assert(!this->create_host_image(1u, t2f(aTypeID), aResolution.x, aResolution.y, aResolution.z, NULL));
	}

	image::image(unsigned int aArrayLayers, type::id aTypeID, unsigned int aResolution) {
		this->zero_out();
		assert(!this->create_host_image(aArrayLayers, t2f(aTypeID), aResolution, 1u, 1u, NULL));
	}

	image::image(unsigned int aArrayLayers, type::id aTypeID, glm::uvec2 aResolution) {
		this->zero_out();
		assert(!this->create_host_image(aArrayLayers, t2f(aTypeID), aResolution.x, aResolution.y, 1u, NULL));
	}

	image::image(unsigned int aArrayLayers, type::id aTypeID, glm::uvec3 aResolution) {
		this->zero_out();
		assert(!this->create_host_image(aArrayLayers, t2f(aTypeID), aResolution.x, aResolution.y, aResolution.z, NULL));
	}

	image::image(type::id aTypeID, unsigned int aResolution, void* aTextureData) {
		this->zero_out();
		assert(!this->create_host_image(1u, t2f(aTypeID), aResolution, 1u, 1u, aTextureData));
	}

	image::image(type::id aTypeID, glm::uvec2 aResolution, void* aTextureData) {
		this->zero_out();
		assert(!this->create_host_image(1u, t2f(aTypeID), aResolution.x, aResolution.y, 1u, aTextureData));
	}

	image::image(type::id aTypeID, glm::uvec3 aResolution, void* aTextureData) {
		this->zero_out();
		assert(!this->create_host_image(1u, t2f(aTypeID), aResolution.x, aResolution.y, aResolution.z, aTextureData));
	}

	image::image(unsigned int aArrayLayers, type::id aTypeID, unsigned int aResolution, void* aTextureData) {
		this->zero_out();
		assert(!this->create_host_image(aArrayLayers, t2f(aTypeID), aResolution, 1u, 1u, aTextureData));
	}

	image::image(unsigned int aArrayLayers, type::id aTypeID, glm::uvec2 aResolution, void* aTextureData) {
		this->zero_out();
		assert(!this->create_host_image(aArrayLayers, t2f(aTypeID), aResolution.x, aResolution.y, 1u, aTextureData));
	}

	image::image(unsigned int aArrayLayers, type::id aTypeID, glm::uvec3 aResolution, void* aTextureData) {
		this->zero_out();
		assert(!this->create_host_image(aArrayLayers, t2f(aTypeID), aResolution.x, aResolution.y, aResolution.z, aTextureData));
	}

	image::image(format aFormat, unsigned int aResolution) {
		this->zero_out();
		assert(!this->create_host_image(1u, aFormat, aResolution, 1u, 1u, NULL));
	}

	image::image(format aFormat, glm::uvec2 aResolution) {
		this->zero_out();
		assert(!this->create_host_image(1u, aFormat, aResolution.x, aResolution.y, 1u, NULL));
	}

	image::image(format aFormat, glm::uvec3 aResolution) {
		this->zero_out();
		assert(!this->create_host_image(1u, aFormat, aResolution.x, aResolution.y, aResolution.z, NULL));
	}

	image::image(unsigned int aArrayLayers, format aFormat, unsigned int aResolution) {
		this->zero_out();
		assert(!this->create_host_image(aArrayLayers, aFormat, aResolution, 1u, 1u, NULL));
	}

	image::image(unsigned int aArrayLayers, format aFormat, glm::uvec2 aResolution) {
		this->zero_out();
		assert(!this->create_host_image(aArrayLayers, aFormat, aResolution.x, aResolution.y, 1u, NULL));
	}

	image::image(unsigned int aArrayLayers, format aFormat, glm::uvec3 aResolution) {
		this->zero_out();
		assert(!this->create_host_image(aArrayLayers, aFormat, aResolution.x, aResolution.y, aResolution.z, NULL));
	}

	image::image(format aFormat, unsigned int aResolution, void* aTextureData) {
		this->zero_out();
		assert(!this->create_host_image(1u, aFormat, aResolution, 1u, 1u, aTextureData));
	}

	image::image(format aFormat, glm::uvec2 aResolution, void* aTextureData) {
		this->zero_out();
		assert(!this->create_host_image(1u, aFormat, aResolution.x, aResolution.y, 1u, aTextureData));
	}

	image::image(format aFormat, glm::uvec3 aResolution, void* aTextureData) {
		this->zero_out();
		assert(!this->create_host_image(1u, aFormat, aResolution.x, aResolution.y, aResolution.z, aTextureData));
	}

	image::image(unsigned int aArrayLayers, format aFormat, unsigned int aResolution, void* aTextureData) {
		this->zero_out();
		assert(!this->create_host_image(aArrayLayers, aFormat, aResolution, 1u, 1u, aTextureData));
	}

	image::image(unsigned int aArrayLayers, format aFormat, glm::uvec2 aResolution, void* aTextureData) {
		this->zero_out();
		assert(!this->create_host_image(aArrayLayers, aFormat, aResolution.x, aResolution.y, 1u, aTextureData));
	}

	image::image(unsigned int aArrayLayers, format aFormat, glm::uvec3 aResolution, void* aTextureData) {
		this->zero_out();
		assert(!this->create_host_image(aArrayLayers, aFormat, aResolution.x, aResolution.y, aResolution.z, aTextureData));
	}

	image::image(context* aContext, create_info aCreateInfo, const util::string& aFilePath) : io::file(aFilePath) {
		vk_result Result = VK_SUCCESS;

		this->load_host_image(aFilePath.ptr());

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, this->Format, glm::uvec3(this->Resolution.x, this->Resolution.y, 1u));
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (this->HostData != NULL) {
			Result = this->write(this->HostSize, this->HostData);
		}
	}

	image::image(context* aContext, create_info aCreateInfo, const char* aFilePath) : io::file(aFilePath) {
		vk_result Result = VK_SUCCESS;

		this->load_host_image(aFilePath);

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, this->Format, glm::uvec3(this->Resolution.x, this->Resolution.y, 1u));
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (this->HostData != NULL) {
			Result = this->write(this->HostSize, this->HostData);
		}
	}

	image::image(context* aContext, create_info aCreateInfo, const image* aHostImage) {
		vk_result Result = VK_SUCCESS;
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, aHostImage->Format, aHostImage->Resolution);
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (this->HostData != NULL) {
			Result = this->write(this->HostSize, this->HostData);
		}
	}

	// ----- New Interface.

	image::image(context* aContext, create_info aCreateInfo, type::id aTypeID, unsigned int aResolution) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, t2f(aTypeID), glm::uvec3(aResolution, 1u, 1u));
		assert(Result == VK_SUCCESS);
	}

	image::image(context* aContext, create_info aCreateInfo, type::id aTypeID, glm::uvec2 aResolution) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, t2f(aTypeID), glm::uvec3(aResolution.x, aResolution.y, 1u));
		assert(Result == VK_SUCCESS);
	}

	image::image(context* aContext, create_info aCreateInfo, type::id aTypeID, glm::uvec3 aResolution) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, t2f(aTypeID), aResolution);
		assert(Result == VK_SUCCESS);
	}

	image::image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, type::id aTypeID, unsigned int aResolution) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, aArrayLayers, t2f(aTypeID), glm::uvec3(aResolution, 1u, 1u));
		assert(Result == VK_SUCCESS);
	}

	image::image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, type::id aTypeID, glm::uvec2 aResolution) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, aArrayLayers, t2f(aTypeID), glm::uvec3(aResolution.x, aResolution.y, 1u));
		assert(Result == VK_SUCCESS);
	}

	image::image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, type::id aTypeID, glm::uvec3 aResolution) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, aArrayLayers, t2f(aTypeID), aResolution);
		assert(Result == VK_SUCCESS);
	}

	image::image(context* aContext, create_info aCreateInfo, type::id aTypeID, unsigned int aResolution, void* aTextureData) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, t2f(aTypeID), glm::uvec3(aResolution, 1u, 1u));
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (aTextureData != NULL) {
			Result = this->write(this->DeviceSize, aTextureData);
		}
	}

	image::image(context* aContext, create_info aCreateInfo, type::id aTypeID, glm::uvec2 aResolution, void* aTextureData) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, t2f(aTypeID), glm::uvec3(aResolution.x, aResolution.y, 1u));
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (aTextureData != NULL) {
			Result = this->write(this->DeviceSize, aTextureData);
		}
	}

	image::image(context* aContext, create_info aCreateInfo, type::id aTypeID, glm::uvec3 aResolution, void* aTextureData) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, t2f(aTypeID), aResolution);
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (aTextureData != NULL) {
			Result = this->write(this->DeviceSize, aTextureData);
		}
	}

	image::image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, type::id aTypeID, unsigned int aResolution, void* aTextureData) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, aArrayLayers, t2f(aTypeID), glm::uvec3(aResolution, 1u, 1u));
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (aTextureData != NULL) {
			Result = this->write(this->DeviceSize, aTextureData);
		}
	}

	image::image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, type::id aTypeID, glm::uvec2 aResolution, void* aTextureData) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, aArrayLayers, t2f(aTypeID), glm::uvec3(aResolution.x, aResolution.y, 1u));
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (aTextureData != NULL) {
			Result = this->write(this->DeviceSize, aTextureData);
		}
	}

	image::image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, type::id aTypeID, glm::uvec3 aResolution, void* aTextureData) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, aArrayLayers, t2f(aTypeID), aResolution);
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (aTextureData != NULL) {
			Result = this->write(this->DeviceSize, aTextureData);
		}
	}

	image::image(context* aContext, create_info aCreateInfo, format aFormat, unsigned int aResolution) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, aFormat, glm::uvec3(aResolution, 1u, 1u));
		assert(Result == VK_SUCCESS);
	}

	image::image(context* aContext, create_info aCreateInfo, format aFormat, glm::uvec2 aResolution) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, aFormat, glm::uvec3(aResolution.x, aResolution.y, 1u));
		assert(Result == VK_SUCCESS);
	}

	image::image(context* aContext, create_info aCreateInfo, format aFormat, glm::uvec3 aResolution) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, aFormat, aResolution);
		assert(Result == VK_SUCCESS);
	}

	image::image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, format aFormat, unsigned int aResolution) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, aArrayLayers, aFormat, glm::uvec3(aResolution, 1u, 1u));
		assert(Result == VK_SUCCESS);
	}

	image::image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, format aFormat, glm::uvec2 aResolution) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, aArrayLayers, aFormat, glm::uvec3(aResolution.x, aResolution.y, 1u));
		assert(Result == VK_SUCCESS);
	}

	image::image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, format aFormat, glm::uvec3 aResolution) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, aArrayLayers, aFormat, aResolution);
		assert(Result == VK_SUCCESS);
	}

	image::image(context* aContext, create_info aCreateInfo, format aFormat, unsigned int aResolution, void* aTextureData) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, aFormat, glm::uvec3(aResolution, 1u, 1u));
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (aTextureData != NULL) {
			Result = this->write(this->DeviceSize, aTextureData);
		}
	}

	image::image(context* aContext, create_info aCreateInfo, format aFormat, glm::uvec2 aResolution, void* aTextureData) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, aFormat, glm::uvec3(aResolution.x, aResolution.y, 1u));
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (aTextureData != NULL) {
			Result = this->write(this->DeviceSize, aTextureData);
		}
	}

	image::image(context* aContext, create_info aCreateInfo, format aFormat, glm::uvec3 aResolution, void* aTextureData) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, 1u, aFormat, aResolution);
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (aTextureData != NULL) {
			Result = this->write(this->DeviceSize, aTextureData);
		}
	}

	image::image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, format aFormat, unsigned int aResolution, void* aTextureData) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, aArrayLayers, aFormat, glm::uvec3(aResolution, 1u, 1u));
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (aTextureData != NULL) {
			Result = this->write(this->DeviceSize, aTextureData);
		}
	}

	image::image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, format aFormat, glm::uvec2 aResolution, void* aTextureData) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, aArrayLayers, aFormat, glm::uvec3(aResolution.x, aResolution.y, 1u));
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (aTextureData != NULL) {
			Result = this->write(this->DeviceSize, aTextureData);
		}
	}

	image::image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, format aFormat, glm::uvec3 aResolution, void* aTextureData) {
		vk_result Result = VK_SUCCESS;

		// Zero out memory.
		this->zero_out();

		// Allocate Device Memory
		Result = this->create_device_image(aContext, aCreateInfo, aArrayLayers, aFormat, aResolution);
		assert(Result == VK_SUCCESS);

		// Write memory to device memory.
		if (aTextureData != NULL) {
			Result = this->write(this->DeviceSize, aTextureData);
		}
	}

	image::image(image& aInput) {
		vk_result Result = VK_SUCCESS;
		vk_fence Fence = VK_NULL_HANDLE;
		vk_command_buffer CommandBuffer = VK_NULL_HANDLE;

		this->zero_out();

		this->Path				= aInput.Path;
		this->Directory			= aInput.Directory;
		this->Name				= aInput.Name;
		this->Extension			= aInput.Extension;
		this->ID				= aInput.ID;

		// Host Memory Data Transfer
		this->HostSize			= aInput.HostSize;
		if (this->HostSize > 0) {
			this->HostData = malloc(this->HostSize);
			assert(this->HostData);
			memcpy(this->HostData, aInput.HostData, this->HostSize);
		}
		else {
			this->HostData = NULL;
		}

		this->Layers			= aInput.Layers;
		this->Format			= aInput.Format;
		this->Resolution		= aInput.Resolution;
		//this->BitsPerPixel		= aInput.BitsPerPixel;
		//this->BytesPerPixel		= aInput.BytesPerPixel;
		this->Context			= aInput.Context;

		// If context exists, then Device Object exists, copy over.
		if (this->Context != nullptr) {
			Result = this->create_device_image(aInput.Context, aInput.CreateInfo, this->Context->parent()->get_memory_type(aInput.AllocateInfo.memoryTypeIndex));
			assert(Result == VK_SUCCESS);

			Fence = this->Context->create_fence();
			CommandBuffer = (*this << aInput);

			this->Context->execute(device::TRANSFER, CommandBuffer, Fence);
			Result = vkWaitForFences(this->Context->handle(), 1, &Fence, VK_TRUE, UINT64_MAX);

			this->Context->destroy_command_buffer(device::TRANSFER, CommandBuffer);
			this->Context->destroy_fence(Fence);
		}
	}

	image::image(image&& aInput) noexcept {
		// File Info
		this->Path				= aInput.Path;
		this->Directory			= aInput.Directory;
		this->Name				= aInput.Name;
		this->Extension			= aInput.Extension;
		this->ID				= aInput.ID;

		// Host Memory 
		this->HostSize			= aInput.HostSize;
		this->HostData			= aInput.HostData;
		this->Layers			= aInput.Layers;
		this->Format			= aInput.Format;
		this->Resolution		= aInput.Resolution;
		//this->BitsPerPixel		= aInput.BitsPerPixel;
		//this->BytesPerPixel		= aInput.BytesPerPixel;

		// Device Memory Stuff
		this->Context			= aInput.Context;
		this->DeviceSize		= aInput.DeviceSize;
		this->CreateInfo		= aInput.CreateInfo;
		this->Handle			= aInput.Handle;
		this->AllocateInfo		= aInput.AllocateInfo;
		this->MemoryHandle		= aInput.MemoryHandle;
		this->Layout			= aInput.Layout;
		this->MipExtent			= aInput.MipExtent;
		aInput.zero_out();
	}

	image::~image() {
		this->clear_device_memory();
		this->clear_host_memory();
		this->zero_out();
	}

	image& image::operator=(image& aRhs) {
		if (this == &aRhs) return *this;
		this->clear_device_memory();
		this->clear_host_memory();

		this->Path				= aRhs.Path;
		this->Directory			= aRhs.Directory;
		this->Name				= aRhs.Name;
		this->Extension			= aRhs.Extension;
		this->ID				= aRhs.ID;

		this->HostSize = aRhs.HostSize;
		if ((aRhs.HostSize > 0) && (aRhs.HostData != NULL)) {
			this->HostData = malloc(this->HostSize);
			assert(this->HostData);
			memcpy(this->HostData, aRhs.HostData, this->HostSize);
		}
		else {
			this->HostData = NULL;
		}

		this->Layers			= aRhs.Layers;
		this->Format			= aRhs.Format;
		this->Resolution		= aRhs.Resolution;
		//this->BitsPerPixel		= aRhs.BitsPerPixel;
		//this->BytesPerPixel		= aRhs.BytesPerPixel;

		this->Context = aRhs.Context;
		if (this->Context != nullptr) {
			vk_result Result = VK_SUCCESS;

			Result = this->create_device_image(aRhs.Context, aRhs.CreateInfo, aRhs.Context->parent()->get_memory_type(aRhs.AllocateInfo.memoryTypeIndex));

			vk_fence Fence = this->Context->create_fence();
			vk_command_buffer CommandBuffer = (*this << aRhs);

			this->Context->execute(device::TRANSFER, CommandBuffer, Fence);
			Result = vkWaitForFences(this->Context->handle(), 1, &Fence, VK_TRUE, UINT64_MAX);

			this->Context->destroy_command_buffer(device::TRANSFER, CommandBuffer);
			this->Context->destroy_fence(Fence);
		}
		else {
			this->CreateInfo		= {};
			this->Handle			= VK_NULL_HANDLE;
			this->AllocateInfo		= {};
			this->MemoryHandle		= VK_NULL_HANDLE;
			this->Layout			= NULL;
			this->MipExtent			= NULL;
		}

		return *this;
	}

	image& image::operator=(image&& aRhs) noexcept {
		this->clear_device_memory();
		this->clear_host_memory();
		// File Info
		this->Path				= aRhs.Path;
		this->Directory			= aRhs.Directory;
		this->Name				= aRhs.Name;
		this->Extension			= aRhs.Extension;
		this->ID				= aRhs.ID;

		// Host Memory 
		this->HostSize				= aRhs.HostSize;
		this->HostData				= aRhs.HostData;

		this->Layers			= aRhs.Layers;
		this->Format			= aRhs.Format;
		this->Resolution		= aRhs.Resolution;
		//this->BitsPerPixel		= aRhs.BitsPerPixel;
		//this->BytesPerPixel		= aRhs.BytesPerPixel;

		// Device Memory Stuff
		this->Context			= aRhs.Context;
		this->DeviceSize		= aRhs.DeviceSize;
		this->CreateInfo		= aRhs.CreateInfo;
		this->Handle			= aRhs.Handle;
		this->AllocateInfo		= aRhs.AllocateInfo;
		this->MemoryHandle		= aRhs.MemoryHandle;
		this->Layout			= aRhs.Layout;
		this->MipExtent			= aRhs.MipExtent;
		aRhs.zero_out();
		return *this;
	}

	vk_command_buffer image::operator<<(image& aRhs) {
		vk_command_buffer CommandBuffer = VK_NULL_HANDLE;
		// Cannot do transfer operations if they do not share the
		// same context.
		if (this->Context != aRhs.Context) return CommandBuffer;
		// Check if dimensions are equal.
		if ((this->CreateInfo.extent.width != aRhs.CreateInfo.extent.width) ||
			(this->CreateInfo.extent.height != aRhs.CreateInfo.extent.height) ||
			(this->CreateInfo.extent.depth != aRhs.CreateInfo.extent.depth) ||
			(this->CreateInfo.arrayLayers != aRhs.CreateInfo.arrayLayers) ||
			(this->CreateInfo.mipLevels != aRhs.CreateInfo.mipLevels) ||
			(this->CreateInfo.format != aRhs.CreateInfo.format)
			) return CommandBuffer;

		// Check if proper usage flags are enabled.
		if (
			((this->CreateInfo.usage & image::usage::TRANSFER_DST) != image::usage::TRANSFER_DST)
			||
			((aRhs.CreateInfo.usage & image::usage::TRANSFER_SRC) != image::usage::TRANSFER_SRC)
			) return CommandBuffer;

		vk_result Result = VK_SUCCESS;
		uint32_t RegionCount = this->CreateInfo.mipLevels;
		vk_image_copy* RegionList = NULL;

		RegionList = (vk_image_copy*)malloc(RegionCount * sizeof(vk_image_copy));
		assert(RegionList);

		// Copy every region
		for (uint32_t i = 0; i < this->CreateInfo.mipLevels; i++) {
			RegionList[i].srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			RegionList[i].srcSubresource.mipLevel = i;
			RegionList[i].srcSubresource.baseArrayLayer = 0;
			RegionList[i].srcSubresource.layerCount = aRhs.CreateInfo.arrayLayers;
			RegionList[i].srcOffset = { 0, 0, 0 };
			RegionList[i].dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			RegionList[i].dstSubresource.mipLevel = i;
			RegionList[i].dstSubresource.baseArrayLayer = 0;
			RegionList[i].dstSubresource.layerCount = this->CreateInfo.arrayLayers;
			RegionList[i].dstOffset = { 0, 0, 0 };
			RegionList[i].extent = this->MipExtent[i];
		}

		vk_command_buffer_begin_info BeginInfo{};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext = NULL;
		BeginInfo.flags = 0;
		BeginInfo.pInheritanceInfo = NULL;

		CommandBuffer = this->Context->create_command_buffer(device::TRANSFER, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		Result = vkBeginCommandBuffer(CommandBuffer, &BeginInfo);
		// Transition all mip levels and layers into TRANSFER_SRC
		aRhs.cmd_transition(CommandBuffer,
			VK_ACCESS_MEMORY_WRITE_BIT,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
		);
		// Transition all mip levels and layers into TRANSFER_DST
		this->cmd_transition(CommandBuffer,
			VK_ACCESS_NONE,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		);
		vkCmdCopyImage(CommandBuffer,
			aRhs.Handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			this->Handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			RegionCount, RegionList
		);
		Result = vkEndCommandBuffer(CommandBuffer);

		free(RegionList);

		return CommandBuffer;
	}

	vk_command_buffer image::operator>>(image& aRhs) {
		return (aRhs << *this);
	}

	vk_command_buffer image::operator<<(buffer& aRhs) {
		vk_command_buffer CommandBuffer = VK_NULL_HANDLE;
		// Both operands must share the same parent context and memory size.
		if ((this->Context != aRhs.Context) || (this->get_memory_size() != (size_t)aRhs.CreateInfo.size)) return CommandBuffer;
		// Texture must have enabled, TRANSFER_DST. buffer must have TRANSFER_SRC enabled in usage.
		if (
			((this->CreateInfo.usage & image::usage::TRANSFER_DST) != image::usage::TRANSFER_DST)
			||
			((aRhs.CreateInfo.usage & buffer::usage::TRANSFER_SRC) != buffer::usage::TRANSFER_SRC)
			) return CommandBuffer;

		vk_result Result = vk_result::VK_SUCCESS;

		vk_command_buffer_begin_info BeginInfo{};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext = NULL;
		BeginInfo.flags = 0;
		BeginInfo.pInheritanceInfo = NULL;

		vk_buffer_image_copy CopyRegion{};
		CopyRegion.bufferOffset = 0;
		CopyRegion.bufferRowLength = 0;
		CopyRegion.bufferImageHeight = 0;
		CopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		CopyRegion.imageSubresource.mipLevel = 0;
		CopyRegion.imageSubresource.baseArrayLayer = 0;
		CopyRegion.imageSubresource.layerCount = this->CreateInfo.arrayLayers;
		CopyRegion.imageOffset = { 0, 0, 0 };
		CopyRegion.imageExtent = this->CreateInfo.extent;

		CommandBuffer = this->Context->create_command_buffer(device::TRANSFER, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		Result = vkBeginCommandBuffer(CommandBuffer, &BeginInfo);
		// Prepare image as destination.
		this->cmd_transition(CommandBuffer,
			VK_ACCESS_NONE,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, 1,
			0, this->CreateInfo.arrayLayers,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
		);
		vkCmdCopyBufferToImage(CommandBuffer,
			aRhs.Handle,
			this->Handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			1, &CopyRegion
		);
		Result = vkEndCommandBuffer(CommandBuffer);

		return CommandBuffer;
	}

	vk_command_buffer image::operator>>(buffer& aRhs) {
		return (aRhs << *this);
	}

	vk_command_buffer image::generate_mipmaps(vk_filter aFilter) {
		vk_command_buffer CommandBuffer = VK_NULL_HANDLE;
		if (this->Context == nullptr) return CommandBuffer;

		vk_result Result = vk_result::VK_SUCCESS;

		vk_command_buffer_begin_info BeginInfo{};
		BeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		BeginInfo.pNext = NULL;
		BeginInfo.flags = 0;
		BeginInfo.pInheritanceInfo = NULL;

		CommandBuffer = this->Context->create_command_buffer(device::GRAPHICS, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

		Result = vkBeginCommandBuffer(CommandBuffer, &BeginInfo);
		for (uint32_t i = 0; i < this->CreateInfo.mipLevels - 1; i++) {
			// Blit regions.
			vk_image_blit Region{};
			Region.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			Region.srcSubresource.mipLevel = i;
			Region.srcSubresource.baseArrayLayer = 0;
			Region.srcSubresource.layerCount = this->CreateInfo.arrayLayers;
			Region.srcOffsets[0] = { 0, 0, 0 };
			Region.srcOffsets[1] = { (int32_t)this->MipExtent[i].width, (int32_t)this->MipExtent[i].height, (int32_t)this->MipExtent[i].depth };
			Region.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			Region.dstSubresource.mipLevel = i + 1;
			Region.dstSubresource.baseArrayLayer = 0;
			Region.dstSubresource.layerCount = this->CreateInfo.arrayLayers;
			Region.dstOffsets[0] = { 0, 0, 0 };
			Region.dstOffsets[1] = { (int32_t)this->MipExtent[i + 1].width, (int32_t)this->MipExtent[i + 1].height, (int32_t)this->MipExtent[i + 1].depth };

			// i will be referenced in blitting process.
			this->cmd_transition(
				CommandBuffer,
				VK_ACCESS_MEMORY_WRITE_BIT,
				VK_ACCESS_TRANSFER_READ_BIT,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				i, 1,
				0, this->CreateInfo.arrayLayers,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL
			);

			// i + 1 will be written to by image blitting.
			this->cmd_transition(
				CommandBuffer,
				VK_ACCESS_NONE,
				VK_ACCESS_MEMORY_WRITE_BIT,
				VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				i + 1, 1,
				0, this->CreateInfo.arrayLayers,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
			);

			vkCmdBlitImage(
				CommandBuffer,
				this->Handle, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				this->Handle, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1, &Region,
				aFilter
			);

		}
		this->cmd_transition(
			CommandBuffer,
			VK_ACCESS_MEMORY_WRITE_BIT,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			0, this->CreateInfo.mipLevels,
			0, this->CreateInfo.arrayLayers,
			VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
		);
		Result = vkEndCommandBuffer(CommandBuffer);

		return CommandBuffer;
	}

	void image::cmd_transition(vk_command_buffer aCommandBuffer,
		vk_access_flags aSrcAccessMask, vk_access_flags aDstAccessMask,
		vk_pipeline_stage_flags aSrcStage, vk_pipeline_stage_flags aDstStage,
		vk_image_layout aNewLayout
	) {
		this->cmd_transition(aCommandBuffer, aSrcAccessMask, aDstAccessMask, aSrcStage, aDstStage, 0, this->CreateInfo.mipLevels, 0, this->CreateInfo.arrayLayers, aNewLayout);
	}

	void image::cmd_transition(vk_command_buffer aCommandBuffer,
		vk_access_flags aSrcAccessMask, vk_access_flags aDstAccessMask,
		vk_pipeline_stage_flags aSrcStage, vk_pipeline_stage_flags aDstStage,
		uint32_t aMipLevel, uint32_t aMipLevelCount,
		uint32_t aArrayLayer, uint32_t aArrayLayerCount,
		vk_image_layout aNewLayout
	) {
		uint32_t TransitionCount = aMipLevelCount * aArrayLayerCount;
		vk_image_memory_barrier* Transition = (vk_image_memory_barrier*)malloc(TransitionCount * sizeof(vk_image_memory_barrier));
		assert(Transition);

		for (uint32_t i = aMipLevel; i < (aMipLevel + aMipLevelCount); i++) {
			for (uint32_t j = aArrayLayer; j < (aArrayLayer + aArrayLayerCount); j++) {
				uint32_t k = (i - aMipLevel) * aArrayLayerCount + (j - aArrayLayer);
				Transition[k].sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				Transition[k].pNext = NULL;
				Transition[k].srcAccessMask = aSrcAccessMask;
				Transition[k].dstAccessMask = aDstAccessMask;
				Transition[k].oldLayout = this->Layout[i][j];
				Transition[k].newLayout = aNewLayout;
				Transition[k].srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				Transition[k].dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				Transition[k].image = this->Handle;
				Transition[k].subresourceRange.aspectMask = aspect_flag(this->CreateInfo.format);
				Transition[k].subresourceRange.baseMipLevel = i;
				Transition[k].subresourceRange.levelCount = 1;
				Transition[k].subresourceRange.baseArrayLayer = j;
				Transition[k].subresourceRange.layerCount = 1;
				this->Layout[i][j] = aNewLayout;
			}
		}

		vkCmdPipelineBarrier(aCommandBuffer,
			aSrcStage, aDstStage,
			0,
			0, NULL,
			0, NULL,
			TransitionCount, Transition
		);

		free(Transition);
	}

	vk_image_view image::view() {
		// Change later after screwing with.
		vk_result Result = VK_SUCCESS;
		vk_image_view ImageView = VK_NULL_HANDLE;
		vk_image_view_create_info ImageViewCreateInfo{};
		ImageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewCreateInfo.pNext = NULL;
		ImageViewCreateInfo.flags = 0;
		ImageViewCreateInfo.image = this->Handle;
		switch (this->CreateInfo.imageType) {
		default:
			break;
		case VK_IMAGE_TYPE_1D:
			ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_1D;
			break;
		case VK_IMAGE_TYPE_2D:
			ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			break;
		case VK_IMAGE_TYPE_3D:
			ImageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_3D;
			break;
		}
		ImageViewCreateInfo.format = this->CreateInfo.format;
		ImageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.subresourceRange.aspectMask = aspect_flag(this->CreateInfo.format);
		ImageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		ImageViewCreateInfo.subresourceRange.levelCount = this->CreateInfo.mipLevels;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		ImageViewCreateInfo.subresourceRange.layerCount = this->CreateInfo.arrayLayers;
		Result = vkCreateImageView(this->Context->handle(), &ImageViewCreateInfo, NULL, &ImageView);
		return ImageView;
	}

	vk_attachment_description image::description(vk_attachment_load_op aLoadOp, vk_attachment_store_op aStoreOp) {
		return this->description(aLoadOp, aStoreOp, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, this->Layout[0][0], this->Layout[0][0]);
	}

	vk_attachment_description image::description(vk_attachment_load_op aLoadOp, vk_attachment_store_op aStoreOp, vk_image_layout aFinalLayout){
		return this->description(aLoadOp, aStoreOp, VK_ATTACHMENT_LOAD_OP_DONT_CARE, VK_ATTACHMENT_STORE_OP_DONT_CARE, this->Layout[0][0], aFinalLayout);
	}

	vk_attachment_description image::description(
		vk_attachment_load_op aLoadOp, vk_attachment_store_op aStoreOp,
		vk_attachment_load_op aStencilLoadOp, vk_attachment_store_op aStencilStoreOp,
		vk_image_layout aInitialLayout, vk_image_layout aFinalLayout
	) {
		vk_attachment_description AttachmentDescription;
		AttachmentDescription.flags = 0;
		AttachmentDescription.format = this->CreateInfo.format;
		AttachmentDescription.samples = this->CreateInfo.samples;
		AttachmentDescription.loadOp = aLoadOp;
		AttachmentDescription.storeOp = aStoreOp;
		AttachmentDescription.stencilLoadOp = aStencilLoadOp;
		AttachmentDescription.stencilStoreOp = aStencilStoreOp;
		AttachmentDescription.initialLayout = aInitialLayout;
		AttachmentDescription.finalLayout = aFinalLayout;
		return AttachmentDescription;
	}

	vk_result image::write(size_t aMemorySize, void* aData) {
		vk_result Result = VK_SUCCESS;
		command_list CommandList[2] = { command_list(1), command_list(1) };
		vk_fence Fence[2] = { VK_NULL_HANDLE, VK_NULL_HANDLE };
		vk_semaphore Semaphore = VK_NULL_HANDLE;

		// Create staging buffer and prepare for transfer.
		buffer StagingBuffer(
			Context,
			device::HOST_VISIBLE | device::HOST_COHERENT,
			buffer::TRANSFER_SRC | buffer::TRANSFER_DST,
			aMemorySize,
			aData
		);

		// Synchronization primitives.
		Result = this->Context->create_fence(2, Fence);
		Semaphore = this->Context->create_semaphore();

		// Command Lists for execution.
		// This is a TRANSFER operation.
		CommandList[0][0] = (*this << StagingBuffer);
		// This is a GRAPHICS operation.
		CommandList[1][0] = this->generate_mipmaps(VK_FILTER_NEAREST);
		// Execution dependency.
		CommandList[1].depends_on(Semaphore, CommandList[0], VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT);

		// Execute operations.
		Result = this->Context->execute(device::qfeo::TRANSFER, CommandList[0], Fence[0]);
		Result = this->Context->execute(device::qfeo::GRAPHICS, CommandList[1], Fence[1]);
		Result = vkWaitForFences(this->Context->handle(), 2, Fence, VK_TRUE, UINT64_MAX);

		// Free up resources.
		this->Context->destroy_command_list(device::qfeo::TRANSFER, CommandList[0]);
		this->Context->destroy_command_list(device::qfeo::GRAPHICS, CommandList[1]);
		this->Context->destroy_fence(2, Fence);
		this->Context->destroy_semaphore(Semaphore);

		return Result;
	}

	vk_result image::read(size_t aMemorySize, void* aData) {
		vk_result Result = VK_SUCCESS;
		vk_fence Fence = VK_NULL_HANDLE;
		vk_command_buffer TransferToBuffer = VK_NULL_HANDLE;

		// Create staging buffer and prepare for transfer.
		buffer StagingBuffer(
			Context,
			device::HOST_VISIBLE | device::HOST_COHERENT,
			buffer::TRANSFER_SRC | buffer::TRANSFER_DST,
			aMemorySize,
			NULL
		);

		Fence = this->Context->create_fence();
		TransferToBuffer = *this >> StagingBuffer;
		Result = this->Context->execute(device::TRANSFER, TransferToBuffer, Fence);
		Result = vkWaitForFences(this->Context->handle(), 1, &Fence, VK_TRUE, UINT64_MAX);

		this->Context->destroy_fence(Fence);
		this->Context->destroy_command_buffer(device::TRANSFER, TransferToBuffer);
		return VK_SUCCESS;
	}

	size_t image::get_memory_size() const {
		return this->CreateInfo.arrayLayers * this->CreateInfo.extent.width * this->CreateInfo.extent.height * this->CreateInfo.extent.depth * this->bytesperpixel(this->CreateInfo.format);
	}

	vk_image image::handle() {
		return this->Handle;
	}

	void image::clear() {
		this->clear_device_memory();
		this->clear_host_memory();
		this->zero_out();
	}

	bool image::initialize() {
		FreeImage_Initialise();
		return true;
	}

	void image::terminate() {
		FreeImage_DeInitialise();
	}

	bool image::load_host_image(const char* aFilePath) {

		//FREE_IMAGE_FORMAT FreeImageFormat = extid_to_fifid(this->IMAGE_DDS);
		FREE_IMAGE_FORMAT NameFormat = FreeImage_GetFIFFromFilename(aFilePath);
		FREE_IMAGE_FORMAT FileFormat = FreeImage_GetFIFFromFormat(aFilePath);
		FIBITMAP* BitMap = FreeImage_Load(NameFormat, aFilePath);

		this->Layers = 1u;
		switch (FreeImage_GetImageType(BitMap)) {
		default:
			this->Format = format::UNDEFINED;
			break;
		case FIT_BITMAP:
			switch (FreeImage_GetBPP(BitMap)) {
			default:
				this->Format = format::UNDEFINED;
				break;
				//case 1:
				//	break;
				//case 4:
				//	break;
			case 8:
				this->Format = format::R8_UINT;
				break;
			case 16:
				this->Format = format::R8G8_UINT;
				break;
			case 24:
				this->Format = format::R8G8B8_UINT;
				break;
			case 32:
				this->Format = format::R8G8B8A8_UINT;
				break;
			}
			break;
		case FIT_UINT16:
			this->Format = format::R16_UINT;
			break;
		case FIT_INT16:
			this->Format = format::R16_SINT;
			break;
		case FIT_UINT32:
			this->Format = format::R32_UINT;
			break;
		case FIT_INT32:
			this->Format = format::R32_SINT;
			break;
		case FIT_FLOAT:
			this->Format = format::R32_SFLOAT;
			break;
		case FIT_DOUBLE:
			this->Format = format::R64_SFLOAT;
			break;
		case FIT_COMPLEX:
			this->Format = format::R64G64_SFLOAT;
			break;
		case FIT_RGB16:
			this->Format = format::R16G16B16_UINT;
			break;
		case FIT_RGBA16:
			this->Format = format::R16G16B16A16_UINT;
			break;
		case FIT_RGBF:
			this->Format = format::R32G32B32_SFLOAT;
			break;
		case FIT_RGBAF:
			this->Format = format::R32G32B32A32_SFLOAT;
			break;
		}
		this->Resolution.x = FreeImage_GetWidth(BitMap);
		this->Resolution.y = FreeImage_GetHeight(BitMap);
		this->Resolution.z = 1;
		//this->BitsPerPixel = FreeImage_GetBPP(BitMap);
		//this->BytesPerPixel = this->BitsPerPixel / 8;
		this->HostSize = FreeImage_GetMemorySize(BitMap);
		assert(this->HostSize == (this->Resolution.x * this->Resolution.y * this->Resolution.z * bytesperpixel(this->Format)));

		this->HostData = malloc(this->HostSize);
		assert(this->HostData);

		memcpy(this->HostData, (void*)FreeImage_GetBits(BitMap), this->HostSize);

		FreeImage_Unload(BitMap);

		return false;
	}

	bool image::create_host_image(uint32_t aArrayLayers, int aFormat, uint32_t aWidth, uint32_t aHeight, uint32_t aDepth, void* aTextureData) {
		void *nptr = malloc(aArrayLayers * aWidth * aHeight * aDepth * this->bytesperpixel(aFormat));
		if (nptr == NULL) return true;
		this->HostSize				= aArrayLayers * aWidth * aHeight * aDepth * this->bytesperpixel(aFormat);
		this->HostData				= nptr;

		this->Layers			= aArrayLayers;
		this->Format			= aFormat;
		this->Resolution		= glm::uvec3(aWidth, aHeight, aDepth);
		if (aTextureData != NULL) {
			memcpy(this->HostData, aTextureData, this->HostSize);
		}
		return false;
	}

	void image::clear_host_memory() {
		this->Resolution	= glm::uvec3(0, 0, 0);
		this->Format		= format::UNDEFINED;
		this->Layers		= 0;
		free(this->HostData);
		this->HostData			= NULL;
		this->HostSize			= 0;
	}

	/*
	0 = 640, 480
	1 = 320, 240
	2 = 160, 120
	3 = 80, 60
	4 = 40, 30
	5 = 20, 15

	6 mip levels.
	*/
	uint32_t image::mip_level_count(vk_image_type aImageType, vk_extent_3d aExtent) {
		uint32_t MipLevelCount = 1;
		switch (aImageType) {
		default:
			return 0;
		case VK_IMAGE_TYPE_1D:
			while (true) {
				if (aExtent.width % 2 == 0) {
					aExtent.width /= 2;
					MipLevelCount += 1;
				}
				else {
					break;
				}
			}
			break;
		case VK_IMAGE_TYPE_2D:
			while (true) {
				if ((aExtent.width % 2 == 0) && (aExtent.height % 2 == 0)) {
					aExtent.width /= 2;
					aExtent.height /= 2;
					MipLevelCount += 1;
				}
				else {
					break;
				}
			}
			break;
		case VK_IMAGE_TYPE_3D:
			while (true) {
				if ((aExtent.width % 2 == 0) && (aExtent.height % 2 == 0) && (aExtent.depth % 2 == 0)) {
					aExtent.width /= 2;
					aExtent.height /= 2;
					aExtent.depth /= 2;
					MipLevelCount += 1;
				}
				else {
					break;
				}
			}
			break;
		}
		return MipLevelCount;
	}

	vk_result image::create_device_image(context* aContext, create_info aCreateInfo, unsigned int aArrayLayers, int aFormat, glm::uvec3 aResolution) {
		vk_result Result = VK_SUCCESS;
		vk_image_format_properties FormatProperties{};

		// Return if an improper context is provided.
		if (aContext == nullptr) VK_ERROR_INITIALIZATION_FAILED;
		this->Context = aContext;

		Result = this->get_limits(&FormatProperties, aContext->parent(), aCreateInfo, aFormat, aResolution);

		// Create Image Handle.
		if (Result == VK_SUCCESS) {
			this->DeviceSize = aArrayLayers * bytesperpixel(aFormat) * aResolution.x * aResolution.y * aResolution.z;
			this->CreateInfo = pack_create_info(FormatProperties, aCreateInfo, aArrayLayers, aFormat, aResolution);
			Result = vkCreateImage(this->Context->handle(), &this->CreateInfo, NULL, &this->Handle);
		}
		else {
			// Unsupported Image Format
			return Result;
		}

		// Allocate Device Memory.
		if (Result == VK_SUCCESS) {
			this->AllocateInfo = pack_allocate_info(aContext, aContext->parent(), aCreateInfo.Memory);
			Result = vkAllocateMemory(this->Context->handle(), &this->AllocateInfo, NULL, &this->MemoryHandle);
		}
		else {
			// Failed to Create Image Handle.
			this->clear_device_memory();
			return Result;
		}

		// Bind Handle to Memory.
		if (Result == VK_SUCCESS) {
			Result = vkBindImageMemory(this->Context->handle(), this->Handle, this->MemoryHandle, 0);
		}
		else {
			// Memory Allocation Failure.
			this->clear_device_memory();
			return Result;
		}

		if (Result == VK_SUCCESS) {
			// Generate mip levels.
			Result = generate_miplevels(this->CreateInfo.mipLevels, this->CreateInfo.arrayLayers, this->CreateInfo.initialLayout, this->CreateInfo.imageType, this->CreateInfo.extent);
		}
		else {
			// Memory bind Failed.
			this->clear_device_memory();
			return Result;
		}

		return Result;
	}

	vk_result image::create_device_image(context* aContext, vk_image_create_info aCreateInfo, int aMemoryType) {
		vk_result Result = VK_SUCCESS;
		create_info LocalCreateInfo;
		glm::uvec3 StackResolution = glm::uvec3(0, 0, 0);

		LocalCreateInfo.Sample		= aCreateInfo.samples;
		LocalCreateInfo.Tiling		= aCreateInfo.tiling;
		LocalCreateInfo.Memory		= aMemoryType;
		LocalCreateInfo.Usage		= aCreateInfo.usage;
		StackResolution				= glm::uvec3(aCreateInfo.extent.width, aCreateInfo.extent.height, aCreateInfo.extent.depth);
		Result = this->create_device_image(Context, LocalCreateInfo, aCreateInfo.arrayLayers, aCreateInfo.format, StackResolution);

		return Result;
	}

	vk_result image::get_limits(vk_image_format_properties* aReturn, device* aDevice, create_info aCreateInfo, int aFormat, glm::uvec3 aResolution) {
		vk_image_type ImageType = VK_IMAGE_TYPE_MAX_ENUM;
		if ((aResolution.x > 1u) && (aResolution.y > 1u) && (aResolution.z > 1u)) {
			ImageType = VK_IMAGE_TYPE_3D;
		}
		else if ((aResolution.x > 1u) && (aResolution.y > 1u)) {
			ImageType = VK_IMAGE_TYPE_2D;
		}
		else if (aResolution.x > 1u) {
			ImageType = VK_IMAGE_TYPE_1D;
		}
		else {
			ImageType = VK_IMAGE_TYPE_MAX_ENUM;
		}
		return vkGetPhysicalDeviceImageFormatProperties(aDevice->handle(), (vk_format)aFormat, ImageType, (vk_image_tiling)aCreateInfo.Tiling, (vk_image_usage_flags)aCreateInfo.Usage, 0, aReturn);
	}

	vk_image_create_info image::pack_create_info(vk_image_format_properties aImageLimits, create_info aCreateInfo, unsigned int aArrayLayers, int aFormat, glm::uvec3 aResolution) {
		vk_image_create_info NewCreateInfo;
		NewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		NewCreateInfo.pNext = NULL;
		NewCreateInfo.flags = 0;
		if ((aResolution.x > 1u) && (aResolution.y > 1u) && (aResolution.z > 1u)) {
			NewCreateInfo.imageType = VK_IMAGE_TYPE_3D;
		}
		else if ((aResolution.x > 1u) && (aResolution.y > 1u)) {
			NewCreateInfo.imageType = VK_IMAGE_TYPE_2D;
		}
		else if (aResolution.x > 1u) {
			NewCreateInfo.imageType = VK_IMAGE_TYPE_1D;
		}
		else {
			return {};
		}
		NewCreateInfo.format = (vk_format)aFormat;
		switch (NewCreateInfo.imageType) {
		default:
			return {};
		case VK_IMAGE_TYPE_1D:
			NewCreateInfo.extent = { aResolution.x, 1u, 1u };
			break;
		case VK_IMAGE_TYPE_2D:
			NewCreateInfo.extent = { aResolution.x, aResolution.y, 1u };
			break;
		case VK_IMAGE_TYPE_3D:
			NewCreateInfo.extent = { aResolution.x, aResolution.y, aResolution.z };
			break;
		}
		NewCreateInfo.mipLevels					= this->mip_level_count(NewCreateInfo.imageType, NewCreateInfo.extent);
		NewCreateInfo.arrayLayers				= aArrayLayers;
		NewCreateInfo.samples					= (vk_sample_count_flag_bits)aCreateInfo.Sample;
		NewCreateInfo.tiling					= (vk_image_tiling)aCreateInfo.Tiling;
		NewCreateInfo.usage						= aCreateInfo.Usage;
		NewCreateInfo.sharingMode				= VK_SHARING_MODE_EXCLUSIVE;
		NewCreateInfo.queueFamilyIndexCount		= 0;
		NewCreateInfo.pQueueFamilyIndices		= NULL;
		NewCreateInfo.initialLayout				= VK_IMAGE_LAYOUT_UNDEFINED;
		return NewCreateInfo;
	}

	vk_memory_allocate_info image::pack_allocate_info(context* aContext, device* aDevice, int aMemoryType) {
		vk_memory_allocate_info NewAllocateInfo{};
		vk_memory_requirements ImageMemoryRequirements = aContext->get_image_memory_requirements(this->Handle);
		NewAllocateInfo.sType				= VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		NewAllocateInfo.pNext				= NULL;
		NewAllocateInfo.allocationSize		= ImageMemoryRequirements.size;
		NewAllocateInfo.memoryTypeIndex		= aDevice->get_memory_type_index(ImageMemoryRequirements, aMemoryType);
		return NewAllocateInfo;
	}

	vk_result image::generate_miplevels(unsigned int aMipLevel, unsigned int aArrayLayers, vk_image_layout aInitialLayout, vk_image_type aImageType, vk_extent_3d aExtent) {
		vk_result Result = VK_SUCCESS;

		if (Result == VK_SUCCESS) {

			this->Layout = (vk_image_layout**)malloc(aMipLevel * sizeof(vk_image_layout*));

			this->MipExtent = (vk_extent_3d*)malloc(aMipLevel * sizeof(vk_extent_3d));

			if ((this->Layout != NULL) && (this->MipExtent != NULL)) {
				bool FullyAllocated = true;
				for (uint32_t i = 0; i < aMipLevel; i++) {
					this->Layout[i] = (vk_image_layout*)malloc(aArrayLayers * sizeof(vk_image_layout));
					FullyAllocated &= (this->Layout[i] != NULL);
				}

				if (!FullyAllocated) {
					for (uint32_t i = 0; i < aMipLevel; i++) {
						free(this->Layout[i]);
						this->Layout[i] = NULL;
					}
					free(this->Layout);
					this->Layout = NULL;
					free(this->MipExtent);
					this->MipExtent = NULL;
					Result = VK_ERROR_OUT_OF_HOST_MEMORY;
				}
			}
			else {
				free(this->Layout);
				this->Layout = NULL;
				free(this->MipExtent);
				this->MipExtent = NULL;
				Result = VK_ERROR_OUT_OF_HOST_MEMORY;
			}

		}

		if (Result == VK_SUCCESS) {
			// Set layout for all mip levels and array elements.
			for (uint32_t i = 0; i < aMipLevel; i++) {
				for (uint32_t j = 0; j < aArrayLayers; j++) {
					this->Layout[i][j] = aInitialLayout;
				}
			}

			// Mip Level resolutions.
			for (uint32_t i = 0; i < aMipLevel; i++) {
				switch (aImageType) {
				default:
					return VK_ERROR_FEATURE_NOT_PRESENT;
				case VK_IMAGE_TYPE_1D:
					this->MipExtent[i] = { (aExtent.width >> i), 1u, 1u };
					break;
				case VK_IMAGE_TYPE_2D:
					this->MipExtent[i] = { (aExtent.width >> i), (aExtent.height >> i), 1u };
					break;
				case VK_IMAGE_TYPE_3D:
					this->MipExtent[i] = { (aExtent.width >> i), (aExtent.height >> i), (aExtent.depth >> i) };
					break;
				}
			}
		}
		else {
			// Maybe clear memory here?
			return Result;
		}

		return Result;
	}

	void image::clear_device_memory() {
		if (this->Context != nullptr) {
			if (this->Handle != VK_NULL_HANDLE) {
				vkDestroyImage(this->Context->handle(), this->Handle, NULL);
				this->Handle = VK_NULL_HANDLE;
			}
			if (this->MemoryHandle != VK_NULL_HANDLE) {
				vkFreeMemory(this->Context->handle(), this->MemoryHandle, NULL);
				this->MemoryHandle = VK_NULL_HANDLE;
			}
		}

		if (this->Layout != NULL) {
			for (uint32_t i = 0; i < this->CreateInfo.mipLevels; i++) {
				free(this->Layout[i]); 
				this->Layout[i] = NULL;
			}
		}
		free(this->Layout); 
		this->Layout = NULL;

		free(this->MipExtent); 
		this->MipExtent = NULL;

		this->AllocateInfo = {};
		this->CreateInfo = {};
		this->DeviceSize = 0;
		this->Context = nullptr;
	}

	void image::zero_out() {
		this->Path					= "";
		this->Directory				= "";
		this->Name					= "";
		this->Extension				= "";
		this->ID					= EID_UNKNOWN;

		// Host Memory Stuff.
		this->HostSize				= 0;
		this->HostData				= NULL;
		this->Layers				= 0;
		this->Format				= format::UNDEFINED;
		this->Resolution			= glm::uvec3(0u, 0u, 0u);

		// Device Memory Handles.
		this->Context				= nullptr;
		this->DeviceSize			= 0;
		this->CreateInfo			= {};
		this->Handle				= VK_NULL_HANDLE;
		this->AllocateInfo			= {};
		this->MemoryHandle			= VK_NULL_HANDLE;
		this->Layout				= NULL;
		this->MipExtent				= NULL;
	}

	vk_image_view image::create_system_window_frame(context* aContext, vk_format aFormat, vk_image_usage_flags aUsageFlags, vk_image aImageHandle, glm::uvec3 aImageResolution) {
		this->Resolution = aImageResolution;
		this->Layout = (vk_image_layout**)malloc(sizeof(vk_image_layout*));
		if (Layout == NULL) {
			throw log::message(log::ERROR, log::OUT_OF_HOST_MEMORY, "System Window Creation", log::GEODESUKA, "image", "Frame", "Insufficient Host Memory!");
		}

		this->Layout[0] = (vk_image_layout*)malloc(sizeof(vk_image_layout));
		if (this->Layout[0] == NULL) {
			throw log::message(log::ERROR, log::OUT_OF_HOST_MEMORY, "System Window Creation", log::GEODESUKA, "image", "Frame", "Insufficient Host Memory!");
		}

		this->MipExtent = (vk_extent_3d*)malloc(sizeof(vk_extent_3d));
		if (this->MipExtent == NULL) {
			throw log::message(log::ERROR, log::OUT_OF_HOST_MEMORY, "System Window Creation", log::GEODESUKA, "image", "Frame", "Insufficient Host Memory!");
		}

		this->Layout[0][0] = VK_IMAGE_LAYOUT_UNDEFINED;
		this->MipExtent[0] = { this->Resolution.x, this->Resolution.y, 1u };

		this->Context											= aContext;
		this->CreateInfo.sType									= VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		this->CreateInfo.pNext									= NULL;
		this->CreateInfo.flags									= 0;
		this->CreateInfo.imageType								= VK_IMAGE_TYPE_2D;
		this->CreateInfo.format									= aFormat;
		this->CreateInfo.extent									= this->MipExtent[0];
		this->CreateInfo.mipLevels								= 1;
		this->CreateInfo.arrayLayers							= 1;
		this->CreateInfo.samples								= VK_SAMPLE_COUNT_1_BIT;
		this->CreateInfo.tiling									= VK_IMAGE_TILING_OPTIMAL;
		this->CreateInfo.usage									= aUsageFlags;
		this->CreateInfo.sharingMode							= VK_SHARING_MODE_EXCLUSIVE;
		this->CreateInfo.queueFamilyIndexCount					= 0;
		this->CreateInfo.pQueueFamilyIndices					= NULL;
		this->CreateInfo.initialLayout							= VK_IMAGE_LAYOUT_UNDEFINED;
		this->Handle											= aImageHandle;

		vk_image_view_create_info ImageViewCreateInfo{};
		vk_image_view ImageView = VK_NULL_HANDLE;

		ImageViewCreateInfo.sType								= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageViewCreateInfo.pNext								= NULL;
		ImageViewCreateInfo.flags								= 0;
		ImageViewCreateInfo.image								= this->Handle;
		ImageViewCreateInfo.viewType							= VK_IMAGE_VIEW_TYPE_2D;
		ImageViewCreateInfo.format								= this->CreateInfo.format;
		ImageViewCreateInfo.components.r						= VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.g						= VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.b						= VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.components.a						= VK_COMPONENT_SWIZZLE_IDENTITY;
		ImageViewCreateInfo.subresourceRange.aspectMask			= VK_IMAGE_ASPECT_COLOR_BIT;
		ImageViewCreateInfo.subresourceRange.baseMipLevel		= 0;
		ImageViewCreateInfo.subresourceRange.levelCount			= 1;
		ImageViewCreateInfo.subresourceRange.baseArrayLayer		= 0;
		ImageViewCreateInfo.subresourceRange.layerCount			= 1;

		vk_result Result = vkCreateImageView(this->Context->handle(), &ImageViewCreateInfo, NULL, &ImageView);
		if (Result == VK_SUCCESS) {
			*this->Context << log::message(log::INFO, log::SUCCESS, "System Window", log::VULKAN, "image", "Frame", "Image View Creation Succesful");
		}
		else {
			*this->Context << log::message(Result, "System Window", log::VULKAN, "image", "Frame", "Image View Creation Failed");
		}

		return ImageView;
	}

}
