/*
    MANGO Multimedia Development Platform
    Copyright (C) 2012-2017 Twilight Finland 3D Oy Ltd. All rights reserved.
*/
#include <map>
#include <mango/core/core.hpp>
#include <mango/image/image.hpp>
#include "../../external/google/etc.hpp"
#include "../../external/google/astc.hpp"
#include "../../external/bc/BC.h"

#define MAKE_FORMAT(bits, type, order, s0, s1, s2, s3) \
    Format(bits, Format::type, Format::order, s0, s1, s2, s3)

#define FORMAT_ASTC  MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8)

namespace mango
{

    void decode_block_dxt1           (const TextureCompressionInfo& info, u8* output, const u8* input, int stride); // BC1
    void decode_block_dxt3           (const TextureCompressionInfo& info, u8* output, const u8* input, int stride); // BC2
    void decode_block_dxt5           (const TextureCompressionInfo& info, u8* output, const u8* input, int stride); // BC3
    void decode_block_3dc_x          (const TextureCompressionInfo& info, u8* output, const u8* input, int stride); // BC4U
    void decode_block_3dc_xy         (const TextureCompressionInfo& info, u8* output, const u8* input, int stride); // BC5U
    void decode_block_uyvy           (const TextureCompressionInfo& info, u8* output, const u8* input, int stride);
    void decode_block_yuy2           (const TextureCompressionInfo& info, u8* output, const u8* input, int stride);
    void decode_block_grgb8          (const TextureCompressionInfo& info, u8* output, const u8* input, int stride);
    void decode_block_rgbg8          (const TextureCompressionInfo& info, u8* output, const u8* input, int stride);
    void decode_block_rgb9e5         (const TextureCompressionInfo& info, u8* output, const u8* input, int stride);
    void decode_block_r11f_g11f_b10f (const TextureCompressionInfo& info, u8* output, const u8* input, int stride);
    void decode_block_r10f_g11f_b11f (const TextureCompressionInfo& info, u8* output, const u8* input, int stride);
    void decode_block_pvrtc          (const TextureCompressionInfo& info, u8* output, const u8* input, int stride);
    void decode_block_atc            (const TextureCompressionInfo& info, u8* output, const u8* input, int stride);
    void decode_block_atc_e          (const TextureCompressionInfo& info, u8* output, const u8* input, int stride);
    void decode_block_atc_i          (const TextureCompressionInfo& info, u8* output, const u8* input, int stride);

    void encode_block_etc1           (const TextureCompressionInfo& info, u8* output, const u8* input, int stride);

} // namespace mango

namespace
{
    using namespace mango;

