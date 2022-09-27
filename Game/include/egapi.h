#pragma once

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <memory>
#include <wrl.h>
#include <glmh.h>
#include <array>

namespace EProject
{
    using namespace Microsoft::WRL;

    enum class TextureFmt
    {
        None,
        R8, RG8, RGBA8, RGBA8_SRGB,
        R16, RG16, RGBA16,
        R16f, RG16f, RGBA16f,
        R32, RG32, RGB32, RGBA32,
        R32f, RG32f, RGB32f, RGBA32f,
        D16, D24_S8, D32f, D32f_S8,
    };

    enum class PrimTopology
    {
        Point,
        Line,
        Linestrip,
        Triangle,
        Trianglestrip,
    };

    enum class TexFilter { None, Point, Linear };
    enum class TexWrap { Repeat, Mirror, Clamp, ClampBorder };

    enum class CullMode { None, Back, Front };
    enum class Compare { Never, Less, Equal, Less_Equal, Greater, Not_Equal, Greater_Equal, Always };
    enum class StencilOp { Keep, Zero, Replace, Inc_Clamp, Dec_Clamp, Ind, Dec };
    enum class BlendFunc { Add, Sub, Rev_Sub, Min, Max };
    enum class Blend { Zero, One, Src_Alpha, Inv_Src_Alpha, Dst_Alpha, Inv_Dst_Alpha, Src_Color, Inv_Src_Color, Dst_Color, Inv_Dst_Color };

    struct Sampler
    {
        TexFilter filter;
        TexFilter mipfilter;
        int anisotropy;
        TexWrap wrap_x;
        TexWrap wrap_y;
        TexWrap wrap_z;
        glm::vec4 border;
        Compare comparison;

        bool operator==(const Sampler& p) const
        {
            return memcmp(this, &p, sizeof(Sampler)) == 0;
        }

        std::size_t operator() (const Sampler& s) const
        {
            //return MurmurHash2(this, sizeof(Sampler));
            return 1;
        }
    };

    const static Sampler cSampler_Linear
    {
        TexFilter::Linear , TexFilter::Linear , 16 ,
        TexWrap::Repeat, TexWrap::Repeat, TexWrap::Repeat, {0,0,0,0},
        Compare::Never
    };

    const static Sampler cSampler_LinearClamped
    {
        TexFilter::Linear , TexFilter::Linear , 16 ,
        TexWrap::Clamp, TexWrap::Clamp, TexWrap::Clamp, {0,0,0,0},
        Compare::Never
    };

    const static Sampler cSampler_NoFilter
    {
        TexFilter::Point , TexFilter::None , 0 ,
        TexWrap::Repeat, TexWrap::Repeat, TexWrap::Repeat, {0,0,0,0},
        Compare::Never
    };

    struct DrawIndexedCmd
    {
        UINT IndexCount;
        UINT InstanceCount;
        UINT StartIndex;
        INT  BaseVertex;
        UINT BaseInstance;
    };

    class States
    {
    private:
        struct StateData
        {
            D3D11_RASTERIZER_DESC m_r_desc;
            ComPtr<ID3D11RasterizerState> m_r_state;
            UINT m_stencil_ref;
            D3D11_DEPTH_STENCIL_DESC m_d_desc;
            ComPtr<ID3D11DepthStencilState> m_d_state;
            D3D11_BLEND_DESC m_b_desc;
            ComPtr<ID3D11BlendState> m_b_state;

            inline StateData(States* st)
            {
                m_r_desc = st->m_r_desc;
                m_d_desc = st->m_d_desc;
                m_b_desc = st->m_b_desc;
                m_r_state = st->m_r_state;
                m_d_state = st->m_d_state;
                m_b_state = st->m_b_state;
                m_stencil_ref = st->m_stencil_ref;
            }
        };
    private:
        ID3D11Device* m_device;
        ID3D11DeviceContext* m_deviceContext;

        D3D11_RASTERIZER_DESC m_r_desc;
        ComPtr<ID3D11RasterizerState> m_r_state;

        UINT m_stencil_ref;
        D3D11_DEPTH_STENCIL_DESC m_d_desc;
        ComPtr<ID3D11DepthStencilState> m_d_state;

        D3D11_BLEND_DESC m_b_desc;
        ComPtr<ID3D11BlendState> m_b_state;

