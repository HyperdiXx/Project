#include "eutils.h"


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

    void Camera2D::updateScreen()
    {        
        auto fbSize = m_ubo->getDevice()->currentFrameBufferSize();

        float aspectRatio = (float)fbSize.x / (float)fbSize.y;

        float camFOV = 60.0f;
        float bottom = -camFOV;
        float top = camFOV;
        float left = bottom * aspectRatio;
        float right = top * aspectRatio;

        m_buf.view = glm::mat4(1.0);
        m_buf.proj = glm::orthoLH(left, right, bottom, top, m_buf.z_near_far.x, m_buf.z_near_far.y);

        m_buf.updateViewProj();
        m_ubo->setSubData(0, 1, &m_buf);
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

};