    const TextureCompressionInfo g_blockTable[] =
    {

        // NONE

        TextureCompressionInfo(),

        // AMD_compressed_ATC_texture

        TextureCompressionInfo(
            TextureCompression::ATC_RGB,
            0,
            opengl::ATC_RGB_AMD,
            0,
            4, 4,  8, MAKE_FORMAT(32, UNORM, BGRA, 8, 8, 8, 8), decode_block_atc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ATC_RGBA_EXPLICIT_ALPHA,
            0,
            opengl::ATC_RGBA_EXPLICIT_ALPHA_AMD,
            0,
            4, 4, 16, MAKE_FORMAT(32, UNORM, BGRA, 8, 8, 8, 8), decode_block_atc_e, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ATC_RGBA_INTERPOLATED_ALPHA,
            0,
            opengl::ATC_RGBA_INTERPOLATED_ALPHA_AMD,
            0,
            4, 4, 16, MAKE_FORMAT(32, UNORM, BGRA, 8, 8, 8, 8), decode_block_atc_i, nullptr
        ),

        // AMD_compressed_3DC_texture

        TextureCompressionInfo(
            TextureCompression::AMD_3DC_X,
            dxgi::FORMAT_BC4_UNORM,
            opengl::AMD_3DC_X,
            vulkan::FORMAT_BC4_UNORM_BLOCK,
            4, 4,  8, MAKE_FORMAT(8, UNORM, R, 8, 0, 0, 0), decode_block_3dc_x, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::AMD_3DC_XY,
            dxgi::FORMAT_BC5_UNORM,
            opengl::AMD_3DC_XY,
            vulkan::FORMAT_BC5_UNORM_BLOCK,
            4, 4, 16, MAKE_FORMAT(16, UNORM, RG, 8, 8, 0, 0), decode_block_3dc_xy, nullptr
        ),

		// LATC

        TextureCompressionInfo(
            TextureCompression::LATC1_LUMINANCE,
            0,
            opengl::COMPRESSED_LUMINANCE_LATC1_EXT,
            0,
            4, 4,  8, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::LATC1_SIGNED_LUMINANCE,
            0,
            opengl::COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT,
            0,
            4, 4,  8, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::LATC2_LUMINANCE_ALPHA,
            0,
            opengl::COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT,
            0,
            4, 4, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::LATC2_SIGNED_LUMINANCE_ALPHA,
            0,
            opengl::COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT,
            0,
            4, 4, 16, FORMAT_NONE, nullptr, nullptr
        ),

        // DXT

        TextureCompressionInfo(
            TextureCompression::DXT1,
            dxgi::FORMAT_BC1_UNORM,
            opengl::COMPRESSED_RGB_S3TC_DXT1_EXT,
            vulkan::FORMAT_BC1_RGB_UNORM_BLOCK,
            4, 4,  8, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_dxt1, encode_block_bc1
        ),

        TextureCompressionInfo(
            TextureCompression::DXT1_SRGB,
            dxgi::FORMAT_BC1_UNORM_SRGB,
            opengl::COMPRESSED_SRGB_S3TC_DXT1_EXT,
            vulkan::FORMAT_BC1_RGB_SRGB_BLOCK,
            4, 4,  8, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_dxt1, encode_block_bc1
        ),

        TextureCompressionInfo(
            TextureCompression::DXT1_ALPHA1,
            0,
            opengl::COMPRESSED_RGBA_S3TC_DXT1_EXT,
            vulkan::FORMAT_BC1_RGBA_UNORM_BLOCK,
            4, 4,  8, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_dxt1, encode_block_bc1a
        ),

        TextureCompressionInfo(
            TextureCompression::DXT1_ALPHA1_SRGB,
            0,
            opengl::COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT,
            vulkan::FORMAT_BC1_RGBA_SRGB_BLOCK,
            4, 4,  8, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_dxt1, encode_block_bc1a
        ),

        TextureCompressionInfo(
            TextureCompression::DXT3,
            dxgi::FORMAT_BC2_UNORM,
            opengl::COMPRESSED_RGBA_S3TC_DXT3_EXT,
            vulkan::FORMAT_BC2_UNORM_BLOCK,
            4, 4, 16, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_dxt3, encode_block_bc2
        ),

        TextureCompressionInfo(
            TextureCompression::DXT3_SRGB,
            dxgi::FORMAT_BC2_UNORM_SRGB,
            opengl::COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT,
            vulkan::FORMAT_BC2_SRGB_BLOCK,
            4, 4, 16, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_dxt3, encode_block_bc2
        ),

        TextureCompressionInfo(
            TextureCompression::DXT5,
            dxgi::FORMAT_BC3_UNORM,
            opengl::COMPRESSED_RGBA_S3TC_DXT5_EXT,
            vulkan::FORMAT_BC3_UNORM_BLOCK,
            4, 4, 16, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_dxt5, encode_block_bc3
        ),

        TextureCompressionInfo(
            TextureCompression::DXT5_SRGB,
            dxgi::FORMAT_BC3_UNORM_SRGB,
            opengl::COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT,
            vulkan::FORMAT_BC3_SRGB_BLOCK,
            4, 4, 16, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_dxt5, encode_block_bc3
        ),

#ifdef MANGO_ENABLE_LICENSE_MICROSOFT

        // RGTC

        TextureCompressionInfo(
            TextureCompression::RGTC1_RED,
            dxgi::FORMAT_BC4_UNORM,
            opengl::COMPRESSED_RED_RGTC1,
            vulkan::FORMAT_BC4_UNORM_BLOCK,
            4, 4,  8, MAKE_FORMAT(128, FLOAT32, RGBA, 32, 32, 32, 32), decode_block_bc4u, encode_block_bc4u
        ),

        TextureCompressionInfo(
            TextureCompression::RGTC1_SIGNED_RED,
            dxgi::FORMAT_BC4_SNORM,
            opengl::COMPRESSED_SIGNED_RED_RGTC1,
            vulkan::FORMAT_BC4_SNORM_BLOCK,
            4, 4,  8, MAKE_FORMAT(128, FLOAT32, RGBA, 32, 32, 32, 32), decode_block_bc4s, encode_block_bc4s
        ),

        TextureCompressionInfo(
            TextureCompression::RGTC2_RG,
            dxgi::FORMAT_BC5_UNORM,
            opengl::COMPRESSED_RG_RGTC2,
            vulkan::FORMAT_BC5_UNORM_BLOCK,
            4, 4, 16, MAKE_FORMAT(128, FLOAT32, RGBA, 32, 32, 32, 32), decode_block_bc5u, encode_block_bc5u
        ),

        TextureCompressionInfo(
            TextureCompression::RGTC2_SIGNED_RG,
            dxgi::FORMAT_BC5_SNORM,
            opengl::COMPRESSED_SIGNED_RG_RGTC2,
            vulkan::FORMAT_BC5_SNORM_BLOCK,
            4, 4, 16, MAKE_FORMAT(128, FLOAT32, RGBA, 32, 32, 32, 32), decode_block_bc5s, encode_block_bc5s
        ),

        // BPTC

        TextureCompressionInfo(
            TextureCompression::BPTC_RGB_UNSIGNED_FLOAT,
            dxgi::FORMAT_BC6H_UF16,
            opengl::COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT,
            vulkan::FORMAT_BC6H_UFLOAT_BLOCK,
            4, 4, 16, MAKE_FORMAT(128, FLOAT32, RGBA, 32, 32, 32, 32), decode_block_bc6hu, encode_block_bc6hu
        ),

        TextureCompressionInfo(
            TextureCompression::BPTC_RGB_SIGNED_FLOAT,
            dxgi::FORMAT_BC6H_SF16,
            opengl::COMPRESSED_RGB_BPTC_SIGNED_FLOAT,
            vulkan::FORMAT_BC6H_SFLOAT_BLOCK,
            4, 4, 16, MAKE_FORMAT(128, FLOAT32, RGBA, 32, 32, 32, 32), decode_block_bc6hs, encode_block_bc6hs
        ),

        TextureCompressionInfo(
            TextureCompression::BPTC_RGBA_UNORM,
            dxgi::FORMAT_BC7_UNORM,
            opengl::COMPRESSED_RGBA_BPTC_UNORM,
            vulkan::FORMAT_BC7_UNORM_BLOCK,
            4, 4, 16, MAKE_FORMAT(128, FLOAT32, RGBA, 32, 32, 32, 32), decode_block_bc7, encode_block_bc7
        ),

        TextureCompressionInfo(
            TextureCompression::BPTC_SRGB_ALPHA_UNORM,
            dxgi::FORMAT_BC7_UNORM_SRGB,
            opengl::COMPRESSED_SRGB_ALPHA_BPTC_UNORM,
            vulkan::FORMAT_BC7_SRGB_BLOCK,
            4, 4, 16, MAKE_FORMAT(128, FLOAT32, RGBA, 32, 32, 32, 32), decode_block_bc7, encode_block_bc7
        ),

#endif

        // IMG_texture_compression_pvrtc

        TextureCompressionInfo(
            TextureCompression::PVRTC_RGB_4BPP,
            0,
            opengl::COMPRESSED_RGB_PVRTC_4BPPV1_IMG,
            0,
            4, 4, 8, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_pvrtc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::PVRTC_RGB_2BPP,
            0,
            opengl::COMPRESSED_RGB_PVRTC_2BPPV1_IMG,
            0,
            8, 4, 8, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_pvrtc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::PVRTC_RGBA_4BPP,
            0,
            opengl::COMPRESSED_RGBA_PVRTC_4BPPV1_IMG,
            vulkan::FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG,
            4, 4, 8, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_pvrtc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::PVRTC_RGBA_2BPP,
            0,
            opengl::COMPRESSED_RGBA_PVRTC_2BPPV1_IMG,
            vulkan::FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG,
            8, 4, 8, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_pvrtc, nullptr
        ),

        // IMG_texture_compression_pvrtc2

        TextureCompressionInfo(
            TextureCompression::PVRTC2_RGBA_2BPP,
            0,
            opengl::COMPRESSED_RGBA_PVRTC_2BPPV2_IMG,
            vulkan::FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG,
            8, 4, 8, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::PVRTC2_RGBA_4BPP,
            0,
            opengl::COMPRESSED_RGBA_PVRTC_4BPPV2_IMG,
            vulkan::FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG,
            4, 4, 8, FORMAT_NONE, nullptr, nullptr
        ),

        // EXT_pvrtc_sRGB

        TextureCompressionInfo(
            TextureCompression::PVRTC_SRGB_2BPP,
            0,
            opengl::COMPRESSED_SRGB_PVRTC_2BPPV1_EXT,
            0,
            8, 8, 16, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_pvrtc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::PVRTC_SRGB_4BPP,
            0,
            opengl::COMPRESSED_SRGB_PVRTC_4BPPV1_EXT,
            0,
            8, 8, 32, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_pvrtc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::PVRTC_SRGB_ALPHA_2BPP,
            0,
            opengl::COMPRESSED_SRGB_ALPHA_PVRTC_2BPPV1_EXT,
            vulkan::FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG,
            8, 8, 16, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_pvrtc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::PVRTC_SRGB_ALPHA_4BPP,
            0,
            opengl::COMPRESSED_SRGB_ALPHA_PVRTC_4BPPV1_EXT,
            vulkan::FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG,
            8, 8, 32, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_pvrtc, nullptr
        ),

#ifdef MANGO_ENABLE_LICENSE_APACHE

        // OES_compressed_ETC1_RGB8_texture

        TextureCompressionInfo(
            TextureCompression::ETC1_RGB,
            0,
            opengl::ETC1_RGB8_OES,
            0,
            4, 4, 8, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_etc1, encode_block_etc1
        ),

        // ETC2 / EAC

        TextureCompressionInfo(
            TextureCompression::EAC_R11,
            0,
            opengl::COMPRESSED_R11_EAC,
            vulkan::FORMAT_EAC_R11_UNORM_BLOCK,
            4, 4,  8, MAKE_FORMAT(16, UNORM, R, 16, 0, 0, 0), decode_block_eac_r11, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::EAC_SIGNED_R11,
            0,
            opengl::COMPRESSED_SIGNED_R11_EAC,
            vulkan::FORMAT_EAC_R11_SNORM_BLOCK,
            4, 4,  8, MAKE_FORMAT(16, SNORM, R, 16, 0, 0, 0), decode_block_eac_r11, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::EAC_RG11,
            0,
            opengl::COMPRESSED_RG11_EAC,
            vulkan::FORMAT_EAC_R11G11_UNORM_BLOCK,
            4, 4, 16, MAKE_FORMAT(32, UNORM, RG, 16, 16, 0, 0), decode_block_eac_rg11, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::EAC_SIGNED_RG11,
            0,
            opengl::COMPRESSED_SIGNED_RG11_EAC,
            vulkan::FORMAT_EAC_R11G11_SNORM_BLOCK,
            4, 4, 16, MAKE_FORMAT(32, SNORM, RG, 16, 16, 0, 0), decode_block_eac_rg11, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ETC2_RGB,
            0,
            opengl::COMPRESSED_RGB8_ETC2,
            vulkan::FORMAT_ETC2_R8G8B8_UNORM_BLOCK,
            4, 4,  8, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_etc2, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ETC2_SRGB,
            0,
            opengl::COMPRESSED_SRGB8_ETC2,
            vulkan::FORMAT_ETC2_R8G8B8_SRGB_BLOCK,
            4, 4,  8, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_etc2, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ETC2_RGB_ALPHA1,
            0,
            opengl::COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
            vulkan::FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK,
            4, 4,  8, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_etc2, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ETC2_SRGB_ALPHA1,
            0,
            opengl::COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
            vulkan::FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK,
            4, 4,  8, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_etc2, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ETC2_RGBA,
            0,
            opengl::COMPRESSED_RGBA8_ETC2_EAC,
            vulkan::FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK,
            4, 4, 16, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_etc2_eac, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ETC2_SRGB_ALPHA8,
            0,
            opengl::COMPRESSED_SRGB8_ALPHA8_ETC2_EAC,
            vulkan::FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK,
            4, 4, 16, MAKE_FORMAT(32, UNORM, RGBA, 8, 8, 8, 8), decode_block_etc2_eac, nullptr
        ),

        // KHR_texture_compression_astc_ldr
        // KHR_texture_compression_astc_hdr

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_4x4,
            dxgi::FORMAT_ASTC_4X4_UNORM,
            opengl::COMPRESSED_RGBA_ASTC_4x4_KHR,
            vulkan::FORMAT_ASTC_4x4_UNORM_BLOCK,
            4,  4, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_5x4,
            dxgi::FORMAT_ASTC_5X4_UNORM,
            opengl::COMPRESSED_RGBA_ASTC_5x4_KHR,
            vulkan::FORMAT_ASTC_5x4_UNORM_BLOCK,
            5,  4, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_5x5,
            dxgi::FORMAT_ASTC_5X5_UNORM,
            opengl::COMPRESSED_RGBA_ASTC_5x5_KHR,
            vulkan::FORMAT_ASTC_5x5_UNORM_BLOCK,
            5,  5, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_6x5,
            dxgi::FORMAT_ASTC_6X5_UNORM,
            opengl::COMPRESSED_RGBA_ASTC_6x5_KHR,
            vulkan::FORMAT_ASTC_6x5_UNORM_BLOCK,
            6,  5, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_6x6,
            dxgi::FORMAT_ASTC_6X6_UNORM,
            opengl::COMPRESSED_RGBA_ASTC_6x6_KHR,
            vulkan::FORMAT_ASTC_6x6_UNORM_BLOCK,
            6,  6, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_8x5,
            dxgi::FORMAT_ASTC_8X5_UNORM,
            opengl::COMPRESSED_RGBA_ASTC_8x5_KHR,
            vulkan::FORMAT_ASTC_8x5_UNORM_BLOCK,
            8,  5, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_8x6,
            dxgi::FORMAT_ASTC_8X6_UNORM,
            opengl::COMPRESSED_RGBA_ASTC_8x6_KHR,
            vulkan::FORMAT_ASTC_8x6_UNORM_BLOCK,
            8,  6, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_8x8,
            dxgi::FORMAT_ASTC_8X8_UNORM,
            opengl::COMPRESSED_RGBA_ASTC_8x8_KHR,
            vulkan::FORMAT_ASTC_8x8_UNORM_BLOCK,
            8,  8, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_10x5,
            dxgi::FORMAT_ASTC_10X5_UNORM,
            opengl::COMPRESSED_RGBA_ASTC_10x5_KHR,
            vulkan::FORMAT_ASTC_10x5_UNORM_BLOCK,
            10,  5, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_10x6,
            dxgi::FORMAT_ASTC_10X6_UNORM,
            opengl::COMPRESSED_RGBA_ASTC_10x6_KHR,
            vulkan::FORMAT_ASTC_10x6_UNORM_BLOCK,
            10,  6, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_10x8,
            dxgi::FORMAT_ASTC_10X8_UNORM,
            opengl::COMPRESSED_RGBA_ASTC_10x8_KHR,
            vulkan::FORMAT_ASTC_10x8_UNORM_BLOCK,
            10,  8, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_10x10,
            dxgi::FORMAT_ASTC_10X10_UNORM,
            opengl::COMPRESSED_RGBA_ASTC_10x10_KHR,
            vulkan::FORMAT_ASTC_10x10_UNORM_BLOCK,
            10, 10, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_12x10,
            dxgi::FORMAT_ASTC_12X10_UNORM,
            opengl::COMPRESSED_RGBA_ASTC_12x10_KHR,
            vulkan::FORMAT_ASTC_12x10_UNORM_BLOCK,
            12, 10, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_12x12,
            dxgi::FORMAT_ASTC_12X12_UNORM,
            opengl::COMPRESSED_RGBA_ASTC_12x12_KHR,
            vulkan::FORMAT_ASTC_12x12_UNORM_BLOCK,
            12, 12, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_4x4,
            dxgi::FORMAT_ASTC_4X4_UNORM_SRGB,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,
            vulkan::FORMAT_ASTC_4x4_SRGB_BLOCK,
            4,  4, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_5x4,
            dxgi::FORMAT_ASTC_5X4_UNORM_SRGB,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR,
            vulkan::FORMAT_ASTC_5x4_SRGB_BLOCK,
            5,  4, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_5x5,
            dxgi::FORMAT_ASTC_5X5_UNORM_SRGB,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR,
            vulkan::FORMAT_ASTC_5x5_SRGB_BLOCK,
            5,  5, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_6x5,
            dxgi::FORMAT_ASTC_6X5_UNORM_SRGB,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR,
            vulkan::FORMAT_ASTC_6x5_SRGB_BLOCK,
            6,  5, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_6x6,
            dxgi::FORMAT_ASTC_6X6_UNORM_SRGB,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR,
            vulkan::FORMAT_ASTC_6x6_SRGB_BLOCK,
            6,  6, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_8x5,
            dxgi::FORMAT_ASTC_8X5_UNORM_SRGB,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR,
            vulkan::FORMAT_ASTC_8x5_SRGB_BLOCK,
            8,  5, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_8x6,
            dxgi::FORMAT_ASTC_8X6_UNORM_SRGB,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR,
            vulkan::FORMAT_ASTC_8x6_SRGB_BLOCK,
            8,  6, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_8x8,
            dxgi::FORMAT_ASTC_8X8_UNORM_SRGB,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR,
            vulkan::FORMAT_ASTC_8x8_SRGB_BLOCK,
            8,  8, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_10x5,
            dxgi::FORMAT_ASTC_10X5_UNORM_SRGB,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR,
            vulkan::FORMAT_ASTC_10x5_SRGB_BLOCK,
            10,  5, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_10x6,
            dxgi::FORMAT_ASTC_10X6_UNORM_SRGB,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR,
            vulkan::FORMAT_ASTC_10x6_SRGB_BLOCK,
            10,  6, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_10x8,
            dxgi::FORMAT_ASTC_10X8_UNORM_SRGB,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR,
            vulkan::FORMAT_ASTC_10x8_SRGB_BLOCK,
            10,  8, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_10x10,
            dxgi::FORMAT_ASTC_10X10_UNORM_SRGB,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR,
            vulkan::FORMAT_ASTC_10x10_SRGB_BLOCK,
            10, 10, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_12x10,
            dxgi::FORMAT_ASTC_12X10_UNORM_SRGB,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR,
            vulkan::FORMAT_ASTC_12x10_SRGB_BLOCK,
            12, 10, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_12x12,
            dxgi::FORMAT_ASTC_12X12_UNORM_SRGB,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR,
            vulkan::FORMAT_ASTC_12x12_SRGB_BLOCK,
            12, 12, 16, FORMAT_ASTC, decode_block_astc, nullptr
        ),

