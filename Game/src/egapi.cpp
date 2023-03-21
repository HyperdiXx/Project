#include "egapi.h"
#include <cassert>
#include <fstream>
#include <sstream>
#include <d3dcompiler.h>
#include "edx11api.h"

#include <filesystem>

namespace EProject
{
    int getPixelsSize(TextureFmt fmt)
    {
        switch (fmt) {
        case TextureFmt::R8: return 1;
        case TextureFmt::RG8: return 2;
        case TextureFmt::RGBA8: return 4;
        case TextureFmt::RGBA8_SRGB: return 4;
        case TextureFmt::R16: return 2;
        case TextureFmt::RG16: return 4;
        case TextureFmt::RGBA16: return 8;
        case TextureFmt::R16f: return 2;
        case TextureFmt::RG16f: return 4;
        case TextureFmt::RGBA16f: return 8;
        case TextureFmt::R32: return 4;
        case TextureFmt::RG32: return 8;
        case TextureFmt::RGB32: return 12;
        case TextureFmt::RGBA32: return 16;
        case TextureFmt::R32f: return 4;
        case TextureFmt::RG32f: return 8;
        case TextureFmt::RGB32f: return 12;
        case TextureFmt::RGBA32f: return 16;
        case TextureFmt::D16: return 2;
        case TextureFmt::D24_S8: return 4;
        case TextureFmt::D32f: return 4;
        case TextureFmt::D32f_S8: return 8;
        default:
            return 0;
        }
    }

    void States::setDefaultStates()
    {
        m_r_desc.FillMode = D3D11_FILL_SOLID;
        m_r_desc.CullMode = D3D11_CULL_BACK;
        m_r_desc.FrontCounterClockwise = true;
        m_r_desc.DepthBias = 0;
        m_r_desc.DepthBiasClamp = 0;
        m_r_desc.SlopeScaledDepthBias = 0;
        m_r_desc.DepthClipEnable = true;
        m_r_desc.ScissorEnable = false;
        m_r_desc.MultisampleEnable = false;
        m_r_desc.AntialiasedLineEnable = false;
        m_r_state = nullptr;

        m_stencil_ref = 0xff;
        m_d_desc.DepthEnable = false;
        m_d_desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        m_d_desc.DepthFunc = D3D11_COMPARISON_LESS;
        m_d_desc.StencilEnable = false;
        m_d_desc.StencilReadMask = 0xff;
        m_d_desc.StencilWriteMask = 0xff;
        m_d_desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        m_d_desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        m_d_desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        m_d_desc.FrontFace.StencilFunc = D3D11_COMPARISON_NEVER;
        m_d_desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        m_d_desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        m_d_desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        m_d_desc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
        m_d_state = nullptr;

        m_b_desc.AlphaToCoverageEnable = false;
        m_b_desc.IndependentBlendEnable = true;
        
        for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
        {
            m_b_desc.RenderTarget[i].BlendEnable = false;
            m_b_desc.RenderTarget[i].SrcBlend = D3D11_BLEND_ONE;
            m_b_desc.RenderTarget[i].DestBlend = D3D11_BLEND_ONE;
            m_b_desc.RenderTarget[i].BlendOp = D3D11_BLEND_OP_ADD;
            m_b_desc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
            m_b_desc.RenderTarget[i].SrcBlendAlpha = D3D11_BLEND_ONE;
            m_b_desc.RenderTarget[i].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            m_b_desc.RenderTarget[i].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        }

        m_b_state = nullptr;
    }

    void States::push()
    {
        m_states.emplace_back(this);
        ComPtr<ID3D11RasterizerState> m_r_state = nullptr;
        ComPtr<ID3D11DepthStencilState> m_d_state = nullptr;
        ComPtr<ID3D11BlendState> m_b_state = nullptr;
    }

    void States::pop()
    {
        const auto& last = m_states.back();

        m_r_desc = last.m_r_desc;
        m_d_desc = last.m_d_desc;
        m_b_desc = last.m_b_desc;
        m_r_state = last.m_r_state;
        m_d_state = last.m_d_state;
        m_b_state = last.m_b_state;
        m_stencil_ref = last.m_stencil_ref;
        
        if (m_r_state)
            m_deviceContext->RSSetState(m_r_state.Get());
        if (m_d_state)
            m_deviceContext->OMSetDepthStencilState(m_d_state.Get(), m_stencil_ref);
        if (m_b_state)
            m_deviceContext->OMSetBlendState(m_b_state.Get(), nullptr, 0xffffffff);
    }

    void States::setWireframe(bool wire)
    {
        if ((m_r_desc.FillMode == D3D11_FILL_WIREFRAME) != wire)
        {
            m_r_desc.FillMode = wire ? D3D11_FILL_WIREFRAME : D3D11_FILL_SOLID;
            m_r_state = nullptr;
        }
    }

    void States::setCull(CullMode cm)
    {
        D3D11_CULL_MODE dx_cm = D3D11_CULL_NONE;
        switch (cm) 
        {
        case CullMode::None: dx_cm = D3D11_CULL_NONE; break;
        case CullMode::Back: dx_cm = D3D11_CULL_BACK; break;
        case CullMode::Front: dx_cm = D3D11_CULL_FRONT; break;
        }
        
        if (m_r_desc.CullMode != dx_cm)
        {
            m_r_desc.CullMode = dx_cm;
            m_r_state = nullptr;
        }
    }

    void States::setDepthEnable(bool enable)
    {
        if (static_cast<bool>(m_d_desc.DepthEnable) != enable) 
        {
            m_d_desc.DepthEnable = enable;
            m_d_state = nullptr;
        }
    }

    void States::setDepthWrite(bool enable)
    {
        if ((m_d_desc.DepthWriteMask == D3D11_DEPTH_WRITE_MASK_ALL) != enable)
        {
            m_d_desc.DepthWriteMask = enable ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
            m_d_state = nullptr;
        }
    }

    void States::setDepthFunc(Compare cmp)
    {
        D3D11_COMPARISON_FUNC dx_cmp = toDX(cmp);
        if (m_d_desc.DepthFunc != dx_cmp)
        {
            m_d_desc.DepthFunc = dx_cmp;
            m_d_state = nullptr;
        }
    }

    void States::setBlend(bool enable, Blend src, Blend dst, int rt_index, BlendFunc bf)
    {
        setBlendSeparateAlpha(enable, src, dst, bf, src, dst, bf, rt_index);
    }

    void States::setBlendSeparateAlpha(bool enable, Blend src_color, Blend dst_color, BlendFunc bf_color, Blend src_alpha, Blend dst_alpha, BlendFunc bf_alpha, int rt_index)
    {
        if (m_b_desc.IndependentBlendEnable != (rt_index >= 0))
        {
            m_b_desc.IndependentBlendEnable = (rt_index >= 0);
            m_b_state = nullptr;
        }

        D3D11_BLEND dx_src_blend = enable ? toDX(src_color) : D3D11_BLEND_ONE;
        D3D11_BLEND dx_dst_blend = enable ? toDX(dst_color) : D3D11_BLEND_ONE;
        D3D11_BLEND_OP dx_blend_op = enable ? toDX(bf_color) : D3D11_BLEND_OP_ADD;
        D3D11_BLEND dx_src_alpha_blend = enable ? toDX(src_alpha) : D3D11_BLEND_ONE;
        D3D11_BLEND dx_dst_alpha_blend = enable ? toDX(dst_alpha) : D3D11_BLEND_ONE;
        D3D11_BLEND_OP dx_blend_alpha_op = enable ? toDX(bf_alpha) : D3D11_BLEND_OP_ADD;

        int n = (rt_index < 0) ? 0 : rt_index;
        
        if (static_cast<bool>(m_b_desc.RenderTarget[n].BlendEnable) != enable)
        {
            m_b_desc.RenderTarget[n].BlendEnable = enable;
            m_b_state = nullptr;
        }
        
        if (m_b_desc.RenderTarget[n].SrcBlend != dx_src_blend)
        {
            m_b_desc.RenderTarget[n].SrcBlend = dx_src_blend;
            m_b_state = nullptr;
        }
        
        if (m_b_desc.RenderTarget[n].DestBlend != dx_dst_blend)
        {
            m_b_desc.RenderTarget[n].DestBlend = dx_dst_blend;
            m_b_state = nullptr;
        }
        
        if (m_b_desc.RenderTarget[n].BlendOp != dx_blend_op)
        {
            m_b_desc.RenderTarget[n].BlendOp = dx_blend_op;
            m_b_state = nullptr;
        }
        
        if (m_b_desc.RenderTarget[n].SrcBlendAlpha != dx_src_alpha_blend)
        {
            m_b_desc.RenderTarget[n].SrcBlendAlpha = dx_src_alpha_blend;
            m_b_state = nullptr;
        }
        
        if (m_b_desc.RenderTarget[n].DestBlendAlpha != dx_dst_alpha_blend)
        {
            m_b_desc.RenderTarget[n].DestBlendAlpha = dx_dst_alpha_blend;
            m_b_state = nullptr;
        }
        
        if (m_b_desc.RenderTarget[n].BlendOpAlpha != dx_blend_alpha_op)
        {
            m_b_desc.RenderTarget[n].BlendOpAlpha = dx_blend_alpha_op;
            m_b_state = nullptr;
        }

        if (rt_index < 0)
        {
            for (int i = 1; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
                m_b_desc.RenderTarget[i] = m_b_desc.RenderTarget[0];
        }
    }

    void States::setColorWrite(bool enable, int rt_index)
    {
        if (m_b_desc.IndependentBlendEnable != (rt_index >= 0))
        {
            m_b_desc.IndependentBlendEnable = (rt_index >= 0);
            m_b_state = nullptr;
        }
        
        int n = (rt_index < 0) ? 0 : rt_index;
        if (static_cast<bool>(m_b_desc.RenderTarget[n].RenderTargetWriteMask) != enable)
        {
            m_b_desc.RenderTarget[n].RenderTargetWriteMask = enable ? D3D11_COLOR_WRITE_ENABLE_ALL : 0;
            m_b_state = nullptr;
        }
        
        if (rt_index < 0) 
        {
            for (int i = 1; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
                m_b_desc.RenderTarget[i] = m_b_desc.RenderTarget[0];
        }
    }

    void States::validateStates()
    {
        if (!m_r_state)
        {
            getD3DErr(m_device->CreateRasterizerState(&m_r_desc, &m_r_state));
            m_deviceContext->RSSetState(m_r_state.Get());
        }
        
        if (!m_d_state)
        {
            getD3DErr(m_device->CreateDepthStencilState(&m_d_desc, &m_d_state));
            m_deviceContext->OMSetDepthStencilState(m_d_state.Get(), m_stencil_ref);
        }
        
        if (!m_b_state)
        {
            getD3DErr(m_device->CreateBlendState(&m_b_desc, &m_b_state));
            m_deviceContext->OMSetBlendState(m_b_state.Get(), nullptr, 0xffffffff);
        }
    }

    States::States(ID3D11Device* device, ID3D11DeviceContext* device_context)
    {
        m_device = device;
        m_deviceContext = device_context;
        setDefaultStates();
    }

    GDevice::GDevice(HWND wnd, bool sRGB)
    {
        m_isSrgb = sRGB;

        m_hwnd = wnd;
        RECT rct;
        GetClientRect(wnd, &rct);

        m_lastWndSize = glm::ivec2(rct.right - rct.left, rct.bottom - rct.top);

        DXGI_SWAP_CHAIN_DESC sd;
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = 1;
        sd.BufferDesc.Width = m_lastWndSize.x;
        sd.BufferDesc.Height = m_lastWndSize.y;
        sd.BufferDesc.Format = m_isSrgb ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = wnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;

        getD3DErr(D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            0,
            D3D11_CREATE_DEVICE_SINGLETHREADED,// | D3D11_CREATE_DEVICE_DEBUG,
            nullptr,
            0,
            D3D11_SDK_VERSION,
            &sd,
            &m_swapChain,
            &m_dev,
            nullptr,
            &m_context)
        );

        getD3DErr(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &m_backBuffer));
        getD3DErr(m_dev->CreateRenderTargetView(m_backBuffer.Get(), nullptr, &m_RTView));

        m_states = std::make_unique<States>(m_dev.Get(), m_context.Get());

        D3D11_TEXTURE2D_DESC descDepth;
        ZeroMemory(&descDepth, sizeof(descDepth));
        descDepth.Width = m_lastWndSize.x;
        descDepth.Height = m_lastWndSize.y;
        descDepth.MipLevels = 1;
        descDepth.ArraySize = 1;
        descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
        descDepth.SampleDesc.Count = 1;
        descDepth.SampleDesc.Quality = 0;
        descDepth.Usage = D3D11_USAGE_DEFAULT;
        descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
        descDepth.CPUAccessFlags = 0;
        descDepth.MiscFlags = 0;

        getD3DErr(m_dev->CreateTexture2D(&descDepth, nullptr, &m_depthStencil));

        D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
        ZeroMemory(&descDSV, sizeof(descDSV));
        descDSV.Format = descDepth.Format;
        descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
        descDSV.Texture2D.MipSlice = 0;
        getD3DErr(m_dev->CreateDepthStencilView(m_depthStencil.Get(), 0, &m_depthStencilView));
    }