        std::vector<StateData> m_states;
    private:
        void setDefaultStates();
    public:
        void push();
        void pop();

        void setWireframe(bool wire);
        void setCull(CullMode cm);

        void setDepthEnable(bool enable);
        void setDepthWrite(bool enable);
        void setDepthFunc(Compare cmp);

        void setBlend(bool enable, Blend src = Blend::One, Blend dst = Blend::One, int rt_index = -1, BlendFunc bf = BlendFunc::Add);
        void setBlendSeparateAlpha(bool enable, Blend src_color = Blend::One, Blend dst_color = Blend::One, BlendFunc bf_color = BlendFunc::Add,
            Blend src_alpha = Blend::One, Blend dst_alpha = Blend::One, BlendFunc bf_alpha = BlendFunc::Add,
            int rt_index = -1);

        void setColorWrite(bool enable, int rt_index = -1);

        void validateStates();
        States(ID3D11Device* device, ID3D11DeviceContext* device_context);
    };

    class ShaderProgram;
    class VertexBuffer;
    class IndexBuffer;
    class UniformBuffer;
    class StructuredBuffer;

    using ShaderProgramPtr = std::shared_ptr<ShaderProgram>;
    using VertexBufferPtr = std::shared_ptr<VertexBuffer>;
    using IndexBufferPtr = std::shared_ptr<IndexBuffer>;
    using UniformBufferPtr = std::shared_ptr<UniformBuffer>;
    using StructuredBufferPtr = std::shared_ptr<StructuredBuffer>;

    class GDevice : public std::enable_shared_from_this<GDevice>
    {
        friend class ShaderProgram;
        friend class VertexBuffer;
        friend class IndexBuffer;
        friend class UniformBuffer;
        friend class StructuredBuffer;
    public:
        GDevice(HWND wnd, bool sRGB);
        ~GDevice();

        //FrameBufferPtr setFrameBuffer(const FrameBufferPtr& fbo, bool update_viewport = true);
        glm::ivec2 currentFrameBufferSize() const;

        ID3D11Device* getDX11Device() const;
        ID3D11DeviceContext* getDX11DeviceContext() const;

        States* getStates();

        ShaderProgram* getActiveProgram();

        //FrameBufferPtr getActiveFrameBuffer() const;
        //Program* getActiveProgram();

        /*FrameBufferPtr Create_FrameBuffer();
        Texture2DPtr Create_Texture2D();
        Texture3DPtr Create_Texture3D();*/

        IndexBufferPtr createIndexBuffer();
        VertexBufferPtr createVertexBuffer();
        ShaderProgramPtr createShaderProgram();
        UniformBufferPtr createUniformBuffer();
        StructuredBufferPtr createStructuredBuffer();

        void beginFrame();
        void endFrame();

        bool isSRGB() const;
        HWND getWindow() const;

    protected:

        ShaderProgram* m_activeProgram = nullptr;

    private:

        void setDefaultFramebuffer();
        void setViewport(const glm::vec2& size);
    private:
        HWND m_hwnd;

        ComPtr<ID3D11Device> m_dev;
        ComPtr<ID3D11DeviceContext> m_context;
        ComPtr<IDXGISwapChain> m_swapChain;
        ComPtr<ID3D11Texture2D> m_backBuffer;
        ComPtr<ID3D11RenderTargetView> m_RTView;

        std::unique_ptr<States> m_states;
        glm::ivec2 m_lastWndSize;
        bool m_isSrgb;

    };

    using GDevicePtr = std::shared_ptr<GDevice>;

    class DeviceHolder
    {
    protected:
        GDevicePtr m_device;
    public:
        DeviceHolder(const GDevicePtr& device);
        GDevicePtr getDevice();
    };

    enum class ShaderType { Vertex = 0, Hull = 1, Domain = 2, Geometry = 3, Pixel = 4, Compute = 5 };

    struct ShaderInput
    {
        std::wstring filePath;
        std::string target;
        std::string entyPoint;
        ShaderType type = ShaderType::Vertex;
    };

    enum class LayoutType { Byte, Word, UInt, Float };

    struct LayoutField
    {
        std::string name;
        LayoutType type = LayoutType::Byte;
        int num_fields = 0;
        bool do_norm = false;
        int offset = 0;
        int array_size = 0;
        int getSize() const;
        bool operator==(const LayoutField& l) const;
        std::size_t hash() const;
    };