        // OES_texture_compression_astc

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_3x3x3,
            0,
            opengl::COMPRESSED_RGBA_ASTC_3x3x3_OES,
            0,
            3, 3, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_4x3x3,
            0,
            opengl::COMPRESSED_RGBA_ASTC_4x3x3_OES,
            0,
            4, 3, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_4x4x3,
            0,
            opengl::COMPRESSED_RGBA_ASTC_4x4x3_OES,
            0,
            4, 4, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_4x4x4,
            0,
            opengl::COMPRESSED_RGBA_ASTC_4x4x4_OES,
            0,
            4, 4, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_5x4x4,
            0,
            opengl::COMPRESSED_RGBA_ASTC_5x4x4_OES,
            0,
            5, 4, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_5x5x4,
            0,
            opengl::COMPRESSED_RGBA_ASTC_5x5x4_OES,
            0,
            5, 5, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_5x5x5,
            0,
            opengl::COMPRESSED_RGBA_ASTC_5x5x5_OES,
            0,
            5, 5, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_6x5x5,
            0,
            opengl::COMPRESSED_RGBA_ASTC_6x5x5_OES,
            0,
            6, 5, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_6x6x5,
            0,
            opengl::COMPRESSED_RGBA_ASTC_6x6x5_OES,
            0,
            6, 6, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_RGBA_6x6x6,
            0,
            opengl::COMPRESSED_RGBA_ASTC_6x6x6_OES,
            0,
            6, 6, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_3x3x3,
            0,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_3x3x3_OES,
            0,
            3, 3, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_4x3x3,
            0,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_4x3x3_OES,
            0,
            4, 3, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_4x4x3,
            0,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x3_OES,
            0,
            4, 4, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_4x4x4,
            0,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_4x4x4_OES,
            0,
            4, 4, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_5x4x4,
            0,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_5x4x4_OES,
            0,
            5, 4, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_5x5x4,
            0,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x4_OES,
            0,
            5, 5, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_5x5x5,
            0,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_5x5x5_OES,
            0,
            5, 5, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_6x5x5,
            0,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_6x5x5_OES,
            0,
            6, 5, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_6x6x5,
            0,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x5_OES,
            0,
            6, 6, 16, FORMAT_NONE, nullptr, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::ASTC_SRGB_ALPHA_6x6x6,
            0,
            opengl::COMPRESSED_SRGB8_ALPHA8_ASTC_6x6x6_OES,
            0,
            6, 6, 16, FORMAT_NONE, nullptr, nullptr
        ),