    GDevice::~GDevice()
    {
    }

    FrameBufferPtr GDevice::setFrameBuffer(const FrameBufferPtr& fbo, bool update_viewport)
    {
        FrameBufferPtr currentFbo = m_activeFbo.lock();
        std::weak_ptr<Framebuffer> m_new_fbo = fbo;

        if (m_activeFboPtr != fbo.get())
        {
            m_activeFbo = m_new_fbo;
            m_activeFboPtr = fbo.get();

            if (m_activeFboPtr)
            {
                m_activeFboPtr->prepareSlots();
                m_context->OMSetRenderTargetsAndUnorderedAccessViews(
                    UINT(m_activeFboPtr->m_rtv_count),
                    m_activeFboPtr->m_colors_to_bind.data(),
                    m_activeFboPtr->m_depth_view.Get(),
                    UINT(m_activeFboPtr->m_rtv_count),
                    UINT(glm::clamp<int>(int(m_activeFboPtr->m_uav_to_bind.size()) - m_activeFboPtr->m_rtv_count, 0, 7)),
                    m_activeFboPtr->m_uav_to_bind.data(),
                    m_activeFboPtr->m_uav_initial_counts.data()
                );
            }
            else 
            {
                setDefaultFramebuffer();
            }
        }
        if (update_viewport)
        {
            setViewport(currentFrameBufferSize());
        }


        return currentFbo;
    }

    ID3D11SamplerState* GDevice::obtainSampler(const Sampler& s)
    {
        auto it = m_samplers.find(s);
        if (it == m_samplers.end()) 
        {
            D3D11_SAMPLER_DESC desc = {};

            bool iscomparison = s.comparison != Compare::Never;

            if (s.anisotropy > 1)
            {
                desc.Filter = iscomparison ? D3D11_FILTER_COMPARISON_ANISOTROPIC : D3D11_FILTER_ANISOTROPIC;
            }
            else
            {
                if (iscomparison)
                {
                    if (s.filter == TexFilter::Linear)
                    {
                        if (s.mipfilter == TexFilter::Linear)
                        {
                            desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
                        }
                        else 
                        {
                            desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
                        }
                    }
                    else
                    {
                        if (s.mipfilter == TexFilter::Linear)
                        {
                            desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
                        }
                        else 
                        {
                            desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
                        }
                    }
                }
                else 
                {
                    if (s.filter == TexFilter::Linear)
                    {
                        if (s.mipfilter == TexFilter::Linear)
                        {
                            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                        }
                        else 
                        {
                            desc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
                        }
                    }
                    else
                    {
                        if (s.mipfilter == TexFilter::Linear)
                        {
                            desc.Filter = D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
                        }
                        else 
                        {
                            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
                        }
                    }
                }
            }

            desc.AddressU = toDX(s.wrap_x);
            desc.AddressV = toDX(s.wrap_x);
            desc.AddressW = toDX(s.wrap_x);
            desc.MipLODBias = 0;
            desc.MaxAnisotropy = s.anisotropy;
            desc.ComparisonFunc = toDX(s.comparison);
            desc.BorderColor[0] = s.border.x;
            desc.BorderColor[1] = s.border.y;
            desc.BorderColor[2] = s.border.z;
            desc.BorderColor[3] = s.border.w;
            desc.MinLOD = 0;
            desc.MaxLOD = s.mipfilter == TexFilter::None ? 0 : D3D11_FLOAT32_MAX;

            ComPtr<ID3D11SamplerState> sampler;
            getD3DErr(m_dev->CreateSamplerState(&desc, &sampler));
            m_samplers.insert({ s, sampler });
            
            return sampler.Get();
        }
        
        return it->second.Get();
    }

    glm::ivec2 GDevice::currentFrameBufferSize() const
    {
        return m_lastWndSize;
    }

    ID3D11Device* GDevice::getDX11Device() const
    {
        return m_dev.Get();
    }

    ID3D11DeviceContext* GDevice::getDX11DeviceContext() const
    {
        return m_context.Get();
    }

    States* GDevice::getStates()
    {
        return m_states.get();
    }

    FrameBufferPtr GDevice::getActiveFrameBuffer() const
    {
        return m_activeFbo.lock();
    }

    ShaderProgram* GDevice::getActiveProgram()
    {
        return m_activeProgram;
    }

    FrameBufferPtr GDevice::createFrameBuffer()
    {
        return std::make_shared<Framebuffer>(shared_from_this());
    }

    GPUTexture2DPtr GDevice::createTexture2D()
    {
        return std::make_shared<GPUTexture2D>(shared_from_this());
    }

    IndexBufferPtr GDevice::createIndexBuffer()
    {
        return std::make_shared<IndexBuffer>(shared_from_this());
    }

    VertexBufferPtr GDevice::createVertexBuffer()
    {
        return std::make_shared<VertexBuffer>(shared_from_this());
    }

    ShaderProgramPtr GDevice::createShaderProgram()
    {
        return std::make_shared<ShaderProgram>(shared_from_this());
    }

    UniformBufferPtr GDevice::createUniformBuffer()
    {
        return std::make_shared<UniformBuffer>(shared_from_this());
    }

    StructuredBufferPtr GDevice::createStructuredBuffer()
    {
        return std::make_shared<StructuredBuffer>(shared_from_this());
    }

