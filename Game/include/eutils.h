#pragma once

#include "egapi.h"
#include "emath.h"

#include <filesystem>

namespace EProject
{
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
                view_proj = proj * view;
                view_inv = glm::inverse(view);
                proj_inv = glm::inverse(proj);
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

        CameraBase(const GDevicePtr& device);
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

        void updateScreen();    
    };

    class Camera3D : public CameraBase
    {
    public:
        Camera3D(const GDevicePtr& dev);
    };

    using Camera2DPtr = std::shared_ptr<Camera2D>;
    using Camera3DPtr = std::shared_ptr<Camera3D>;

    class PathHandler
    {
    public:

        static std::filesystem::path getDataDir();
        static std::filesystem::path getShadersDir();
    };

}