#endif

        // Packed Pixel

        TextureCompressionInfo(
            TextureCompression::RGB9_E5,
            dxgi::FORMAT_R9G9B9E5_SHAREDEXP,
            0x8C3D,
            0,
            1, 1, 4, MAKE_FORMAT(128, FLOAT32, RGBA, 32, 32, 32, 32), decode_block_rgb9e5, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::R11F_G11F_B10F,
            0,
            0x8C3A,
            0,
            1, 1, 4, MAKE_FORMAT(128, FLOAT32, RGBA, 32, 32, 32, 32), decode_block_r11f_g11f_b10f, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::R10F_G11F_B11F,
            0,
            0,
            0,
            1, 1, 4, MAKE_FORMAT(128, FLOAT32, RGBA, 32, 32, 32, 32), decode_block_r10f_g11f_b11f, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::G8R8G8B8,
            dxgi::FORMAT_G8R8_G8B8_UNORM,
            0,
            0,
            2, 1, 4, FORMAT_R8G8B8A8, decode_block_grgb8, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::R8G8B8G8,
            dxgi::FORMAT_R8G8_B8G8_UNORM,
            0,
            0,
            2, 1, 4, FORMAT_R8G8B8A8, decode_block_rgbg8, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::UYVY,
            0,
            0,
            0,
            2, 1, 4, FORMAT_R8G8B8A8, decode_block_uyvy, nullptr
        ),

        TextureCompressionInfo(
            TextureCompression::YUY2,
            0,
            0,
            0,
            2, 1, 4, FORMAT_R8G8B8A8, decode_block_yuy2, nullptr
        ),
    };

    // block decode

    void directBlockDecode(const TextureCompressionInfo& block, const Surface& surface, Memory memory, int xsize, int ysize)
    {
        const int blockImageSize = block.width * surface.format.bytes();
        const int blockImageStride = block.height * surface.stride;

        const bool origin = (block.getCompressionFlags() & TextureCompressionInfo::ORIGIN) != 0;
        const u8* data = memory.address;

        ConcurrentQueue queue;

        for (int y = 0; y < ysize; ++y)
        {
            u8* image = surface.image;
            int stride = surface.stride;

            if (origin)
            {
                image += (ysize - y) * blockImageStride;
                image -= stride;
                stride = -stride;
            }
            else
            {
                image += y * blockImageStride;
            }

            queue.enqueue([&] (u8* image, const u8* data)
            {
                for (int x = 0; x < xsize; ++x)
                {
                    block.decode(block, image, data, stride);
                    image += blockImageSize;
                    data += block.bytes;
                }
            }, image, data);

            data += block.bytes * xsize;
        }

        queue.wait();
    }

    void clipConvertBlockDecode(const TextureCompressionInfo& block, const Surface& surface, Memory memory, int xsize, int ysize)
    {
        Blitter blitter(surface.format, block.format);

        const bool origin = (block.getCompressionFlags() & TextureCompressionInfo::ORIGIN) != 0;
        const u8* data = memory.address;

        BlitRect rect;
        rect.dest.stride = origin ? -surface.stride : surface.stride;
        rect.src.stride = block.width * block.format.bytes();

        const int blockStride = block.width * surface.format.bytes();
        const int xblocks = ceil_div(surface.width, block.width);

        ConcurrentQueue queue;

        for (int y = 0; y < surface.height; y += block.height)
        {
            rect.dest.address = surface.image + (origin ? surface.height - y - 1 : y) * surface.stride;
            rect.height = std::min(y + block.height, surface.height) - y; // vertical clipping

            queue.enqueue([&] (BlitRect rect, const u8* data)
            {
                Buffer temp(block.height * rect.src.stride);
                rect.src.address = temp;

                for (int x = 0; x < surface.width; x += block.width)
                {
                    block.decode(block, temp, data, rect.src.stride);

                    rect.width = std::min(x + block.width, surface.width) - x; // horizontal clipping
                    blitter.convert(rect);

                    rect.dest.address += blockStride;
                    data += block.bytes;
                }
            }, rect, data);

            data += block.bytes * xblocks;
        }

        queue.wait();
    }

    // surface decode

    void directSurfaceDecode(const TextureCompressionInfo& block, const Surface& surface, Memory memory, int xsize, int ysize)
    {
        TextureCompressionInfo temp = block;
        temp.width = surface.width;
        temp.height = surface.height;
        temp.decode(temp, surface.image, memory.address, surface.stride);
    }

    void clipConvertSurfaceDecode(const TextureCompressionInfo& block, const Surface& surface, Memory memory, int xsize, int ysize)
    {
        TextureCompressionInfo temp = block;
        temp.width = surface.width;
        temp.height = surface.height;

        Bitmap bitmap(surface.width, surface.height, block.format);
        temp.decode(temp, bitmap.image, memory.address, bitmap.stride);
        Surface(surface).blit(0, 0, bitmap);
    }

} // namespace

