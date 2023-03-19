#pragma once

#include "egapi.h"
#include "emath.h"

#include <unordered_map>
#include <filesystem>

namespace EProject
{
    using stbi_uc = unsigned char;

    static constexpr glm::vec3 MVEK3UP = glm::vec3(0.0f, 1.0f, 0.0f);
    static constexpr glm::vec3 MVEK3FORWARD = glm::vec3(0.0f, 0.0f, 1.0f);

    class File
    {
    public:
        std::filesystem::path getPath()
        {
            return m_path;
        }

        bool isGood()
        {
            return m_f;
        }

        File(const std::filesystem::path& filename, bool write = false)
        {
            m_path = std::filesystem::absolute(filename);
#ifdef _WIN32
            _wfopen_s(&m_f, m_path.wstring().c_str(), write ? L"wb" : L"rb");
#else
            fopen_s(&m_f, m_path.string().c_str(), write ? L"wb" : L"rb");
#endif
        }

        ~File()
        {
            if (m_f)
                fclose(m_f);
        }

        inline void readBuf(void* v, int size)
        {
            fread(v, size, 1, m_f);
        }

        inline void writeBuf(const void* v, int size)
        {
            fwrite(v, size, 1, m_f);
        }

        template <typename T>
        inline T& Read(T& x) {
            readBuf(&x, sizeof(x));
            return x;
        }

        template <typename T>
        inline void write(const T& x)
        {
            writeBuf(&x, sizeof(x));
        }

        inline std::string readString() {
            uint32_t n;
            Read(n);
            std::string res;
            if (n) {
                res.resize(n);
                readBuf(const_cast<char*>(res.data()), n);
            }
            return res;
        }

        inline void writeString(const std::string& str) {
            uint32_t n = uint32_t(str.size());
            write(n);
            if (n)
            {
                writeBuf(str.data(), n);
            }
        }

        inline int tell()
        {
            return ftell(m_f);
        }
    private:
        std::filesystem::path m_path;
        FILE* m_f;
    };

    class QPC
    {
    public:
        uint64_t TimeMcS() const;

        uint64_t Time() const;
        void Pause();
        void Unpause();
        QPC();

    private:
        uint64_t m_start;
        uint64_t m_freq;
        bool m_paused = false;
        uint64_t m_paused_time;
    };

    class CameraBase
    {
    protected:
        struct CameraBuf
        {
            glm::mat4 view;
            glm::mat4 proj;
            glm::mat4 view_proj;
            glm::mat4 view_inv;
            glm::mat4 proj_inv;
            glm::mat4 view_proj_inv;
            glm::vec2 z_near_far;
            glm::vec2 dummy;
            
            void updateViewProj()
            {
                // DirectX
                view_proj = glm::transpose(proj * view);
                
                view_inv = glm::inverse(glm::transpose(view));
                proj_inv = glm::inverse(glm::transpose(proj));
                view_proj_inv = glm::inverse(view_proj);
            }

            CameraBuf() :
                view(1),
                proj(1),
                view_proj(1),
                view_inv(1),
                proj_inv(1),
                view_proj_inv(1),
                z_near_far(0.1f, 100.0f),
                dummy(0) {
            }
        };
    public:

        explicit CameraBase(const GDevicePtr& device);
        virtual ~CameraBase();

        glm::mat4 getView() const;
        glm::mat4 getProj() const;
        glm::mat4 getViewProj() const;
        glm::mat4 getViewInv() const;
        glm::mat4 getProjInv() const;
        glm::mat4 getViewProjInv() const;

        UniformBufferPtr getUBO();

    protected:
        CameraBuf m_buf;
        UniformBufferPtr m_ubo;
    };

    class Camera2D : public CameraBase
    {
    public:
        Camera2D(const GDevicePtr& dev);

        void updateScreen(float fov);    
    };

    class Camera3D : public CameraBase
    {
    public:
        Camera3D(const GDevicePtr& dev);

        void setPosition(const glm::vec3& pos);
        const glm::vec3& getPosition() const;

        void setRotation(const glm::vec3& rot);
        const glm::vec3& getRotation() const;

        void lookAt(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up);

    private:
        glm::vec3 m_pos = { 0.0f, 0.0f, -10.0f };
        glm::vec3 m_rot = { 0.0f, 0.0f, 0.0f };
    };

    using Camera2DPtr = std::shared_ptr<Camera2D>;
    using Camera3DPtr = std::shared_ptr<Camera3D>;
    
    class AssetManager;

    class PathHandler
    {
    public:

        static std::filesystem::path getDataDir();
        static std::filesystem::path getShadersDir();
        static std::filesystem::path getTexturesDir();
        static std::filesystem::path getModelsDir();
    };

    class IAsset
    {
    public:
        IAsset() = default;
        IAsset(const std::filesystem::path& p);
        virtual ~IAsset() = default;

        virtual void init() = 0;

        virtual bool load(const GDevicePtr& _ptr) = 0;
        virtual bool unload() = 0;
        
        const std::string& getTag() const { return m_tag; }
        std::string& getTag() { return m_tag; }
 
        bool isValid() const { return m_valid; }

    protected:
        std::string m_tag = "";
        std::filesystem::path m_path;
        bool m_valid = false;
    };

    struct PathKey
    {
        std::filesystem::path path;

        std::size_t operator()(const PathKey& k) const;
        bool operator==(const PathKey& other) const;

        PathKey() = default;
        PathKey(const std::filesystem::path& _str) : path(_str) {}
    };

    class Texture2D : public IAsset
    {
    public:
        Texture2D() = default;
        ~Texture2D() override;
        
        Texture2D(const std::filesystem::path& _path);
        Texture2D(const Texture2D& r);

        void init() override;

        bool load(const GDevicePtr& _ptr) override;
        bool unload() override;

        TextureFmt  getFormat() const { return m_fmt; }
        glm::ivec2  getSize()   const { return m_size; }
        const void* getData()   const;

    private:
        stbi_uc* m_data = nullptr;
        TextureFmt m_fmt = TextureFmt::RGBA8;
        glm::ivec2 m_size = glm::ivec2(0);
    };

    template<typename T>
    using Asset = std::shared_ptr<T>;

    class AssetManager final
    {
    public:

        explicit AssetManager(const GDevicePtr& _ptr);

        template<typename T>
        Asset<T> getAsset(const std::filesystem::path& _pathKey)
        {
            static_assert(std::is_base_of<IAsset, T>::value, "AssetManager: Asset not from base IAsset class!");

            auto it = m_cache.find(_pathKey);
            if (it == m_cache.end())
            {
                auto result = std::make_unique<T>(_pathKey);
                bool loaded = result->load(m_ptr);               

                if (loaded)
                {
                    result->init();
                    it = m_cache.insert({ std::move(_pathKey), std::move(result) }).first;
                }                                              
            }

            return std::static_pointer_cast<T>(it->second);
        }

    private:
        std::unordered_map<PathKey, std::shared_ptr<IAsset>, PathKey> m_cache;
        GDevicePtr m_ptr;
    };

    using AssetManagerPtr = std::shared_ptr<AssetManager>;
}
