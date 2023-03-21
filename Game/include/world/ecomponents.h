#pragma once

#include <ecs/ecomponent.h>
#include <egapi.h>
#include <graphics/emesh.h>
#include <emath.h>

using namespace ECS;
using namespace EProject;

class TagComponent final
{
public:
    TagComponent() = default;
    explicit TagComponent(const std::string& tag) : mTag(tag) {}

    std::string mTag;
};

class TransformComponent final
{
public:  
    TransformComponent() = default;
    explicit TransformComponent(const glm::vec3& pos, const glm::quat& q = glm::quat(), const glm::vec3& sc = glm::vec3(1.0f, 1.0f, 1.0f)) : mPos(pos), mRot(q), mScale(sc) {}

    glm::vec3 mPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat mRot = glm::quat();
    glm::vec3 mScale = glm::vec3(1.0f, 1.0f, 1.0f);
};

class DirectLightComponent final
{
public:
    DirectLightComponent() = default;
    explicit DirectLightComponent(const glm::vec3& pos, const glm::vec3& color = glm::vec3()) : mPos(pos), mColor(color) {}

    glm::vec3 mPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 mColor = glm::vec3();
};

class StaticMeshComponent final
{
public:
    StaticMeshComponent() = default;
    explicit StaticMeshComponent(const StaticMeshRenderablePtr& mdl) : m_model(mdl) {}

    StaticMeshRenderablePtr m_model;
    ShaderProgramPtr m_shader;
    StructuredBufferPtr m_sb;
};

class SkinnedMeshComponent final
{
public:
    SkinnedMeshComponent() = default;

    StaticMeshRenderable m_model;

    ShaderProgramPtr m_shader;
    StructuredBufferPtr m_sb;
};

class SpriteComponent final
{
public:
    SpriteComponent() = default;

    glm::vec2 mPos;
    glm::vec3 mColor;

    //TextureAtlasID id;
    //glm::vec2 uv;
};