namespace mango
{

	// ----------------------------------------------------------------------------
    // TextureCompressionInfo
    // ----------------------------------------------------------------------------

    TextureCompressionInfo::TextureCompressionInfo()
        : compression(TextureCompression::NONE)
        , dxgi(0)
        , gl(0)
        , vk(0)
        , width(1)
        , height(1)
        , bytes(0)
        , format(FORMAT_NONE)
        , decode(nullptr)
        , encode(nullptr)
    {
    }

    TextureCompressionInfo::TextureCompressionInfo(
        TextureCompression compression, u32 dxgi, u32 gl, u32 vk,
        int width, int height, int bytes, const Format& format, DecodeFunc decode, EncodeFunc encode)
        : compression(compression)
        , dxgi(dxgi)
        , gl(gl)
        , vk(vk)
        , width(width)
        , height(height)
        , bytes(bytes)
        , format(format)
        , decode(decode)
        , encode(encode)
    {
    }

    TextureCompressionInfo::TextureCompressionInfo(TextureCompression comp)
    {
        const TextureCompressionInfo* info = &g_blockTable[0];

        for (const auto& node : g_blockTable)
        {
            if (node.compression == comp)
            {
                info = &node;
                break;
            }
        }

        *this = *info;
    }

    TextureCompressionInfo::TextureCompressionInfo(dxgi::TextureFormat format)
    {
        const TextureCompressionInfo* info = &g_blockTable[0];

        for (const auto& node : g_blockTable)
        {
            if (node.dxgi == format)
            {
                info = &node;
                break;
            }
        }

        *this = *info;
    }

