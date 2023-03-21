#include "eutils.h"

#define STBI_WINDOWS_UTF8
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace EProject
{

    CameraBase::CameraBase(const GDevicePtr& device)
    {
        if (!device)
        {
            return;
        }

        m_ubo = device->createUniformBuffer();
        m_ubo->setState(getLayoutSelector()->end(sizeof(CameraBuf)), 1, nullptr);
    }

    CameraBase::~CameraBase()
    {

    }

    glm::mat4 CameraBase::getView() const
    {
        return m_buf.view;
    }

    glm::mat4 CameraBase::getProj() const
    {
        return m_buf.proj;
    }

    glm::mat4 CameraBase::getViewProj() const
    {
        return m_buf.view_proj;
    }

    glm::mat4 CameraBase::getViewInv() const
    {
        return m_buf.view_inv;
    }

    glm::mat4 CameraBase::getProjInv() const
    {
        return m_buf.proj_inv;
    }

    glm::mat4 CameraBase::getViewProjInv() const
    {
        return m_buf.view_proj_inv;
    }

    UniformBufferPtr CameraBase::getUBO()
    {
        m_ubo->validateDynamicData();
        return m_ubo;
    }

    Camera2D::Camera2D(const GDevicePtr& device) : CameraBase(device)
    {
    }

    void Camera2D::updateScreen(float fov)
    {        
        const auto fbSize = m_ubo->getDevice()->currentFrameBufferSize();
        const float aspectRatio = static_cast<float>(fbSize.x) / static_cast<float>(fbSize.y);

        float camFOV = fov;
        float bottom = -camFOV;
        float top = camFOV;
        float left = bottom * aspectRatio;
        float right = top * aspectRatio;

        m_buf.view = glm::mat4(1.0);
        m_buf.proj = glm::orthoRH(left, right, bottom, top, m_buf.z_near_far.x, m_buf.z_near_far.y);

        m_buf.updateViewProj();
        m_ubo->setSubData(0, 1, &m_buf);
    }

    Camera3D::Camera3D(const GDevicePtr& device) : CameraBase(device)
    {
    }

    void Camera3D::lookAt(const glm::vec3& pos, const glm::vec3& dir, const glm::vec3& up)
    {
        const auto fbSize = m_ubo->getDevice()->currentFrameBufferSize();
        const float aspectRatio = static_cast<float>(fbSize.x) / static_cast<float>(fbSize.y);

        // DirectX3d
        m_buf.view = glm::lookAtRH(pos, dir, up);
        m_buf.proj = glm::perspectiveFovRH_ZO(glm::radians(45.0f), static_cast<float>(fbSize.x), static_cast<float>(fbSize.y), m_buf.z_near_far.x, m_buf.z_near_far.y);

        m_buf.updateViewProj();
        m_ubo->setSubData(0, 1, &m_buf);
    }

    void Camera3D::setPosition(const glm::vec3& pos)
    {
        m_pos = pos;
    }

    const glm::vec3& Camera3D::getPosition() const
    {
        return m_pos;
    }

    void Camera3D::setRotation(const glm::vec3& rot)
    {
        m_rot = rot;
    }

    const glm::vec3& Camera3D::getRotation() const
    {
        return m_rot;
    }

    std::filesystem::path PathHandler::getDataDir()
    {
        auto currentPath = std::filesystem::current_path();
        return currentPath.parent_path() / "Data";
    }

    std::filesystem::path PathHandler::getShadersDir()
    {
        return getDataDir() / "Shaders";
    }

    std::filesystem::path PathHandler::getTexturesDir()
    {
        return getDataDir() / "Textures";
    }

    std::filesystem::path PathHandler::getModelsDir()
    {
        return getDataDir() / "Models";
    }

    std::size_t PathKey::operator()(const PathKey& k) const
    {
        return std::filesystem::hash_value(k.path);
    }

    bool PathKey::operator==(const PathKey& other) const
    {
        return path == other.path;
    }

    Texture2D::Texture2D(const Texture2D& _rhs)
    {
        m_fmt = _rhs.m_fmt;
        m_size = _rhs.m_size;
        
        memcpy((char*)m_data, _rhs.m_data, sizeof(_rhs.m_data));
    }

    Texture2D::~Texture2D()
    {
        if (m_data)
        {
            stbi_image_free(m_data);
        }
    }

    Texture2D::Texture2D(const std::filesystem::path& _path) : IAsset(_path)
    {
        
    }

    void Texture2D::init()
    {

    }

    bool Texture2D::load(const GDevicePtr& _ptr)
    {
        //stbi_is_hdr - check for floating point format
        //stbi_is_16_bit - check for 16 bif formats?
        //stbi_loadf - load floating point format
        //stbi_failure_reason - get error information

        stbi_set_flip_vertically_on_load(0);

        int comp;
        int targetformat = STBI_rgb_alpha;
        m_data = stbi_load(m_path.u8string().c_str(), &m_size.x, &m_size.y, &comp, targetformat);
        if (!m_data)
        {
            throw std::runtime_error("Texture2D: Load failed: " + m_path.u8string());
        }

        m_fmt = TextureFmt::RGBA8;
        m_valid = true;

        return true;
    }

    bool Texture2D::unload()
    {
        if (m_data)
        {
            stbi_image_free(m_data);

            return true;
        }

        return false;
    }

    const void* Texture2D::getData() const
    {
        return (const void*)m_data;
    }

    IAsset::IAsset(const std::filesystem::path& p) : m_path(p)
    {
        assert(std::filesystem::exists(m_path));
    }

    AssetManager::AssetManager(const GDevicePtr& _ptr) : m_ptr(_ptr)
    {
    }


};
