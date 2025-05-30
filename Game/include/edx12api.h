#pragma once

#include <d3dcommon.h>
#include <d3d12.h>


namespace EProject
{
    /*void getD3DErr(HRESULT hres) {
        switch (hres)
        {
        case(S_OK): return;
        case(S_FALSE): throw std::runtime_error("S_FALSE ");
        case(E_NOTIMPL): throw std::runtime_error("E_NOTIMPL");
        case(E_OUTOFMEMORY): throw std::runtime_error("E_OUTOFMEMORY");
        case(E_INVALIDARG): throw std::runtime_error("E_INVALIDARG");
        case(E_FAIL): throw std::runtime_error("E_FAIL");
        case(DXGI_ERROR_WAS_STILL_DRAWING): throw std::runtime_error("DXGI_ERROR_WAS_STILL_DRAWING");
        case(DXGI_ERROR_INVALID_CALL): throw std::runtime_error("DXGI_ERROR_INVALID_CALL ");
        case(D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD): throw std::runtime_error("D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD");
        case(D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS): throw std::runtime_error("D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS");
        case(D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS): throw std::runtime_error("D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS");
        case(D3D11_ERROR_FILE_NOT_FOUND): throw std::runtime_error("D3D11_ERROR_FILE_NOT_FOUND");
        case(DXGI_ERROR_ACCESS_DENIED): throw std::runtime_error("DXGI_ERROR_ACCESS_DENIED");
        case(DXGI_ERROR_ACCESS_LOST): throw std::runtime_error("DXGI_ERROR_ACCESS_LOST");
        case(DXGI_ERROR_ALREADY_EXISTS): throw std::runtime_error("DXGI_ERROR_ALREADY_EXISTS");
        case(DXGI_ERROR_CANNOT_PROTECT_CONTENT): throw std::runtime_error("DXGI_ERROR_CANNOT_PROTECT_CONTENT");
        case(DXGI_ERROR_DEVICE_HUNG): throw std::runtime_error("DXGI_ERROR_DEVICE_HUNG");
        case(DXGI_ERROR_DEVICE_REMOVED): throw std::runtime_error("DXGI_ERROR_DEVICE_REMOVED");
        case(DXGI_ERROR_DEVICE_RESET): throw std::runtime_error("DXGI_ERROR_DEVICE_RESET");
        case(DXGI_ERROR_DRIVER_INTERNAL_ERROR): throw std::runtime_error("DXGI_ERROR_DRIVER_INTERNAL_ERROR");
        case(DXGI_ERROR_FRAME_STATISTICS_DISJOINT): throw std::runtime_error("DXGI_ERROR_FRAME_STATISTICS_DISJOINT");
        case(DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE): throw std::runtime_error("DXGI_ERROR_GRAPHICS_VIDPN_SOURCE_IN_USE");
        case(DXGI_ERROR_MORE_DATA): throw std::runtime_error("DXGI_ERROR_MORE_DATA");
        case(DXGI_ERROR_NAME_ALREADY_EXISTS): throw std::runtime_error("DXGI_ERROR_NAME_ALREADY_EXISTS");
        case(DXGI_ERROR_NONEXCLUSIVE): throw std::runtime_error("DXGI_ERROR_NONEXCLUSIVE");
        case(DXGI_ERROR_NOT_CURRENTLY_AVAILABLE): throw std::runtime_error("DXGI_ERROR_NOT_CURRENTLY_AVAILABLE");
        case(DXGI_ERROR_NOT_FOUND): throw std::runtime_error("DXGI_ERROR_NOT_FOUND");
        case(DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED): throw std::runtime_error("DXGI_ERROR_REMOTE_CLIENT_DISCONNECTED");
        case(DXGI_ERROR_REMOTE_OUTOFMEMORY): throw std::runtime_error("DXGI_ERROR_REMOTE_OUTOFMEMORY");
        case(DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE): throw std::runtime_error("DXGI_ERROR_RESTRICT_TO_OUTPUT_STALE");
        case(DXGI_ERROR_SDK_COMPONENT_MISSING): throw std::runtime_error("DXGI_ERROR_SDK_COMPONENT_MISSING");
        case(DXGI_ERROR_SESSION_DISCONNECTED): throw std::runtime_error("DXGI_ERROR_SESSION_DISCONNECTED");
        case(DXGI_ERROR_UNSUPPORTED): throw std::runtime_error("DXGI_ERROR_UNSUPPORTED");
        case(DXGI_ERROR_WAIT_TIMEOUT): throw std::runtime_error("DXGI_ERROR_WAIT_TIMEOUT");
        default:
            auto s = std::to_string(hres);
            throw std::runtime_error(s.c_str());
        }
    }

    DXGI_FORMAT toDXGI_Fmt(TextureFmt fmt)
    {
        switch (fmt) 
        {
        case TextureFmt::R8: return DXGI_FORMAT_R8_UNORM;
        case TextureFmt::RG8: return DXGI_FORMAT_R8G8_UNORM;
        case TextureFmt::RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TextureFmt::RGBA8_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case TextureFmt::R16: return DXGI_FORMAT_R16_UNORM;
        case TextureFmt::RG16: return DXGI_FORMAT_R16G16_UNORM;
        case TextureFmt::RGBA16: return DXGI_FORMAT_R16G16B16A16_UNORM;
        case TextureFmt::R16f: return DXGI_FORMAT_R16_FLOAT;
        case TextureFmt::RG16f: return DXGI_FORMAT_R16G16_FLOAT;
        case TextureFmt::RGBA16f: return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case TextureFmt::R32: return DXGI_FORMAT_R32_UINT;
        case TextureFmt::RG32: return DXGI_FORMAT_R32G32_UINT;
        case TextureFmt::RGB32: return DXGI_FORMAT_R32G32B32_UINT;
        case TextureFmt::RGBA32: return DXGI_FORMAT_R32G32B32A32_UINT;
        case TextureFmt::R32f: return DXGI_FORMAT_R32_FLOAT;
        case TextureFmt::RG32f: return DXGI_FORMAT_R32G32_FLOAT;
        case TextureFmt::RGB32f: return DXGI_FORMAT_R32G32B32_FLOAT;
        case TextureFmt::RGBA32f: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case TextureFmt::D16: return DXGI_FORMAT_R16_UNORM;
        case TextureFmt::D24_S8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case TextureFmt::D32f: return DXGI_FORMAT_R32_TYPELESS;
        case TextureFmt::D32f_S8: return DXGI_FORMAT_R32G8X24_TYPELESS;
        default:
            return DXGI_FORMAT_UNKNOWN;
        }
    }

    DXGI_FORMAT toDXGI_SRVFmt(TextureFmt fmt)
    {
        switch (fmt) 
        {
        case TextureFmt::R8: return DXGI_FORMAT_R8_UNORM;
        case TextureFmt::RG8: return DXGI_FORMAT_R8G8_UNORM;
        case TextureFmt::RGBA8: return DXGI_FORMAT_R8G8B8A8_UNORM;
        case TextureFmt::RGBA8_SRGB: return DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
        case TextureFmt::R16: return DXGI_FORMAT_R16_UNORM;
        case TextureFmt::RG16: return DXGI_FORMAT_R16G16_UNORM;
        case TextureFmt::RGBA16: return DXGI_FORMAT_R16G16B16A16_UNORM;
        case TextureFmt::R16f: return DXGI_FORMAT_R16_FLOAT;
        case TextureFmt::RG16f: return DXGI_FORMAT_R16G16_FLOAT;
        case TextureFmt::RGBA16f: return DXGI_FORMAT_R16G16B16A16_FLOAT;
        case TextureFmt::R32: return DXGI_FORMAT_R32_UINT;
        case TextureFmt::RG32: return DXGI_FORMAT_R32G32_UINT;
        case TextureFmt::RGB32: return DXGI_FORMAT_R32G32B32_UINT;
        case TextureFmt::RGBA32: return DXGI_FORMAT_R32G32B32A32_UINT;
        case TextureFmt::R32f: return DXGI_FORMAT_R32_FLOAT;
        case TextureFmt::RG32f: return DXGI_FORMAT_R32G32_FLOAT;
        case TextureFmt::RGB32f: return DXGI_FORMAT_R32G32B32_FLOAT;
        case TextureFmt::RGBA32f: return DXGI_FORMAT_R32G32B32A32_FLOAT;
        case TextureFmt::D16: return DXGI_FORMAT_R16_UNORM;
        case TextureFmt::D24_S8: return DXGI_FORMAT_D24_UNORM_S8_UINT;
        case TextureFmt::D32f: return DXGI_FORMAT_R32_FLOAT;
        case TextureFmt::D32f_S8: return DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
        default:
            return DXGI_FORMAT_UNKNOWN;
        }
    }

    bool isShaderRes(TextureFmt fmt)
    {
        return fmt != TextureFmt::D24_S8;
    }

    bool isRenderTarget(TextureFmt fmt)
    {
        return
            (fmt == TextureFmt::R8) ||
            (fmt == TextureFmt::RG8) ||
            (fmt == TextureFmt::RGBA8) ||
            (fmt == TextureFmt::RGBA8_SRGB) ||
            (fmt == TextureFmt::R16f) ||
            (fmt == TextureFmt::RG16f) ||
            (fmt == TextureFmt::RGBA16f) ||
            (fmt == TextureFmt::R32f) ||
            (fmt == TextureFmt::RG32f) ||
            (fmt == TextureFmt::RGB32f) ||
            (fmt == TextureFmt::RGBA32f);
    }

    bool isDepthTarget(TextureFmt fmt)
    {
        return
            (fmt == TextureFmt::D16) ||
            (fmt == TextureFmt::D24_S8) ||
            (fmt == TextureFmt::D32f) ||
            (fmt == TextureFmt::D32f_S8);
    }

    bool isUAV(TextureFmt fmt)
    {
        return
            (fmt == TextureFmt::R32) ||
            (fmt == TextureFmt::RG32) ||
            (fmt == TextureFmt::RGB32) ||
            (fmt == TextureFmt::RGBA32) ||
            (fmt == TextureFmt::R32f) ||
            (fmt == TextureFmt::RG32f) ||
            (fmt == TextureFmt::RGB32f) ||
            (fmt == TextureFmt::RGBA32f);
    }

    int calcMipLevelsCount(int w, int h)
    {
        assert(w >= 0);
        assert(h >= 0);

        int min_size = glm::min(w, h);
        int max_mip = 0;
        while (min_size > 0) {
            min_size >>= 1;
            max_mip++;
        }
        return max_mip;
    }
    
    int calcMipLevelsCount(int w, int h, int z) 
    {
        assert(w >= 0);
        assert(h >= 0);

        int min_size = glm::min(glm::min(w, h), z);
        int max_mip = 0;
        while (min_size > 0) {
            min_size >>= 1;
            max_mip++;
        }
        return max_mip;
    }

    D3D11_BLEND toDX(Blend b) {
        switch (b)
        {
        case Blend::Zero: return D3D11_BLEND_ZERO;
        case Blend::One: return D3D11_BLEND_ONE;
        case Blend::Src_Alpha: return D3D11_BLEND_SRC_ALPHA;
        case Blend::Inv_Src_Alpha: return D3D11_BLEND_INV_SRC_ALPHA;
        case Blend::Dst_Alpha: return D3D11_BLEND_DEST_ALPHA;
        case Blend::Inv_Dst_Alpha: return D3D11_BLEND_INV_DEST_ALPHA;
        case Blend::Src_Color: return D3D11_BLEND_SRC_COLOR;
        case Blend::Inv_Src_Color: return D3D11_BLEND_INV_SRC_COLOR;
        case Blend::Dst_Color: return D3D11_BLEND_DEST_COLOR;
        case Blend::Inv_Dst_Color: return D3D11_BLEND_INV_DEST_COLOR;
        default:
            return D3D11_BLEND_ZERO;
        }
    }
    
    D3D11_BLEND_OP toDX(BlendFunc b)
    {
        switch (b)
        {
        case BlendFunc::Add: return D3D11_BLEND_OP_ADD;
        case BlendFunc::Sub: return D3D11_BLEND_OP_SUBTRACT;
        case BlendFunc::Rev_Sub: return D3D11_BLEND_OP_REV_SUBTRACT;
        case BlendFunc::Min: return D3D11_BLEND_OP_MIN;
        case BlendFunc::Max: return D3D11_BLEND_OP_MAX;
        default:
            return D3D11_BLEND_OP_ADD;
        }
    }

    D3D11_COMPARISON_FUNC toDX(Compare cmp)
    {
        D3D11_COMPARISON_FUNC res = D3D11_COMPARISON_NEVER;
        
        switch (cmp)
        {
        case Compare::Never: res = D3D11_COMPARISON_NEVER; break;
        case Compare::Less: res = D3D11_COMPARISON_LESS; break;
        case Compare::Equal: res = D3D11_COMPARISON_EQUAL; break;
        case Compare::Less_Equal: res = D3D11_COMPARISON_LESS_EQUAL; break;
        case Compare::Greater: res = D3D11_COMPARISON_GREATER; break;
        case Compare::Not_Equal: res = D3D11_COMPARISON_NOT_EQUAL; break;
        case Compare::Greater_Equal: res = D3D11_COMPARISON_GREATER_EQUAL; break;
        case Compare::Always: res = D3D11_COMPARISON_ALWAYS; break;
        }

        return res;
    }

    D3D11_TEXTURE_ADDRESS_MODE toDX(TexWrap wrap)
    {
        D3D11_TEXTURE_ADDRESS_MODE res = D3D11_TEXTURE_ADDRESS_WRAP;
        switch (wrap)
        {
            case TexWrap::Repeat: res = D3D11_TEXTURE_ADDRESS_WRAP; break;
            case TexWrap::Mirror: res = D3D11_TEXTURE_ADDRESS_MIRROR; break;
            case TexWrap::Clamp: res = D3D11_TEXTURE_ADDRESS_CLAMP; break;
            case TexWrap::ClampBorder: res = D3D11_TEXTURE_ADDRESS_BORDER; break;
        }
        return res;
    }*/
}