    TextureCompressionInfo::TextureCompressionInfo(opengl::TextureFormat format)
    {
        const TextureCompressionInfo* info = &g_blockTable[0];

        for (const auto& node : g_blockTable)
        {
            if (node.gl == format)
            {
                info = &node;
                break;
            }
        }

        *this = *info;
    }

    TextureCompressionInfo::TextureCompressionInfo(vulkan::TextureFormat format)
    {
        const TextureCompressionInfo* info = &g_blockTable[0];

        for (const auto& node : g_blockTable)
        {
            if (node.vk == format)
            {
                info = &node;
                break;
            }
        }

        *this = *info;
    }

    TextureCompressionStatus TextureCompressionInfo::decompress(const Surface& surface, Memory memory) const
    {
        TextureCompressionStatus status;

        if (!decode)
        {
            status.setError("No decoder for 0x%x.", compression);
            return status;
        }

        const int xsize = ceil_div(surface.width, width);
        const int ysize = ceil_div(surface.height, height);

        const bool noclip = surface.width == (xsize * width) &&
                            surface.height == (ysize * height);
        const bool noconvert = surface.format == format;
        const bool direct = noclip && noconvert;

        if (getCompressionFlags() & TextureCompressionInfo::SURFACE)
        {
            if (direct)
            {
                directSurfaceDecode(*this, surface, memory, xsize, ysize);
            }
            else
            {
                clipConvertSurfaceDecode(*this, surface, memory, xsize, ysize);
            }
        }
        else
        {
            if (direct)
            {
                directBlockDecode(*this, surface, memory, xsize, ysize);
            }
            else
            {
                clipConvertBlockDecode(*this, surface, memory, xsize, ysize);
            }
        }

        status.direct = direct;

        return status;
    }

    TextureCompressionStatus TextureCompressionInfo::compress(Memory memory, const Surface& surface) const
    {
        TextureCompressionStatus status;

        if (!encode)
        {
            status.setError("No encoder for 0x%x.", compression);
            return status;
        }

        ConcurrentQueue queue;

        u8* address = memory.address;

        const int xblocks = ceil_div(surface.width, width);
        const int yblocks = ceil_div(surface.height, height);

        for (int y = 0; y < yblocks; ++y)
        {
            queue.enqueue([this, y, xblocks, &surface, address]
            {
                Bitmap temp(width, height, format);
                u8* data = address + y * xblocks * bytes;

                for (int x = 0; x < xblocks; ++x)
                {
                    Surface source(surface, x * width, y * height, width, height);
                    temp.blit(0, 0, source);

                    u8* image = temp.address<u8>();
                    encode(*this, data, image, temp.stride);
                    data += bytes;
                }
            });
        }

        queue.wait();

        return status;
    }

} // namespace mango