    struct Layout
    {
        std::vector<LayoutField> fields;
        int stride;
        bool operator==(const Layout& l) const;
        std::size_t hash() const;
    };

    class ShaderProgram : public DeviceHolder
    {
    private:

        const ShaderType cShaders[6] = { ShaderType::Vertex, ShaderType::Hull, ShaderType::Domain, ShaderType::Geometry, ShaderType::Pixel, ShaderType::Compute };

        enum class SlotKind { Uniform, Texture, Buffer, Sampler };

        struct ShaderSlot
        {
            SlotKind kind;
            std::string name;
            const Layout* layout;
            int bindPoints[6] = { -1,-1,-1,-1,-1,-1 };

            ComPtr<ID3D11ShaderResourceView> view;
            ComPtr<ID3D11Buffer> buffer;
            ID3D11SamplerState* sampler;

            void select(ID3D11DeviceContext* dev) const;
            ShaderSlot() : kind(SlotKind::Uniform), layout(nullptr), sampler(nullptr) {}
        };

        struct InputLayoutData
        {
            const Layout* vertices;
            const Layout* instances;
            int step_rate;
            ComPtr<ID3D11InputLayout> m_dx_layout;
            InputLayoutData();
            void rebuildLayout(ID3D11Device* device, const std::string& shader_code);
        };

    public:

        ShaderProgram(const GDevicePtr& device);
        ~ShaderProgram();

        bool compileFromFile(const ShaderInput& input);
        bool create();

        void activateProgram();
        void setInputBuffers(const VertexBufferPtr& vbo, const IndexBufferPtr& ibo, const VertexBufferPtr& instances, int instanceStepRate);

        void drawIndexed(PrimTopology pt, const DrawIndexedCmd& cmd);
        void drawIndexed(PrimTopology pt, const std::vector<DrawIndexedCmd>& cmd_buf);
        void drawIndexed(PrimTopology pt, int index_start = 0, int index_count = -1, int instance_count = -1, int base_vertex = 0, int base_instance = 0);
        void draw(PrimTopology pt, int vert_start = 0, int vert_count = -1, int instance_count = -1, int base_instance = 0);

        void setValue(const char* name, float v);
        void setValue(const char* name, int i);
        void setValue(const char* name, const glm::vec2& v);
        void setValue(const char* name, const glm::vec3& v);
        void setValue(const char* name, const glm::vec4& v);
        void setValue(const char* name, const glm::mat4& m);

        //void setResource(const char* name, const UniformBufferPtr& ubo);
        //void setResource(const char* name, const StructuredBufferPtr& sbo);
        //void setResource(const char* name, const Texture2DPtr& tex, bool as_array = false, bool as_cubemap = false);
        //void setResource(const char* name, const Texture3DPtr& tex);
        void setResource(const char* name, const Sampler& s);


    private:

        void autoReflect(const void* data, int data_size, ShaderType st);

        void selectInputBuffers();
        void selectTopology(PrimTopology pt);

        bool isProgramActive() const;

        ID3D11InputLayout* getLayout(const Layout* vertices, const Layout* instances, int step_rate);

        int obtainSlotIdx(SlotKind kind, const std::string& name, const Layout* layout);
        int findSlot(const char* name);

    private:

        UniformBufferPtr m_ub[6];
        bool m_globals_dirty;