    void GDevice::beginFrame()
    {
        RECT rct;
        GetClientRect(m_hwnd, &rct);
        glm::ivec2 new_wnd_size = glm::ivec2(rct.right - rct.left, rct.bottom - rct.top);

        if (m_lastWndSize != new_wnd_size) 
        {
            m_lastWndSize = new_wnd_size;

            ID3D11RenderTargetView* tmpRT = nullptr;
            ID3D11DepthStencilView* tmpDS = nullptr;
            m_context->OMSetRenderTargetsAndUnorderedAccessViews(1, &tmpRT, tmpDS, 0, 0, nullptr, nullptr);
            m_RTView = nullptr;
            m_backBuffer = nullptr;
            getD3DErr(m_swapChain->ResizeBuffers(1, m_lastWndSize.x, m_lastWndSize.y, m_isSrgb ? DXGI_FORMAT_R8G8B8A8_UNORM_SRGB : DXGI_FORMAT_R8G8B8A8_UNORM, 0));
            getD3DErr(m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), &m_backBuffer));
            getD3DErr(m_dev->CreateRenderTargetView(m_backBuffer.Get(), nullptr, &m_RTView));
            getD3DErr(m_dev->CreateDepthStencilView(m_depthStencil.Get(), nullptr, &m_depthStencilView));
        }

        setDefaultFramebuffer();
        setViewport(m_lastWndSize);
        
        const float clearColor[4] = { 0.75f, 0.75f, 0.75f, 1.0f };
        m_context->ClearRenderTargetView(m_RTView.Get(), clearColor);    

        if (auto* dsView = m_depthStencilView.Get())
        {
            m_context->ClearDepthStencilView(dsView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        }
    }

    void GDevice::endFrame()
    {
        m_swapChain->Present(1, 0);
    }

    bool GDevice::isSRGB() const
    {
        return m_isSrgb;
    }

    HWND GDevice::getWindow() const
    {
        return m_hwnd;
    }

    void GDevice::setDefaultFramebuffer()
    {
        ID3D11RenderTargetView* defRT = m_RTView.Get();
        ID3D11DepthStencilView* defDS = m_depthStencilView.Get();

        getDX11DeviceContext()->OMSetRenderTargetsAndUnorderedAccessViews(1, &defRT, defDS, 0, 0, nullptr, nullptr);
    }

    void GDevice::setViewport(const glm::vec2& size)
    {
        D3D11_VIEWPORT vp = {};
        vp.TopLeftX = 0;
        vp.TopLeftY = 0;
        vp.Width = size.x;
        vp.Height = size.y;
        vp.MinDepth = 0.0;
        vp.MaxDepth = 1.0;
        getDX11DeviceContext()->RSSetViewports(1, &vp);
    }

    ShaderProgram::ShaderProgram(const GDevicePtr& device) : DeviceHolder(device)
    {
        m_globals_dirty = false;
        m_selectedInstanceStep = 0;
    }

    ShaderProgram::~ShaderProgram()
    {
        if (m_device->m_activeProgram == this) m_device->m_activeProgram = nullptr;
    }

    bool ShaderProgram::compileFromFile(const ShaderInput& input)
    {
        if (!std::filesystem::exists(input.filePath))
        {
            return false;
        }

        ID3DBlob* compiledShader = nullptr;
        ID3DBlob* errorMessages = nullptr;
        
        D3D_SHADER_MACRO defines[] =
        {
            "HLSL5", "1",
            "DISABLE_WAVE_INTRINSICS", "1",
            NULL, NULL
        };

        int flags = 0;

        const char* target = input.target.c_str();
        const char* entryPoint = input.entyPoint.c_str();
        
        HRESULT hr = (D3DCompileFromFile(input.filePath.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, target,
            flags, 0, &compiledShader, &errorMessages));

        if (FAILED(hr) && errorMessages)
        {
            const char* errorMsg = (const char*)errorMessages->GetBufferPointer();
            
            //MessageBox(nullptr, errorMsg, L"Shader Compilation Error", MB_RETRYCANCEL);
        }

        m_shaderData[int(input.type)] = compiledShader;

        //m_blob = compiledShader;

        //else
        //{
        //    // Compress the shader
        //    D3D_SHADER_DATA shaderData;
        //    shaderData.pBytecode = compiledShader->GetBufferPointer();
        //    shaderData.BytecodeLength = compiledShader->GetBufferSize();
        //    ID3DBlobPtr compressedShader;
        //    DXCall(D3DCompressShaders(1, &shaderData, D3D_COMPRESS_SHADER_KEEP_ALL_PARTS, &compressedShader));

        //    // Create the cache directory if it doesn't exist
        //    if (DirectoryExists(baseCacheDir.c_str()) == false)
        //        Win32Call(CreateDirectory(baseCacheDir.c_str(), nullptr));

        //    if (DirectoryExists(cacheDir.c_str()) == false)
        //        Win32Call(CreateDirectory(cacheDir.c_str(), nullptr));

        //    File cacheFile(cacheName.c_str(), File::OpenWrite);

        //    // Write the compiled shader to disk
        //    uint64 shaderSize = compressedShader->GetBufferSize();
        //    cacheFile.Write(shaderSize, compressedShader->GetBufferPointer());

        //    return true;
        //}

        return true;
    }

    bool ShaderProgram::create()
    {        
        for (int i = 0; i < 6; ++i)
        {
            auto blob = m_shaderData[i];
            if (!blob)
            {
                continue;
            }

            //std::vector<char>& raw_data = m_shaderCode[int(st)];
            ShaderType st = cShaders[i];

            void* blobPtr = (ID3D10Blob**)blob->GetBufferPointer();
            size_t blobSize = blob->GetBufferSize();

            std::string shaderCode = {};
            shaderCode.append((char*)(blob->GetBufferPointer()), blob->GetBufferSize());
            
            auto& elem = m_shaderCode[i];
            elem = std::move(shaderCode);

            ID3D11DeviceChild** tmp = &m_shaders[int(st)];

            switch (cShaders[i])
            {
                case ShaderType::Vertex:
                {
                    getD3DErr(getDevice()->getDX11Device()->CreateVertexShader(blobPtr, blobSize, nullptr, (ID3D11VertexShader**)tmp));
                } break;
                case ShaderType::Hull:
                {
                    getD3DErr(getDevice()->getDX11Device()->CreateHullShader(blobPtr, blobSize, nullptr, (ID3D11HullShader**)tmp));
                } break; 
                case ShaderType::Domain:
                {
                    getD3DErr(getDevice()->getDX11Device()->CreateDomainShader(blobPtr, blobSize, nullptr, (ID3D11DomainShader**)tmp));
                } break;
                case ShaderType::Geometry:
                {
                    getD3DErr(getDevice()->getDX11Device()->CreateGeometryShader(blobPtr, blobSize, nullptr, (ID3D11GeometryShader**)tmp));
                } break;
                case ShaderType::Pixel:
                {
                    getD3DErr(getDevice()->getDX11Device()->CreatePixelShader(blobPtr, blobSize, nullptr, (ID3D11PixelShader**)tmp));
                } break;
                case ShaderType::Compute:
                {
                    getD3DErr(getDevice()->getDX11Device()->CreateComputeShader(blobPtr, blobSize, nullptr, (ID3D11ComputeShader**)tmp));
                } break;
            }
            autoReflect(elem.data(), int(elem.size()), st);
        }
        
        return true;
    }

    const Layout* autoReflectCB(ID3D11ShaderReflectionConstantBuffer* cb_ref)
    {
        D3D11_SHADER_BUFFER_DESC cb_desc;
        cb_ref->GetDesc(&cb_desc);
        assert(cb_desc.Type == D3D_CT_CBUFFER);

        std::vector<char> raw_data;
        raw_data.resize(cb_desc.Size);
        LayoutSelector* lb = getLayoutSelector();
        for (UINT vidx = 0; vidx < cb_desc.Variables; vidx++)
        {
            ID3D11ShaderReflectionVariable* var_ref;
            var_ref = cb_ref->GetVariableByIndex(vidx);
            D3D11_SHADER_VARIABLE_DESC var_desc;
            var_ref->GetDesc(&var_desc);

            ID3D11ShaderReflectionType* type_ref = var_ref->GetType();
            D3D11_SHADER_TYPE_DESC type_desc;
            type_ref->GetDesc(&type_desc);

            LayoutType lt = {};
            switch (type_desc.Type)
            {
            case D3D_SVT_INT: lt = LayoutType::UInt; break;
            case D3D_SVT_UINT: lt = LayoutType::UInt; break;
            case D3D_SVT_FLOAT: lt = LayoutType::Float; break;
            default:
                throw std::runtime_error("unsupported layout type");
            }

            int arr_size = type_desc.Elements == 0 ? 1 : type_desc.Elements;
            lb->add(var_desc.Name, lt, type_desc.Rows * type_desc.Columns, false, var_desc.StartOffset, arr_size);

            if (var_desc.DefaultValue)
            {
                memcpy(&raw_data[var_desc.StartOffset], var_desc.DefaultValue, var_desc.Size);
            }
        }
        return lb->end(cb_desc.Size);
    }

    void ShaderProgram::autoReflect(const void* data, int data_size, ShaderType st)
    {
        ComPtr<ID3D11ShaderReflection> ref;
        D3DReflect(data, data_size, __uuidof(ID3D11ShaderReflection), &ref);
        D3D11_SHADER_DESC shader_desc = {};
        auto hr = ref->GetDesc(&shader_desc);

        for (UINT i = 0; i < shader_desc.BoundResources; i++)
        {
            D3D11_SHADER_INPUT_BIND_DESC res_desc;
            ref->GetResourceBindingDesc(i, &res_desc);

            SlotKind kind;
            switch (res_desc.Type)
            {
            case D3D_SIT_CBUFFER: kind = SlotKind::Uniform; break;
            case D3D_SIT_TEXTURE: kind = SlotKind::Texture; break;
            case D3D_SIT_STRUCTURED: kind = SlotKind::Buffer; break;
            case D3D_SIT_SAMPLER: kind = SlotKind::Sampler; break;
            default:
                throw std::runtime_error("Unsupported resource type!");
            }
            
            const Layout* l = res_desc.Type == D3D_SIT_CBUFFER ? autoReflectCB(ref->GetConstantBufferByName(res_desc.Name)) : nullptr;
            ShaderSlot& slot = m_slots[obtainSlotIdx(kind, std::string(res_desc.Name), l)];
            slot.bindPoints[int(st)] = res_desc.BindPoint;

            if (slot.name == "Globals") 
            {
                m_ub[int(st)] = std::make_shared<UniformBuffer>(m_device);
                m_ub[int(st)]->setState(slot.layout, 1);
                slot.buffer = m_ub[int(st)]->getHandle();
            }
        }
    }

    void ShaderProgram::activateProgram()
    {
        if (m_globals_dirty)
        {
            for (int i = 0; i < 6; i++)
            {
                if (m_ub[i])
                {
                    m_ub[i]->validateDynamicData();
                }
            }
        }

        if (!isProgramActive()) 
        {
            m_device->m_activeProgram = this;
            
            for (const auto& slot : m_slots)
            {
                slot.select(m_device->getDX11DeviceContext());
            }
                        
            selectInputBuffers();

            ID3D11DeviceChild* tmp;
            tmp = m_shaders[int(ShaderType::Vertex)] ? m_shaders[int(ShaderType::Vertex)].Get() : nullptr;
            m_device->getDX11DeviceContext()->VSSetShader((ID3D11VertexShader*)tmp, nullptr, 0);
            tmp = m_shaders[int(ShaderType::Hull)] ? m_shaders[int(ShaderType::Hull)].Get() : nullptr;
            m_device->getDX11DeviceContext()->HSSetShader((ID3D11HullShader*)tmp, nullptr, 0);
            tmp = m_shaders[int(ShaderType::Domain)] ? m_shaders[int(ShaderType::Domain)].Get() : nullptr;
            m_device->getDX11DeviceContext()->DSSetShader((ID3D11DomainShader*)tmp, nullptr, 0);
            tmp = m_shaders[int(ShaderType::Geometry)] ? m_shaders[int(ShaderType::Geometry)].Get() : nullptr;
            m_device->getDX11DeviceContext()->GSSetShader((ID3D11GeometryShader*)tmp, nullptr, 0);
            tmp = m_shaders[int(ShaderType::Pixel)] ? m_shaders[int(ShaderType::Pixel)].Get() : nullptr;
            m_device->getDX11DeviceContext()->PSSetShader((ID3D11PixelShader*)tmp, nullptr, 0);
            tmp = m_shaders[int(ShaderType::Compute)] ? m_shaders[int(ShaderType::Compute)].Get() : nullptr;
            m_device->getDX11DeviceContext()->CSSetShader((ID3D11ComputeShader*)tmp, nullptr, 0);
        }
    }

    void ShaderProgram::setInputBuffers(const VertexBufferPtr& vbo, const IndexBufferPtr& ibo, const VertexBufferPtr& instances, int instanceStepRate)
    {
        m_selectedVBO = vbo;
        m_selectedIBO = ibo;
        m_selectedInstances = instances;
        m_selectedInstanceStep = instanceStepRate;
        if (isProgramActive())
        {
            selectInputBuffers();
        }
    }

    void ShaderProgram::drawIndexed(PrimTopology pt, const DrawIndexedCmd& cmd)
    {
    }

    void ShaderProgram::drawIndexed(PrimTopology pt, const std::vector<DrawIndexedCmd>& cmd_buf)
    {
    }

    void ShaderProgram::drawIndexed(PrimTopology pt, int index_start, int index_count, int instance_count, int base_vertex, int base_instance)
    {
        activateProgram();
        selectTopology(pt);
        
        m_device->getStates()->validateStates();

        if (index_count < 0) index_count = m_selectedIBO->getIndexCount();
        //if (instance_count < 0) instance_count = m_selected_instances ? m_selected_instances->VertexCount() : 0;
        if (instance_count < 0) instance_count = 0;

        if (instance_count)
        {
            m_device->getDX11DeviceContext()->DrawIndexedInstanced(index_count, instance_count, index_start, base_vertex, base_instance);
        }
        else
        {
            m_device->getDX11DeviceContext()->DrawIndexed(index_count, index_start, base_vertex);
        }
    }

    void ShaderProgram::draw(PrimTopology pt, int vert_start, int vert_count, int instance_count, int base_instance)
    {
        activateProgram();
        selectTopology(pt);
        
        m_device->getStates()->validateStates();
        
        //if (instance_count < 0) instance_count = m_selected_instances ? m_selected_instances->VertexCount() : 0;

        if (vert_count < 0) vert_count = m_selectedVBO->getVertexCount();
        if (instance_count < 0) instance_count = 0;

        if (instance_count)
        {
            m_device->getDX11DeviceContext()->DrawInstanced(vert_count, instance_count, vert_start, base_instance);
        }
        else 
        {
            m_device->getDX11DeviceContext()->Draw(vert_count, vert_start);
        }
    }

    void ShaderProgram::setValue(const char* name, float v)
    {
        for (int i = 0; i < 6; i++)
        {
            if (m_ub[i])
            {
                m_ub[i]->setValue(name, v);
            }
        }

        m_globals_dirty = true;
    }

    void ShaderProgram::setValue(const char* name, int i)
    {
        for (int i = 0; i < 6; i++)
        {
            if (m_ub[i])
            {
                m_ub[i]->setValue(name, i);
            }
        }

        m_globals_dirty = true;
    }

    void ShaderProgram::setValue(const char* name, const glm::vec2& v)
    {
        for (int i = 0; i < 6; i++)
        {
            if (m_ub[i])
            {
                m_ub[i]->setValue(name, v);
            }
        }

        m_globals_dirty = true;
    }

    void ShaderProgram::setValue(const char* name, const glm::vec3& v)
    {
        for (int i = 0; i < 6; i++)
        {
            if (m_ub[i])
            {
                m_ub[i]->setValue(name, v);
            }
        }

        m_globals_dirty = true;
    }

    void ShaderProgram::setValue(const char* name, const glm::vec4& v)
    {
        for (int i = 0; i < 6; i++)
        {
            if (m_ub[i])
            {
                m_ub[i]->setValue(name, v);
            }
        }

        m_globals_dirty = true;
    }

    void ShaderProgram::setValue(const char* name, const glm::mat4& m)
    {
        for (int i = 0; i < 6; i++)
        {
            if (m_ub[i])
            {
                m_ub[i]->setValue(name, m);
            }
        }

        m_globals_dirty = true;
    }

    void ShaderProgram::setResource(const char* name, const UniformBufferPtr& ubo)
    {
        int idx = findSlot(name);
        if (idx < 0)
        {
            return;
        }
        
        ShaderSlot& slot = m_slots[idx];
        if ((slot.buffer ? slot.buffer.Get() : nullptr) != (ubo ? ubo->m_handle.Get() : nullptr)) {
            slot.buffer = ubo ? ubo->m_handle : nullptr;
            if (isProgramActive())
            {
                slot.select(m_device->getDX11DeviceContext());
            }
        }
    }

    void ShaderProgram::setResource(const char* name, const StructuredBufferPtr& sbo)
    {
        int idx = findSlot(name);
        if (idx < 0)
        {
            return;
        }
        
        ShaderSlot& slot = m_slots[idx];
        ComPtr<ID3D11ShaderResourceView> srv = sbo ? sbo->getShaderResource() : nullptr;
        
        if ((slot.view ? slot.view.Get() : nullptr) != srv.Get())
        {
            slot.view = std::move(srv);
            if (isProgramActive())
            {
                slot.select(m_device->getDX11DeviceContext());
            }
        }
    }

    void ShaderProgram::setResource(const char* name, const GPUTexture2DPtr& tex, bool as_array, bool as_cubemap)
    {
        int idx = findSlot(name);
        if (idx < 0)
        {
            return;
        }
        
        ShaderSlot& slot = m_slots[idx];
        ComPtr<ID3D11ShaderResourceView> srv = tex ? tex->getShaderResource(as_array, as_cubemap) : nullptr;
        if ((slot.view ? slot.view.Get() : nullptr) != srv.Get())
        {
            slot.view = std::move(srv);
            if (isProgramActive())
            {
                slot.select(m_device->getDX11DeviceContext());
            }
        }
    }

    void ShaderProgram::setResource(const char* name, const Sampler& s)
    {
        int idx = findSlot(name);

        if (idx < 0)
        {
            return;
        }

        ShaderSlot& slot = m_slots[idx];
        ID3D11SamplerState* new_sampler = m_device->obtainSampler(s);

        if (slot.sampler != new_sampler) 
        {
            slot.sampler = new_sampler;
            if (isProgramActive())
            {
                slot.select(m_device->getDX11DeviceContext());
            }
        }
    }

    void ShaderProgram::selectInputBuffers()
    {
        ID3D11Buffer* dxBuf = m_selectedVBO ? m_selectedVBO->m_handle.Get() : nullptr;
        UINT stride = m_selectedVBO ? m_selectedVBO->m_layout->stride : 0;
        UINT offset = 0;
        m_device->getDX11DeviceContext()->IASetVertexBuffers(0, 1, &dxBuf, &stride, &offset);

        /*dxBuf = m_selectedInstances ? m_selectedInstances->m_handle.Get() : nullptr;
        stride = m_selectedInstances ? m_selectedInstances->m_layout->stride : 0;
        offset = 0;
        m_device->getDX11DeviceContext()->IASetVertexBuffers(1, 1, &dxBuf, &stride, &offset);*/

        dxBuf = m_selectedIBO ? m_selectedIBO->m_handle.Get() : nullptr;
        m_device->getDX11DeviceContext()->IASetIndexBuffer(dxBuf, DXGI_FORMAT_R32_UINT, 0);

        const Layout* vl = m_selectedVBO ? m_selectedVBO->getLayout() : nullptr;
        const Layout* il = m_selectedInstances ? m_selectedInstances->getLayout() : nullptr;
        m_device->getDX11DeviceContext()->IASetInputLayout(getLayout(vl, il, m_selectedInstanceStep));
    }

    void ShaderProgram::selectTopology(PrimTopology pt)
    {
        D3D11_PRIMITIVE_TOPOLOGY dx_pt = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;
        switch (pt)
        {
        case PrimTopology::Point: dx_pt = D3D_PRIMITIVE_TOPOLOGY_POINTLIST; break;
        case PrimTopology::Line: dx_pt = D3D_PRIMITIVE_TOPOLOGY_LINELIST; break;
        case PrimTopology::Linestrip: dx_pt = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
        case PrimTopology::Triangle: dx_pt = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
        case PrimTopology::Trianglestrip: dx_pt = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
        }
        
        m_device->getDX11DeviceContext()->IASetPrimitiveTopology(dx_pt);
    }

    bool ShaderProgram::isProgramActive() const
    {
        return  m_device->m_activeProgram == this;
    }

    ID3D11InputLayout* ShaderProgram::getLayout(const Layout* vertices, const Layout* instances, int step_rate)
    {
        for (const auto& l : m_layouts)
        {
            if ((l.vertices == vertices) && (l.instances == instances) && (l.step_rate == step_rate))
            {
                return l.m_dx_layout.Get();
            }
        }
        
        InputLayoutData new_l;
        new_l.vertices = vertices;
        new_l.instances = instances;
        new_l.step_rate = step_rate;        
        new_l.rebuildLayout(m_device->getDX11Device(), m_shaderCode[int(ShaderType::Vertex)]);
        
        m_layouts.push_back(new_l);

        return new_l.m_dx_layout.Get();
    }

    int ShaderProgram::obtainSlotIdx(SlotKind kind, const std::string& name, const Layout* layout)
    {
        if (name != "Globals")
        {
            for (size_t i = 0; i < m_slots.size(); i++) 
            {
                if (m_slots[i].name == name)
                {
                    assert(m_slots[i].kind == kind);
                    assert(m_slots[i].layout == layout);
                    return static_cast<int>(i);
                }
            }
        }
        
        ShaderSlot newSlot;
        newSlot.kind = kind;
        newSlot.name = name;
        newSlot.layout = layout;
        m_slots.push_back(newSlot);
        
        return static_cast<int>(m_slots.size() - 1);
    }

    int ShaderProgram::findSlot(const char* name)
    {
        for (size_t i = 0; i < m_slots.size(); i++)
        {
            if (m_slots[i].name == name)
            {
                return static_cast<int>(i);
            }
        }

        return -1;
    }

    DeviceHolder::DeviceHolder(const GDevicePtr& device) : m_device(device)
    {

    }

    GDevicePtr DeviceHolder::getDevice()
    {
        return m_device;
    }

    VertexBuffer::VertexBuffer(const GDevicePtr& device) : DeviceHolder(device)
    {
        m_vertCount = 0;
        m_layout = nullptr;
    }

    void VertexBuffer::setState(const Layout* layout, int vertex_count, const void* data)
    {
        m_layout = layout;
        m_vertCount = vertex_count;

        if (!m_vertCount)
        {
            m_handle = nullptr;
            return;
        }

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = vertex_count * m_layout->stride;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = m_layout->stride;

        if (data) 
        {
            D3D11_SUBRESOURCE_DATA dxdata = {};
            dxdata.pSysMem = data;
            dxdata.SysMemPitch = desc.ByteWidth;
            dxdata.SysMemSlicePitch = desc.ByteWidth;
            getD3DErr(m_device->getDX11Device()->CreateBuffer(&desc, &dxdata, &m_handle));
        }
        else 
        {
            getD3DErr(m_device->getDX11Device()->CreateBuffer(&desc, nullptr, &m_handle));
        }
    }

    void VertexBuffer::setSubData(int start_vertex, int num_vertices, const void* data)
    {
        assert(m_handle);
        D3D11_BOX box;
        box.left = start_vertex * m_layout->stride;
        box.top = 0;
        box.right = box.left + num_vertices * m_layout->stride;
        box.bottom = 1;
        box.front = 0;
        box.back = 1;
        m_device->getDX11DeviceContext()->UpdateSubresource(m_handle.Get(), 0, &box, data, 0, 0);
    }

    int VertexBuffer::getVertexCount() const
    {
        return m_vertCount;
    }

    const Layout* VertexBuffer::getLayout() const
    {
        return m_layout;
    }

    IndexBuffer::IndexBuffer(const GDevicePtr& device) : DeviceHolder(device)
    {
        m_indCount = 0;
    }

    void IndexBuffer::setState(int ind_count, const void* data)
    {
        m_indCount = ind_count;

        D3D11_BUFFER_DESC desc;
        desc.ByteWidth = m_indCount * 4;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        desc.StructureByteStride = 4;

        if (data)
        {
            D3D11_SUBRESOURCE_DATA dxdata;
            dxdata.pSysMem = data;
            dxdata.SysMemPitch = desc.ByteWidth;
            dxdata.SysMemSlicePitch = desc.ByteWidth;
            getD3DErr(m_device->getDX11Device()->CreateBuffer(&desc, &dxdata, &m_handle));
        }
        else 
        {
            getD3DErr(m_device->getDX11Device()->CreateBuffer(&desc, nullptr, &m_handle));
        }
    }

    void IndexBuffer::setSubData(int start_idx, int num_indices, const void* data)
    {
        assert(m_handle);
        if (num_indices <= 0) return;
        D3D11_BOX box;
        box.left = start_idx * 4;
        box.top = 0;
        box.right = box.left + num_indices * 4;
        box.bottom = 1;
        box.front = 0;
        box.back = 1;
        m_device->getDX11DeviceContext()->UpdateSubresource(m_handle.Get(), 0, &box, data, 0, 0);
    }

    int IndexBuffer::getIndexCount() const
    {
        return m_indCount;
    }

    int LayoutField::getSize() const
    {
        switch (type)
        {
        case LayoutType::Byte: return 1 * num_fields * array_size;
        case LayoutType::Word: return 2 * num_fields * array_size;
        case LayoutType::UInt: return 4 * num_fields * array_size;
        case LayoutType::Float: return 4 * num_fields * array_size;
        default:
            assert(false);
        }
        return 0;
    }

    bool LayoutField::operator==(const LayoutField& l) const
    {
        return  (name == l.name) &&
                (type == l.type) &&
                (num_fields == l.num_fields) &&
                (do_norm == l.do_norm) &&
                (offset == l.offset) &&
                (array_size == l.array_size);
    }

    std::size_t LayoutField::hash() const
    {
        return  std::hash<std::string>()(name) ^
                std::hash<int>()(num_fields) ^
                std::hash<bool>()(do_norm) ^
                std::hash<int>()(offset) ^
                std::hash<int>()(array_size) ^
                std::hash<LayoutType>()(type);
    }


    bool Layout::operator==(const Layout& l) const
    {
        if (fields.size() != l.fields.size()) 
            return false;
        
        if (stride != l.stride) 
            return false;
        
        for (size_t i = 0; i < fields.size(); i++) 
        {
            if (!(fields[i] == l.fields[i])) return false;
        }
       
        return true;
    }

    std::size_t Layout::hash() const
    {
        std::size_t n = std::hash<int>()(stride);

        for (size_t i = 0; i < fields.size(); i++) {
            n ^= fields[i].hash();
        }
        return n;
    }

    LayoutSelectorInstance globalLS;
    LayoutSelector* getLayoutSelector()
    {
        return globalLS.reset();
    }

    ShaderProgram::InputLayoutData::InputLayoutData()
    {
        vertices = nullptr;
        instances = nullptr;
        step_rate = 1;
    }

    DXGI_FORMAT ConvertToDX(const LayoutField& l)
    {
        assert((l.num_fields > 0) && (l.num_fields < 5));
        switch (l.type) {
        case LayoutType::Byte: 
        {
            switch (l.num_fields)
            {
            case 1: return l.do_norm ? DXGI_FORMAT_R8_UNORM : DXGI_FORMAT_R8_UINT;
            case 2: return l.do_norm ? DXGI_FORMAT_R8G8_UNORM : DXGI_FORMAT_R8G8_UINT;
            case 3: return DXGI_FORMAT_UNKNOWN;
            case 4: return l.do_norm ? DXGI_FORMAT_R8G8B8A8_UNORM : DXGI_FORMAT_R8G8B8A8_UINT;
            }
        }
        case LayoutType::Word:
        {
            switch (l.num_fields)
            {
            case 1: return l.do_norm ? DXGI_FORMAT_R16_UNORM : DXGI_FORMAT_R16_UINT;
            case 2: return l.do_norm ? DXGI_FORMAT_R16G16_UNORM : DXGI_FORMAT_R16G16_UINT;
            case 3: return DXGI_FORMAT_UNKNOWN;
            case 4: return l.do_norm ? DXGI_FORMAT_R16G16B16A16_UNORM : DXGI_FORMAT_R16G16B16A16_UINT;
            }
        }
        case LayoutType::UInt:
        {
            switch (l.num_fields)
            {
            case 1: return DXGI_FORMAT_R32_UINT;
            case 2: return DXGI_FORMAT_R32G32_UINT;
            case 3: return DXGI_FORMAT_R32G32B32_UINT;
            case 4: return DXGI_FORMAT_R32G32B32A32_UINT;
            }
        }
        case LayoutType::Float: 
        {
            switch (l.num_fields)
            {
            case 1: return DXGI_FORMAT_R32_FLOAT;
            case 2: return DXGI_FORMAT_R32G32_FLOAT;
            case 3: return DXGI_FORMAT_R32G32B32_FLOAT;
            case 4: return DXGI_FORMAT_R32G32B32A32_FLOAT;
            }
        }
        default:
            throw std::runtime_error("Unsupported format");
        }
    }

    void ShaderProgram::InputLayoutData::rebuildLayout(ID3D11Device* device, const std::string& shader_code)
    {
        std::vector<D3D11_INPUT_ELEMENT_DESC> descs = {};
        if (vertices)
        {
            for (const auto& f : vertices->fields)
            {
                D3D11_INPUT_ELEMENT_DESC d = {};
                d.SemanticName = f.name.c_str();
                d.SemanticIndex = 0;
                d.Format = ConvertToDX(f);
                d.InputSlot = 0;
                d.AlignedByteOffset = f.offset;
                d.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
                d.InstanceDataStepRate = 0;
                descs.push_back(d);
            }
        }
        if (instances) 
        {
            for (const auto& f : instances->fields)
            {
                D3D11_INPUT_ELEMENT_DESC d = {};
                d.SemanticName = f.name.c_str();
                d.SemanticIndex = 0;
                d.Format = ConvertToDX(f);
                d.InputSlot = 1;
                d.AlignedByteOffset = f.offset;
                d.InputSlotClass = D3D11_INPUT_PER_INSTANCE_DATA;
                d.InstanceDataStepRate = step_rate;
                descs.push_back(d);
            }
        }
        if (descs.size())
        {
            getD3DErr(device->CreateInputLayout(descs.data(), UINT(descs.size()), shader_code.data(), shader_code.size(), &m_dx_layout));
        }
        else 
        {
            m_dx_layout = nullptr;
        }
    }

    void ShaderProgram::ShaderSlot::select(ID3D11DeviceContext* dev) const
    {
        ID3D11Buffer* b = buffer ? buffer.Get() : nullptr;
        ID3D11ShaderResourceView* v = view ? view.Get() : nullptr;

        if (bindPoints[int(ShaderType::Vertex)] >= 0)
        {
            switch (kind)
            {
            case SlotKind::Uniform: { dev->VSSetConstantBuffers(bindPoints[int(ShaderType::Vertex)], 1, &b); break; }
            case SlotKind::Texture: { dev->VSSetShaderResources(bindPoints[int(ShaderType::Vertex)], 1, &v); break; }
            case SlotKind::Buffer: { dev->VSSetShaderResources(bindPoints[int(ShaderType::Vertex)], 1, &v); break; }
            case SlotKind::Sampler: { dev->VSSetSamplers(bindPoints[int(ShaderType::Vertex)], 1, &sampler); break; }
            }
        }
        
        if (bindPoints[int(ShaderType::Hull)] >= 0)
        {
            switch (kind) 
            {
            case SlotKind::Uniform: { dev->HSSetConstantBuffers(bindPoints[int(ShaderType::Hull)], 1, &b); break; }
            case SlotKind::Texture: { dev->HSSetShaderResources(bindPoints[int(ShaderType::Hull)], 1, &v); break; }
            case SlotKind::Buffer: { dev->HSSetShaderResources(bindPoints[int(ShaderType::Hull)], 1, &v); break; }
            case SlotKind::Sampler: { dev->HSSetSamplers(bindPoints[int(ShaderType::Hull)], 1, &sampler); break; }
            }
        }
        
        if (bindPoints[int(ShaderType::Domain)] >= 0)
        {
            switch (kind)
            {
            case SlotKind::Uniform: { dev->DSSetConstantBuffers(bindPoints[int(ShaderType::Domain)], 1, &b); break; }
            case SlotKind::Texture: { dev->DSSetShaderResources(bindPoints[int(ShaderType::Domain)], 1, &v); break; }
            case SlotKind::Buffer: { dev->DSSetShaderResources(bindPoints[int(ShaderType::Domain)], 1, &v); break; }
            case SlotKind::Sampler: { dev->DSSetSamplers(bindPoints[int(ShaderType::Domain)], 1, &sampler); break; }
            }
        }
        
        if (bindPoints[int(ShaderType::Geometry)] >= 0)
        {
            switch (kind)
            {
            case SlotKind::Uniform: { dev->GSSetConstantBuffers(bindPoints[int(ShaderType::Geometry)], 1, &b); break; }
            case SlotKind::Texture: { dev->GSSetShaderResources(bindPoints[int(ShaderType::Geometry)], 1, &v); break; }
            case SlotKind::Buffer: { dev->GSSetShaderResources(bindPoints[int(ShaderType::Geometry)], 1, &v); break; }
            case SlotKind::Sampler: { dev->GSSetSamplers(bindPoints[int(ShaderType::Geometry)], 1, &sampler); break; }
            }
        }

        if (bindPoints[int(ShaderType::Pixel)] >= 0)
        {
            switch (kind)
            {
            case SlotKind::Uniform: { dev->PSSetConstantBuffers(bindPoints[int(ShaderType::Pixel)], 1, &b); break; }
            case SlotKind::Texture: { dev->PSSetShaderResources(bindPoints[int(ShaderType::Pixel)], 1, &v); break; }
            case SlotKind::Buffer: { dev->PSSetShaderResources(bindPoints[int(ShaderType::Pixel)], 1, &v); break; }
            case SlotKind::Sampler: { dev->PSSetSamplers(bindPoints[int(ShaderType::Pixel)], 1, &sampler); break; }
            }
        }
        
        if (bindPoints[int(ShaderType::Compute)] >= 0) 
        {
            switch (kind)
            {
            case SlotKind::Uniform: { dev->CSSetConstantBuffers(bindPoints[int(ShaderType::Compute)], 1, &b); break; }
            case SlotKind::Texture: { dev->CSSetShaderResources(bindPoints[int(ShaderType::Compute)], 1, &v); break; }
            case SlotKind::Buffer: { dev->CSSetShaderResources(bindPoints[int(ShaderType::Compute)], 1, &v); break; }
            case SlotKind::Sampler: { dev->CSSetSamplers(bindPoints[int(ShaderType::Compute)], 1, &sampler); break; }
            }
        }
    }

    UniformBuffer::UniformBuffer(const GDevicePtr& device) : DeviceHolder(device)
    {
        m_elements_count = 0;
        m_layout = nullptr;
        m_dirty = false;
    }

    void UniformBuffer::setValue(void* dest, const void* data, int datasize)
    {
        memcpy(dest, data, datasize);
        m_dirty = true;
    }

    void* UniformBuffer::find(const char* name, int element_idx)
    {
        for (const auto& f : m_layout->fields)
        {
            if (f.name == name)
            {
                int offset = f.offset + m_layout->stride * element_idx;
                return &m_data[offset];
            }
        }

        return nullptr;
    }

    void UniformBuffer::setState(const Layout* layout, int elemets_count, const void* data)
    {
        m_layout = layout;
        m_elements_count = elemets_count;
        m_data.resize(static_cast<size_t>(layout->stride) * elemets_count);
        assert(m_data.size());
        if (data)
        {
            memcpy(m_data.data(), data, m_data.size());
        }

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = UINT(m_data.size());
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        desc.MiscFlags = 0;
        desc.StructureByteStride = layout->stride;

        if (data)
        {
            D3D11_SUBRESOURCE_DATA dxdata = {};
            dxdata.pSysMem = data;
            dxdata.SysMemPitch = desc.ByteWidth;
            dxdata.SysMemSlicePitch = desc.ByteWidth;
            getD3DErr(m_device->getDX11Device()->CreateBuffer(&desc, &dxdata, &m_handle));
        }
        else
        {
            getD3DErr(m_device->getDX11Device()->CreateBuffer(&desc, nullptr, &m_handle));
        }
        
        m_dirty = false;
    }

    void UniformBuffer::setSubData(int start_element, int num_elements, const void* data)
    {
        memcpy(&m_data[static_cast<size_t>(m_layout->stride) * start_element], data, static_cast<size_t>(m_layout->stride) * num_elements);
        m_dirty = true;
    }

    void UniformBuffer::setValue(const char* name, float v, int element_idx)
    {
        void* dst = find(name, element_idx);
        if (dst)
        {
            setValue(dst, &v, sizeof(v));
        }
    }

    void UniformBuffer::setValue(const char* name, int i, int element_idx)
    {
        void* dst = find(name, element_idx);
        if (dst)
        {
            setValue(dst, &i, sizeof(i));
        }
    }

    void UniformBuffer::setValue(const char* name, const glm::vec2& v, int element_idx)
    {
        void* dst = find(name, element_idx);
        if (dst)
        {
            setValue(dst, &v, sizeof(v));
        }
    }

    void UniformBuffer::setValue(const char* name, const glm::vec3& v, int element_idx)
    {
        void* dst = find(name, element_idx);
        if (dst)
        {
            setValue(dst, &v, sizeof(v));
        }
    }

    void UniformBuffer::setValue(const char* name, const glm::vec4& v, int element_idx)
    {
        void* dst = find(name, element_idx);
        if (dst)
        {
            setValue(dst, &v, sizeof(v));
        }
    }

    void UniformBuffer::setValue(const char* name, const glm::mat4& m, int element_idx)
    {
        void* dst = find(name, element_idx);
        if (dst)
        {
            setValue(dst, &m, sizeof(m));
        }
    }

    void UniformBuffer::validateDynamicData()
    {
        if (!m_dirty)
        {
            return;
        }
        
        m_dirty = false;

        D3D11_MAPPED_SUBRESOURCE map_res = {};
        getD3DErr(m_device->getDX11DeviceContext()->Map(m_handle.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &map_res));
        memcpy(map_res.pData, m_data.data(), m_data.size());
        m_device->getDX11DeviceContext()->Unmap(m_handle.Get(), 0);
    }

    const Layout* UniformBuffer::getLayout() const
    {
        return nullptr;
    }

    ComPtr<ID3D11Buffer> UniformBuffer::getHandle()
    {
        return m_handle;
    }

    StructuredBuffer::StructuredBuffer(const GDevicePtr& device) : DeviceHolder(device)
    {
    }

    void StructuredBuffer::setState(int stride, int vertex_count, bool UAV, bool UAV_with_counter, const void* data)
    {
        m_uav = nullptr;
        m_srv = nullptr;

        m_stride = stride;
        m_vert_count = vertex_count;
        m_UAV_access = UAV;
        m_UAV_with_counter = UAV_with_counter;

        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = glm::max(vertex_count, 1) * m_stride;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        
        if (UAV)
        {
            desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        }
        
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        //if (UAV) desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
        desc.StructureByteStride = m_stride;

        if (data) 
        {
            D3D11_SUBRESOURCE_DATA dxdata = {};
            dxdata.pSysMem = data;
            dxdata.SysMemPitch = desc.ByteWidth;
            dxdata.SysMemSlicePitch = desc.ByteWidth;
            getD3DErr(m_device->getDX11Device()->CreateBuffer(&desc, &dxdata, &m_handle));
        }
        else
        {
            getD3DErr(m_device->getDX11Device()->CreateBuffer(&desc, nullptr, &m_handle));
        }
    }

    void StructuredBuffer::setSubData(int start_vertex, int num_vertices, const void* data)
    {
        assert(m_handle);
        if (num_vertices <= 0)
        {
            return;
        }

        D3D11_BOX box = {};
        box.left = start_vertex * m_stride;
        box.top = 0;
        box.right = box.left + num_vertices * m_stride;
        box.bottom = 1;
        box.front = 0;
        box.back = 1;
        m_device->getDX11DeviceContext()->UpdateSubresource(m_handle.Get(), 0, &box, data, 0, 0);
    }

    void StructuredBuffer::readBack(void* data)
    {
        D3D11_BUFFER_DESC desc = {};
        desc.ByteWidth = m_vert_count * m_stride;
        desc.Usage = D3D11_USAGE_STAGING;
        desc.BindFlags = 0;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        desc.StructureByteStride = m_stride;

        ComPtr<ID3D11Buffer> tmp_buf = {};
        getD3DErr(m_device->getDX11Device()->CreateBuffer(&desc, nullptr, &tmp_buf));

        m_device->getDX11DeviceContext()->CopyResource(tmp_buf.Get(), m_handle.Get());

        D3D11_MAPPED_SUBRESOURCE map = {};
        getD3DErr(m_device->getDX11DeviceContext()->Map(tmp_buf.Get(), 0, D3D11_MAP_READ, 0, &map));
        memcpy(data, map.pData, m_vert_count * m_stride);
        m_device->getDX11DeviceContext()->Unmap(tmp_buf.Get(), 0);
    }

    int StructuredBuffer::getStride() const
    {
        return m_stride;
    }

    int StructuredBuffer::getVertexCount() const
    {
        return m_vert_count;
    }

    ComPtr<ID3D11ShaderResourceView> StructuredBuffer::getShaderResource()
    {
        if (!m_vert_count)
        {
            return nullptr;
        }
        
        if (!m_srv)
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Format = DXGI_FORMAT_UNKNOWN;
            desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
            desc.Buffer.FirstElement = 0;
            desc.Buffer.NumElements = glm::max(m_vert_count, 1);
            getD3DErr(m_device->getDX11Device()->CreateShaderResourceView(m_handle.Get(), &desc, &m_srv));
        }

        return m_srv;
    }

    ComPtr<ID3D11UnorderedAccessView> StructuredBuffer::getUnorderedAccess()
    {
        assert(m_UAV_access);
        
        if (!m_uav)
        {
            D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};
            desc.Format = DXGI_FORMAT_UNKNOWN;
            desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            desc.Buffer.FirstElement = 0;
            desc.Buffer.NumElements = m_vert_count;
            desc.Buffer.Flags = m_UAV_with_counter ? D3D11_BUFFER_UAV_FLAG_COUNTER : 0;
            getD3DErr(m_device->getDX11Device()->CreateUnorderedAccessView(m_handle.Get(), &desc, &m_uav));
        }

        return m_uav;
    }

    GPUTexture2D::GPUTexture2D(const GDevicePtr& device) : DeviceHolder(device)
    {
        m_fmt = TextureFmt::RGBA8;
        m_size = glm::ivec2(0, 0);
        m_slices = 1;
        m_mips_count = 1;
    }

    TextureFmt GPUTexture2D::format() const
    {
        return m_fmt;
    }

    glm::ivec2 GPUTexture2D::size() const
    {
        return m_size;
    }

    int GPUTexture2D::slicesCount() const
    {
        return m_slices;
    }

    int GPUTexture2D::mipsCount() const
    {
        return m_mips_count;
    }

    void GPUTexture2D::setState(TextureFmt fmt, int mip_levels)
    {
        m_fmt = fmt;
        m_size = glm::ivec2(0, 0);
        m_slices = 0;
        m_mips_count = mip_levels;
        m_handle = nullptr;
        clearResViews();
    }

    void GPUTexture2D::setState(TextureFmt fmt, glm::ivec2 size, int mip_levels, int slices, const void* data)
    {
        m_fmt = fmt;
        m_size = size;
        m_slices = slices;
        m_mips_count = glm::clamp(mip_levels, 1, calcMipLevelsCount(size.x, size.y));

        D3D11_TEXTURE2D_DESC desc = {};
        desc.Width = m_size.x;
        desc.Height = m_size.y;
        desc.MipLevels = m_mips_count;
        desc.ArraySize = m_slices;
        desc.Format = toDXGI_Fmt(m_fmt);
        desc.SampleDesc = { 1, 0 };
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = 0;
        
        if (isShaderRes(m_fmt))
        {
            desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
        }
        
        if (isRenderTarget(m_fmt)) 
        {
            desc.BindFlags |= D3D11_BIND_RENDER_TARGET;
        }

        if (isDepthTarget(m_fmt))
        {
            desc.BindFlags |= D3D11_BIND_DEPTH_STENCIL;
        }
        
        if (isUAV(m_fmt)) 
        {
            desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        }

        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;
        
        if ((m_slices % 6 == 0) && isShaderRes(m_fmt))
        {
            desc.MiscFlags |= D3D11_RESOURCE_MISC_TEXTURECUBE;
        }
        
        if ((m_mips_count > 0) && isRenderTarget(m_fmt))
        {
            desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;
        }

        if (data)
        {
            D3D11_SUBRESOURCE_DATA d3ddata = {};
            d3ddata.pSysMem = data;
            d3ddata.SysMemPitch = getPixelsSize(m_fmt) * m_size.x;
            d3ddata.SysMemSlicePitch = d3ddata.SysMemPitch * m_size.y;
            getD3DErr(m_device->getDX11Device()->CreateTexture2D(&desc, &d3ddata, &m_handle));
        }
        else 
        {
            getD3DErr(m_device->getDX11Device()->CreateTexture2D(&desc, nullptr, &m_handle));
        }

        clearResViews();
    }

    void GPUTexture2D::setSubData(const glm::ivec2& offset, const glm::ivec2& size, int slice, int mip, const void* data)
    {
        assert(m_handle);

        UINT res_idx = D3D11CalcSubresource(mip, slice, m_mips_count);
        
        D3D11_BOX box = {};

        box.left = offset.x;
        box.top = offset.y;
        box.right = offset.x + size.x;
        box.bottom = offset.y + size.y;
        box.front = 0;
        box.back = 1;

        m_device->getDX11DeviceContext()->UpdateSubresource(m_handle.Get(), res_idx, &box, data, getPixelsSize(m_fmt) * size.x, getPixelsSize(m_fmt) * size.x * size.y);
    }

    void GPUTexture2D::generateMips()
    {
        m_device->getDX11DeviceContext()->GenerateMips(getShaderResource(false, false).Get());
    }

    void GPUTexture2D::readBack(void* data, int mip, int array_slice)
    {

    }

    ID3D11ShaderResourceView* GPUTexture2D::_getShaderResView(bool as_array, bool as_cubemap)
    {
        auto srv = getShaderResource(as_array, as_cubemap);
        return srv.Get();
    }

    ComPtr<ID3D11RenderTargetView> GPUTexture2D::buildRenderTarget(int mip, int slice_start, int slice_count) const
    {
        if (!m_handle)
        {
            return nullptr;
        }
        
        D3D11_RENDER_TARGET_VIEW_DESC desc = {};
        desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
        desc.Format = toDXGI_SRVFmt(m_fmt);
        desc.Texture2DArray.MipSlice = mip;
        desc.Texture2DArray.FirstArraySlice = slice_start;
        desc.Texture2DArray.ArraySize = slice_count;

        ComPtr<ID3D11RenderTargetView> res = {};
        getD3DErr(m_device->getDX11Device()->CreateRenderTargetView(m_handle.Get(), &desc, &res));
        
        return res;
    }

    ComPtr<ID3D11DepthStencilView> GPUTexture2D::buildDepthStencil(int mip, int slice_start, int slice_count, bool read_only) const
    {
        return ComPtr<ID3D11DepthStencilView>();
    }

    ComPtr<ID3D11ShaderResourceView> GPUTexture2D::getShaderResource(bool as_array, bool as_cubemap)
    {
        as_array = as_array || (m_slices > 1);
        int srv_idx = (as_array ? 1 : 0) | (as_cubemap ? 2 : 0);
        
        if (!m_srv[srv_idx])
        {
            D3D11_SHADER_RESOURCE_VIEW_DESC desc = {};
            desc.Format = toDXGI_SRVFmt(m_fmt);

            if (as_array) 
            {
                if (as_cubemap) 
                {
                    desc.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBEARRAY;
                    desc.TextureCubeArray.MipLevels = m_mips_count;
                    desc.TextureCubeArray.MostDetailedMip = 0;
                    desc.TextureCubeArray.First2DArrayFace = 0;
                    desc.TextureCubeArray.NumCubes = m_slices / 6;
                }
                else 
                {
                    desc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2DARRAY;
                    desc.Texture2DArray.MipLevels = m_mips_count;
                    desc.Texture2DArray.MostDetailedMip = 0;
                    desc.Texture2DArray.ArraySize = m_slices;
                    desc.Texture2DArray.FirstArraySlice = 0;
                }
            }
            else
            {
                if (as_cubemap) 
                {
                    desc.ViewDimension = D3D_SRV_DIMENSION_TEXTURECUBE;
                    desc.TextureCube.MipLevels = m_mips_count;
                    desc.TextureCube.MostDetailedMip = 0;
                }
                else 
                {
                    desc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
                    desc.Texture2D.MipLevels = m_mips_count;
                    desc.Texture2D.MostDetailedMip = 0;
                }
            }
            getD3DErr(m_device->getDX11Device()->CreateShaderResourceView(m_handle.Get(), &desc, &m_srv[srv_idx]));
        }

        return m_srv[srv_idx];
    }

    ComPtr<ID3D11UnorderedAccessView> GPUTexture2D::getUnorderedAccess(int mip, int slice_start, int slice_count, bool as_array)
    {
        // Hash key
        glm::ivec3 hk(mip, slice_start, slice_count);

        auto it = m_uav.find(hk);
        if (it == m_uav.end())
        {
            ComPtr<ID3D11UnorderedAccessView> new_view = {};
            D3D11_UNORDERED_ACCESS_VIEW_DESC desc = {};

            desc.Format = toDXGI_Fmt(m_fmt);
            bool is_array = (slice_count > 1) || as_array;
            desc.ViewDimension = is_array ? D3D11_UAV_DIMENSION_TEXTURE2DARRAY : D3D11_UAV_DIMENSION_TEXTURE2D;
            if (is_array) 
            {
                desc.Texture2DArray.MipSlice = mip;
                desc.Texture2DArray.FirstArraySlice = slice_start;
                desc.Texture2DArray.ArraySize = slice_count;
            }
            else 
            {
                desc.Texture2D.MipSlice = mip;
            }
            
            getD3DErr(m_device->getDX11Device()->CreateUnorderedAccessView(m_handle.Get(), &desc, &new_view));
            
            m_uav.insert({ hk, new_view });
            
            return new_view;
        }

        return it->second;
    }

    void GPUTexture2D::clearResViews()
    {
        m_uav.clear();
        m_srv[0] = nullptr;
        m_srv[1] = nullptr;
        m_srv[2] = nullptr;
        m_srv[3] = nullptr;
    }

    Framebuffer::Framebuffer(const GDevicePtr& device) : DeviceHolder(device)
    {
    }

    Framebuffer::~Framebuffer()
    {
        if (m_device->m_activeFboPtr == this)
        {
            m_device->setFrameBuffer(nullptr);
        }
    }

    void Framebuffer::clearColorSlot(int slot, const glm::vec4& color)
    {
        m_device->getDX11DeviceContext()->ClearRenderTargetView(m_color_views[slot].Get(), (float*)&color);
    }

    void Framebuffer::clearDS(float depth, bool clear_depth, char stencil, bool clear_stencil)
    {
        if (m_depth_view) 
        {
            UINT flags = 0;
            flags |= clear_depth ? D3D11_CLEAR_DEPTH : 0;
            flags |= clear_stencil ? D3D11_CLEAR_STENCIL : 0;
            m_device->getDX11DeviceContext()->ClearDepthStencilView(m_depth_view.Get(), flags, depth, stencil);
        }
    }

    void Framebuffer::setColorSlot(int slot, const GPUTexture2DPtr& tex, int mip, int slice_start, int slice_count)
    {
        if (m_tex[slot] != tex)
        {
            m_tex[slot] = tex;
            m_color_views[slot] = nullptr;
            m_colors_to_bind_dirty = true;
            m_colors_to_bind.clear();
        }
        
        Tex2DParams new_params(mip, slice_start, slice_count, false, false);
        
        if (!(m_tex_params[slot] == new_params))
        {
            m_tex_params[slot] = new_params;
            m_color_views[slot] = nullptr;
            m_colors_to_bind_dirty = true;
            m_colors_to_bind.clear();
        }
    }

    void Framebuffer::setDS(const GPUTexture2DPtr& tex, int mip, int slice_start, int slice_count, bool readonly)
    {
        if (m_depth != tex)
        {
            m_depth = tex;
            m_depth_view = nullptr;
        }
        
        Tex2DParams new_params(mip, slice_start, slice_count, readonly, false);
        
        if (!(m_depth_params == new_params))
        {
            m_depth_params = new_params;
            m_depth_view = nullptr;
        }
    }

    GPUTexture2DPtr Framebuffer::getColorSlot(int slot) const
    {
        return m_tex[slot];
    }

    GPUTexture2DPtr Framebuffer::getDS() const
    {
        return m_depth;
    }

    void Framebuffer::clearUAV(int slot, uint32_t v)
    {
        UINT clear_value[4] = { v,v,v,v };
        m_device->getDX11DeviceContext()->ClearUnorderedAccessViewUint(m_uav_to_bind[slot], clear_value);
    }

    void Framebuffer::setUAV(int slot, const GPUTexture2DPtr& tex, int mip, int slice_start, int slice_count, bool as_array)
    {
        m_uav_to_bind_count = -1;
        m_uav_to_bind.clear();
        m_uav[slot] = UAVSlot(tex, mip, slice_start, slice_count, as_array);
    }

    void Framebuffer::setUAV(int slot, const StructuredBufferPtr& buf, int initial_counter)
    {
        m_uav_to_bind_count = -1;
        m_uav_to_bind.clear();
        m_uav[slot] = UAVSlot(buf, initial_counter);
    }

    void Framebuffer::blitToDefaultFBO(int from_slot)
    {
        if (!m_tex[from_slot])
        {
            return;
        }
        
        if (!m_tex[from_slot]->m_handle)
        {
            return;
        }

        ID3D11Texture2D* tex = m_tex[from_slot]->m_handle.Get();
        D3D11_BOX src_box;
        src_box.left = 0;
        src_box.right = m_tex[from_slot]->m_size.x >> m_tex_params[from_slot].mip;
        src_box.top = 0;
        src_box.bottom = m_tex[from_slot]->m_size.y >> m_tex_params[from_slot].mip;
        src_box.front = 0;
        src_box.back = 1;
        UINT src_subres = D3D11CalcSubresource(m_tex_params[from_slot].mip, m_tex_params[from_slot].slice_start, m_tex[from_slot]->m_mips_count);
        ComPtr<ID3D11Resource> dest_res;
        m_device->m_RTView->GetResource(&dest_res);
        m_device->getDX11DeviceContext()->CopySubresourceRegion(dest_res.Get(), 0, 0, 0, 0, tex, src_subres, &src_box);
    }

    void Framebuffer::setSizeFromWindow()
    {
        RECT rct;
        GetClientRect(m_device->m_hwnd, &rct);
        setSize(glm::ivec2(rct.right - rct.left, rct.bottom - rct.top));
    }

    void Framebuffer::setSize(const glm::ivec2& xy)
    {
        if (m_size != xy)
        {
            for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
                m_color_views[i] = nullptr;
            m_depth_view = nullptr;
            m_colors_to_bind.clear();
        }
        
        m_size = xy;
        
        for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
        {
            if (!m_tex[i]) continue;
            if (m_tex[i]->size() != xy) 
            {
                m_tex[i]->setState(m_tex[i]->format(), xy, m_tex[i]->mipsCount(), 1);
                m_color_views[i] = nullptr;
                m_colors_to_bind.clear();
            }
        }

        if (m_depth) 
        {
            if (m_depth->size() != xy) 
            {
                m_depth->setState(m_depth->format(), xy, m_depth->mipsCount(), 1);
                m_depth_view = nullptr;
            }
        }
    }

    glm::ivec2 Framebuffer::getSize() const
    {
        return m_size;
    }

    void Framebuffer::prepareSlots()
    {
        if (m_colors_to_bind.size() == 0)
        {
            m_rtv_count = 0;
            for (int i = 0; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; i++)
            {
                if (m_tex[i]) 
                {
                    m_rtv_count = i + 1;

                    if (!m_color_views[i])
                    {
                        m_color_views[i] = m_tex[i]->buildRenderTarget(m_tex_params[i].mip, m_tex_params[i].slice_start, m_tex_params[i].slice_count);
                    }
                    
                    m_colors_to_bind.push_back(m_color_views[i].Get());
                }
                else 
                {
                    m_colors_to_bind.push_back(nullptr);
                }
            }
        }

        if (m_uav_to_bind_count < 0)
        {
            m_uav_to_bind_count = 0;
            for (int i = m_rtv_count; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT + D3D11_PS_CS_UAV_REGISTER_COUNT; i++) 
            {
                if (m_uav[i].kind != UAV_slot_kind::empty)
                {
                    m_uav_to_bind_count++;
                }
            }
        }

        m_uav_to_bind.clear();
        m_uav_initial_counts.clear();

        if (m_uav_to_bind_count > 0)
        {
            for (int i = m_rtv_count; i < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT + D3D11_PS_CS_UAV_REGISTER_COUNT; i++)
            {
                switch (m_uav[i].kind) 
                {
                case (UAV_slot_kind::tex):
                    m_uav_to_bind.push_back(m_uav[i].tex->getUnorderedAccess(m_uav[i].tex_params.mip, m_uav[i].tex_params.slice_start, m_uav[i].tex_params.slice_count, m_uav[i].tex_params.as_array).Get());
                    m_uav_initial_counts.push_back(m_uav[i].initial_counter);
                    break;
                case (UAV_slot_kind::buf):
                    m_uav_to_bind.push_back(m_uav[i].buf->getUnorderedAccess().Get());
                    m_uav_initial_counts.push_back(m_uav[i].initial_counter);
                    break;
                case (UAV_slot_kind::empty):
                    m_uav_to_bind.push_back(nullptr);
                    m_uav_initial_counts.push_back(-1);
                    break;
                }
            }
        }

        if (m_depth && (!m_depth_view))
        {
            m_depth_view = m_depth->buildDepthStencil(m_depth_params.mip, m_depth_params.slice_start, m_depth_params.slice_count, m_depth_params.read_only);
        }
    }

    Framebuffer::Tex2DParams::Tex2DParams()
    {
        mip = 0;
        slice_start = 0;
        slice_count = 0;
        read_only = false;
    }
    
    Framebuffer::Tex2DParams::Tex2DParams(int m, int s_start, int s_count, bool ronly, bool as_array)
    {
        mip = m;
        slice_start = s_start;
        slice_count = s_count;
        read_only = ronly;
        this->as_array = as_array;
    }

    bool Framebuffer::Tex2DParams::operator==(const Tex2DParams& b)
    {
        return (mip == b.mip) && (slice_start == b.slice_start) && (slice_count == b.slice_count) && (read_only == b.read_only);
    }
}