        ID3D10Blob* m_shaderData[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
        std::array<std::string, 6> m_shaderCode = { std::string(), std::string(), std::string(), std::string(), std::string(), std::string() };
        ComPtr<ID3D11DeviceChild> m_shaders[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

        std::vector<ShaderSlot> m_slots;

        VertexBufferPtr m_selectedVBO;
        IndexBufferPtr m_selectedIBO;
        VertexBufferPtr m_selectedInstances;
        int m_selectedInstanceStep;

        std::vector<InputLayoutData> m_layouts;

    };

    class VertexBuffer : public DeviceHolder
    {
        friend class ShaderProgram;
    public:
        VertexBuffer(const GDevicePtr& device);

        void setState(const Layout* layout, int vertex_count, const void* data = nullptr);
        void setSubData(int start_vertex, int num_vertices, const void* data);

        int getVertexCount() const;
        const Layout* getLayout() const;
    private:
        const Layout* m_layout = nullptr;
        ComPtr<ID3D11Buffer> m_handle;
        int m_vertCount = 0;
    };

    class IndexBuffer : public DeviceHolder
    {
        friend class ShaderProgram;
    public:
        IndexBuffer(const GDevicePtr& device);

        void setState(int ind_count, const void* data = nullptr);
        void setSubData(int start_idx, int num_indices, const void* data);

        int getIndexCount() const;

    private:
        ComPtr<ID3D11Buffer> m_handle;
        int m_indCount = 0;
    };

    class StructuredBuffer : public DeviceHolder
    {
        friend class ShaderProgram;
        friend class FrameBuffer;
    public:
        StructuredBuffer(const GDevicePtr& device);

        void setState(int stride, int vertex_count, bool UAV = false, bool UAV_with_counter = false, const void* data = nullptr);
        void setSubData(int start_vertex, int num_vertices, const void* data);

        void readBack(void* data);

        int getStride() const;
        int getVertexCount() const;

    private:
        ComPtr<ID3D11ShaderResourceView> getShaderResourceView();
        ComPtr<ID3D11UnorderedAccessView> getUnorderedAccessView();

    private:
        ComPtr<ID3D11Buffer> m_handle;
        ComPtr<ID3D11ShaderResourceView> m_srv;
        ComPtr<ID3D11UnorderedAccessView> m_uav;
        int m_stride = 0;
        int m_vert_count = 0;
        bool m_UAV_access = false;
        bool m_UAV_with_counter = false;
    };

    class LayoutSelector
    {
    public:
        virtual LayoutSelector* add(const std::string& name, LayoutType type, int num_fields, bool do_norm = true, int offset = -1, int array_size = 1) = 0;
        virtual const Layout* end(int stride_size = -1) = 0;
    };

    class LayoutSelectorInstance : public LayoutSelector
    {
    public:

        LayoutSelector* add(const std::string& name, LayoutType type, int num_fields, bool do_norm = true, int offset = -1, int array_size = 1) override
        {
            LayoutField f;
            f.name = name;
            f.type = type;
            f.num_fields = num_fields;
            f.do_norm = do_norm;
            f.offset = (offset < 0) ? m_curr_layout.stride : offset;
            f.array_size = array_size;
            m_curr_layout.fields.push_back(f);
            m_curr_layout.stride += f.getSize();
            return this;
        }

        const Layout* end(int stride_size = -1) override
        {
            if (stride_size > 0)
            {
                m_curr_layout.stride = stride_size;
            }

            auto it = m_cache.find(m_curr_layout);

            if (it == m_cache.end())
            {
                m_cache.insert({ m_curr_layout, std::make_unique<Layout>(m_curr_layout) });
                it = m_cache.find(m_curr_layout);
            }

            return it->second.get();
        }

        LayoutSelector* reset()
        {
            m_curr_layout.fields.clear();
            m_curr_layout.stride = 0;
            return this;
        };

    private:
        struct hash_fn
        {
            std::size_t operator() (const Layout& l) const
            {
                return l.hash();
            }
        };

    private:
        std::unordered_map<Layout, std::unique_ptr<Layout>, hash_fn> m_cache;
        Layout m_curr_layout;
    };

    LayoutSelector* getLayoutSelector();

    class UniformBuffer : public DeviceHolder
    {
        friend class Program;
    public:
        UniformBuffer(const GDevicePtr& device);
        
        void setState(const Layout* layout, int elemets_count, const void* data = nullptr);
        void setSubData(int start_element, int num_elements, const void* data);
        void setValue(const char* name, float v, int element_idx = 0);
        void setValue(const char* name, int i, int element_idx = 0);
        void setValue(const char* name, const glm::vec2& v, int element_idx = 0);
        void setValue(const char* name, const glm::vec3& v, int element_idx = 0);
        void setValue(const char* name, const glm::vec4& v, int element_idx = 0);
        void setValue(const char* name, const glm::mat4& m, int element_idx = 0);
        void validateDynamicData();
       
        const Layout* getLayout() const;
        ComPtr<ID3D11Buffer> getHandle();

    private:
        void setValue(void* dest, const void* data, int datasize);
        void* find(const char* name, int element_idx);

    private:
        std::vector<char> m_data;
        ComPtr<ID3D11Buffer> m_handle;
        const Layout* m_layout;
        int m_elements_count;
        bool m_dirty = false;
    };

}
